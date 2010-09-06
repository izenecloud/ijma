/** \file jma_dictionary.cpp
 * Implementation of class JMA_Dictionary.
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Aug 02, 2010
 */

#include "jma_dictionary.h"
#include "file_utils.h"

#include "mmap.h" // MeCab::Mmap
#include "utils.h" // MeCab::read_static
#include "scoped_ptr.h" // MeCab::scoped_array

#include "zwrapper.h" // zlib::ZWrapper

#include <iostream>
#include <fstream>
#include <strstream> // ostringstream
#include <utility> // make_pair
#include <cstring>
#include <cassert>

using namespace std;

namespace
{
/** System dictionary archive */
const char* DICT_ARCHIVE_FILE = "sys.bin";

/** Prefix of user dictionary file name */
const char* USER_DICT_PREFIX = "userdic_";

const unsigned int JMA_DICT_VERSION = 100;
const unsigned int JMA_DICT_BLOCK_SIZE = 512;
const unsigned int JMA_DICT_BLOCK_MASK = JMA_DICT_BLOCK_SIZE - 1;
const unsigned int JMA_DICT_FILE_NAME_SIZE = 500;

/**
 * Round to multiple of JMA_DICT_BLOCK_SIZE.
 * \param value any size such as 514
 * \return the rounded value such as 1024
 */
inline unsigned int roundBlockSize(unsigned int value)
{
    return (value + JMA_DICT_BLOCK_MASK) & ~JMA_DICT_BLOCK_MASK;
}

/**
 * Create file name from user dictionary.
 * \param prefix the prefix of file name, such as "userdic_"
 * \param index the unique index of user dictionary
 * \return the file name combining prefix and index, such as "userdic_1"
 */
inline string createFileName(const char* prefix, unsigned int index)
{
    ostringstream ost;
    ost << prefix << index;
    return ost.str();
}

class Buffer
{
public:
    Buffer(unsigned int s)
        : size_(s) {
        buffer_ = new char[size_];
        reset();
    }

    ~Buffer() {
        delete[] buffer_;
    }

    const char* get() const {
        return buffer_;
    }

    unsigned int size() const {
        return size_;
    }

    unsigned int pos() const {
        return pos_;
    }

    void advance(unsigned int len) {
        pos_ += len;
    }

    void reset() {
        pos_ = 0;
        memset(buffer_, 0, size_);
    }

    template<class T> void put(const T* p) {
        put(p, sizeof(T));
    }

    template<class T> void put(const T* p, unsigned int len) {
        if(len > size_ - pos_) {
            len = size_ - pos_;
        }

        memcpy(buffer_+pos_, p, len);
        pos_ += len;
    }

    void read(ifstream& ifs) {
        if(! ifs)
            return;

        ifs.read(buffer_+pos_, size_ - pos_);
        pos_ += ifs.gcount();
    }

private:
    char* buffer_;
    const unsigned int size_;
    unsigned int pos_;
};

}

namespace jma
{

JMA_Dictionary* JMA_Dictionary::instance_;

JMA_Dictionary* JMA_Dictionary::instance()
{
    if(instance_ == 0)
    {
        instance_ = new JMA_Dictionary;
    }

    return instance_;
}

JMA_Dictionary::JMA_Dictionary()
{
}

JMA_Dictionary::~JMA_Dictionary()
{
    mutex_.lock();
    for(ArchiveMap::iterator it=archiveMap_.begin(); it!=archiveMap_.end(); ++it)
    {
        delete[] it->second.startAddr_;
    }
    archiveMap_.clear();
    mutex_.unlock();
}

bool JMA_Dictionary::open(const char* dirName)
{
    assert(dirName);

    string archiveName = createFilePath(dirName, DICT_ARCHIVE_FILE);
    string dirPath = normalizeDirPath(dirName);

    mutex_.lock();
    ArchiveMap::iterator it = archiveMap_.find(dirPath);
    if(it != archiveMap_.end())
    {
        it->second.refCount_++;
        mutex_.unlock();
        return true;
    }

    DictArchive archive; // reference count is initialized to 1

    // mapping from compressed file into memory
    MeCab::Mmap<char> compressFile;
    if(! compressFile.open(archiveName.c_str()))
    {
        cerr << "error: fail to memory map file " << archiveName << endl;
        mutex_.unlock();
        return false;
    }
    if(compressFile.size() < JMA_DICT_BLOCK_SIZE)
    {
        cerr << "error: dictionary file " << archiveName << " is broken." << endl;
        mutex_.unlock();
        return false;
    }

    const char* start = compressFile.begin();
    const char* ptr = start;

    // read total head
    unsigned int version, fileCount, totalSize;
    MeCab::read_static<unsigned int>(&ptr, version);
    if(version != JMA_DICT_VERSION)
    {
        cerr << "error: dictionary file " << archiveName << " is broken or its version is unsupported." << endl;
        mutex_.unlock();
        return false;
    }
    MeCab::read_static<unsigned int>(&ptr, fileCount);
    MeCab::read_static<unsigned int>(&ptr, totalSize);
    ptr = start + JMA_DICT_BLOCK_SIZE;

    // uncompress
    MeCab::scoped_array<char> sysDictAddr(new char[totalSize]);
    if(! sysDictAddr.get())
    {
        cerr << "error: fail to allcate memory " << totalSize << " bytes to uncompress file " << archiveName << endl;
        mutex_.unlock();
        return false;
    }

    zlib::ZWrapper zwrap;
    unsigned int uncompSize = totalSize;
    if(! zwrap.uncompress(ptr, compressFile.end() - ptr, sysDictAddr.get(), uncompSize))
    {
        cerr << "error: fail to uncompress file " << archiveName << endl;
        mutex_.unlock();
        return false;
    }
    if(uncompSize != totalSize)
    {
        cerr << "error: uncompress size " << uncompSize << " , while it should be " << totalSize << endl;
        mutex_.unlock();
        return false;
    }

    // read each file head
    ptr = sysDictAddr.get();
    const char* content = ptr + JMA_DICT_BLOCK_SIZE * fileCount;
    for(unsigned int i=0; i<fileCount; ++i)
    {
        // file name is the 1st field in head section
        const char* head = ptr;
        DictUnit& dict = archive.dictMap_[head];
        dict.fileName_ = head;

        ptr += JMA_DICT_FILE_NAME_SIZE;
        MeCab::read_static<unsigned int>(&ptr, dict.length_);

        dict.text_ = const_cast<char *>(content);

        // round to multiple of block size
        content += roundBlockSize(dict.length_);
        ptr = head + JMA_DICT_BLOCK_SIZE;
    }

    // check end position
    if(content != sysDictAddr.get() + totalSize)
    {
        cerr << "error: dictionary file " << archiveName << " is broken at end position." << endl;
        mutex_.unlock();
        return false;
    }

    pair<ArchiveMap::iterator, bool> ret = archiveMap_.insert(make_pair(dirPath, archive));
    // assign start address only when inserted successfully
    if(ret.second)
        ret.first->second.startAddr_ = sysDictAddr.unbind();

    mutex_.unlock();
    return ret.second;
}

bool JMA_Dictionary::close(const char* dirName)
{
    assert(dirName);

    bool result = false;
    string dirPath = normalizeDirPath(dirName);

    mutex_.lock();
    ArchiveMap::iterator it = archiveMap_.find(dirPath);
    if(it != archiveMap_.end())
    {
        it->second.refCount_--;
        if(it->second.refCount_ == 0)
        {
            delete[] it->second.startAddr_;
            archiveMap_.erase(it);
        }
        result = true;
    }
    mutex_.unlock();

    return result;
}

const DictUnit* JMA_Dictionary::getDict(const char* fileName) const
{
    assert(fileName);

    const DictUnit* result = 0;
    string dirStr = getDirPath(fileName);
    ArchiveMap::const_iterator archiveIt = archiveMap_.find(dirStr);
    if(archiveIt != archiveMap_.end())
    {
        string fileStr = getFileName(fileName);
        DictMap::const_iterator dictIt = archiveIt->second.dictMap_.find(fileStr);
        if(dictIt != archiveIt->second.dictMap_.end())
            result = &dictIt->second;
    }

    return result;
}

/**
 * Archive format.
 * All the [SECTION] below are rounded up to 512 bytes,
 * the extra space is zero filled.
 *
 * [TOTAL HEAD]
 * version, 4 bytes
 * file count, 4 bytes
 * total size, 4 bytes (the uncompressed size of all sections below)
 *
 * multiple [FILE HEAD] (compressed)
 * file name, 500 bytes (terminated with null)
 * file size in bytes, 4 bytes (maximum file size is 4 Gigabytes)
 *
 * multiple [FILE CONTENT] (compressed)
 */
bool JMA_Dictionary::compile(const std::vector<std::string>& srcFiles, const char* destFile)
{
    assert(destFile);

    if(srcFiles.empty())
    {
        cerr << "error: no dictionary files to compile into archive." << endl;
        return false;
    }

    ofstream ofs(destFile, ios::binary);
    if(! ofs)
    {
        cerr << "error: fail to open file " << destFile << endl;
        return false;
    }

    Buffer buffer(JMA_DICT_BLOCK_SIZE);

    // total head
    buffer.put(&JMA_DICT_VERSION);
    unsigned int fileCount = srcFiles.size();
    buffer.put(&fileCount);
    // the uncompressed size of all sections below
    unsigned int totalSize = JMA_DICT_BLOCK_SIZE * fileCount;
    const streampos totalSizePos = buffer.pos();
    ofs.write(buffer.get(), buffer.size());

    // compressor
    zlib::ZWrapper zwrap;
    if(! zwrap.defalteInit())
    {
        cerr << "error: fail to initialize compress state." << endl;
        return false;
    }

    // each file head
    for(unsigned int i=0; i<fileCount; ++i)
    {
        buffer.reset();
        string fileName = getFileName(srcFiles[i]);
        if(fileName.length() < JMA_DICT_FILE_NAME_SIZE)
        {
            buffer.put(fileName.c_str(), fileName.length());
            buffer.advance(JMA_DICT_FILE_NAME_SIZE - fileName.length());
        }
        else
        {
            // terminated with null
            buffer.put(fileName.c_str(), JMA_DICT_FILE_NAME_SIZE - 1);
        }

        ifstream ifs(srcFiles[i].c_str(), ios::binary | ios::ate);
        if(! ifs)
        {
            cerr << "error: cannot open source file " << srcFiles[i] << endl;
            return false;
        }
        unsigned int fileSize = ifs.tellg();
        buffer.put(&fileSize);

        // round to multiple of block size
        totalSize += roundBlockSize(fileSize);

        if(! zwrap.deflateToStream(buffer.get(), buffer.size(), ofs, false))
        {
            cerr << "error: fail to compress into output file " << destFile << endl;
            return false;
        }
    }

    // each file content
    for(unsigned int i=0; i<fileCount; ++i)
    {
        ifstream ifs(srcFiles[i].c_str(), ios::binary);
        if(! ifs)
        {
            cerr << "error: cannot open source file " << srcFiles[i] << endl;
            return false;
        }

        while(ifs)
        {
            buffer.reset();
            buffer.read(ifs);
            bool isFlush = (i == fileCount - 1) && ifs.eof();
            if(! zwrap.deflateToStream(buffer.get(), buffer.size(), ofs, isFlush))
            {
                cerr << "error: fail to compress into output file " << destFile << endl;
                return false;
            }
        }
    }

    if (! zwrap.deflateEnd())
    {
        cerr << "error: fail to free compress state." << endl;
        return false;
    }

    ofs.seekp(totalSizePos);
    ofs.write(reinterpret_cast<char *>(&totalSize), sizeof(totalSize));

    ofs.close();
    return true;
}

JMA_UserDictionary* JMA_UserDictionary::instance_;

JMA_UserDictionary* JMA_UserDictionary::instance()
{
    if(instance_ == 0)
    {
        instance_ = new JMA_UserDictionary;
    }

    return instance_;
}

JMA_UserDictionary::JMA_UserDictionary()
    : index_(0)
{
}

JMA_UserDictionary::~JMA_UserDictionary()
{
    mutex_.lock();
    for(DictMap::iterator it=userDictMap_.begin(); it!=userDictMap_.end(); ++it)
    {
        delete[] it->second.text_;
    }
    userDictMap_.clear();
    mutex_.unlock();
}

bool JMA_UserDictionary::create(std::string& newName)
{
    DictUnit newDict;

    mutex_.lock();
    newName = createFileName(USER_DICT_PREFIX, index_++);
    newDict.fileName_ = newName;
    pair<DictMap::iterator, bool> ret = userDictMap_.insert(make_pair(newName, newDict));
    mutex_.unlock();

    return ret.second;
}

bool JMA_UserDictionary::release(const char* fileName)
{
    assert(fileName);

    bool result = false;
    mutex_.lock();
    DictMap::iterator it = userDictMap_.find(fileName);
    if(it != userDictMap_.end())
    {
        delete[] it->second.text_;
        userDictMap_.erase(it);
        result = true;
    }
    mutex_.unlock();

    return result;
}

const DictUnit* JMA_UserDictionary::getDict(const char* fileName) const
{
    assert(fileName);

    const DictUnit* result = 0;
    DictMap::const_iterator it = userDictMap_.find(fileName);
    if(it != userDictMap_.end())
        result = &it->second;

    return result;
}

bool JMA_UserDictionary::copyStrToDict(const std::string& str, const char* fileName)
{
    assert(fileName);

    bool result = false;
    mutex_.lock();
    DictMap::iterator it = userDictMap_.find(fileName);
    if(it != userDictMap_.end())
    {
        DictUnit* dict = &it->second;

        // in case of already allocated
        delete[] dict->text_;

        dict->length_ = str.size();
        dict->text_ = new char[dict->length_];
        if(str.copy(dict->text_, dict->length_) == dict->length_)
            result = true;
    }
    mutex_.unlock();

    return result;
}

} // namespace jma
