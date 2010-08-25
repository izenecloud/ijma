/** \file jma_dictionary.cpp
 * Implementation of class JMA_Dictionary.
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Aug 02, 2010
 */

#include "jma_dictionary.h"

#include "mmap.h" // MeCab::Mmap
#include "utils.h" // MeCab::read_static

#include "zwrapper.h" // zlib::ZWrapper

#include <iostream>
#include <fstream>
#include <cstring>
#include <cassert>
using namespace std;

namespace
{
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
 * Extract file name from path string.
 * \param path the path such as "/usr/bin/man", "c:\\windows\\winhelp.exe", or "man"
 * \return the file name such as "man", "winhelp.exe"
 */
inline string getFileName(const string& path)
{
    size_t last = path.find_last_of("/\\");

    if(last == string::npos)
        return path;

    return path.substr(last+1);
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
    : sysDictAddr_(0), binUserDict_(0), txtUserDict_(0)
{
}

JMA_Dictionary::~JMA_Dictionary()
{
    close();
}

void JMA_Dictionary::close()
{
    closeSysDict();
    closeBinUserDict();
    closeTxtUserDict();
}

void JMA_Dictionary::closeSysDict()
{
    dictMap_.clear();
    delete[] sysDictAddr_;
    sysDictAddr_ = 0;
}

void JMA_Dictionary::closeBinUserDict()
{
    if(binUserDict_)
    {
        delete[] binUserDict_->text_;
        delete binUserDict_;
        binUserDict_ = 0;
    }
}

void JMA_Dictionary::closeTxtUserDict()
{
    if(txtUserDict_)
    {
        delete[] txtUserDict_->text_;
        delete txtUserDict_;
        txtUserDict_ = 0;
    }
}

void JMA_Dictionary::createEmptyBinaryUserDict(const char* fileName)
{
    assert(fileName);

    closeBinUserDict();

    binUserDict_ = new DictUnit;
    binUserDict_->fileName_ = getFileName(fileName);
}

void JMA_Dictionary::createEmptyTextUserDict(const char* fileName)
{
    assert(fileName);

    closeTxtUserDict();

    txtUserDict_ = new DictUnit;
    txtUserDict_->fileName_ = getFileName(fileName);
}

bool JMA_Dictionary::open(const char* fileName)
{
    assert(fileName);

    closeSysDict();

    // mapping from compressed file into memory
    MeCab::Mmap<char> compressFile;
    if(! compressFile.open(fileName))
    {
        cerr << "error: fail to memory map file " << fileName << endl;
        return false;
    }
    if(compressFile.size() < JMA_DICT_BLOCK_SIZE)
    {
        cerr << "error: dictionary file " << fileName << " is broken." << endl;
        return false;
    }

    const char* start = compressFile.begin();
    const char* ptr = start;

    // read total head
    unsigned int version, fileCount, totalSize;
    MeCab::read_static<unsigned int>(&ptr, version);
    if(version != JMA_DICT_VERSION)
    {
        cerr << "error: dictionary file " << fileName << " is broken or its version is unsupported." << endl;
        return false;
    }
    MeCab::read_static<unsigned int>(&ptr, fileCount);
    MeCab::read_static<unsigned int>(&ptr, totalSize);
    ptr = start + JMA_DICT_BLOCK_SIZE;

    // uncompress
    sysDictAddr_ = new char[totalSize];
    if(! sysDictAddr_)
    {
        cerr << "error: fail to allcate memory " << totalSize << " bytes to uncompress file " << fileName << endl;
        return false;
    }

    zlib::ZWrapper zwrap;
    unsigned int uncompSize = totalSize;
    if(! zwrap.uncompress(ptr, compressFile.end() - ptr, sysDictAddr_, uncompSize))
    {
        cerr << "error: fail to uncompress file " << fileName << endl;
        return false;
    }
    if(uncompSize != totalSize)
    {
        cerr << "error: uncompress size " << uncompSize << " , while it should be " << totalSize << endl;
        return false;
    }

    // read each file head
    ptr = sysDictAddr_;
    const char* content = ptr + JMA_DICT_BLOCK_SIZE * fileCount;
    for(unsigned int i=0; i<fileCount; ++i)
    {
        // file name is the 1st field in head section
        const char* head = ptr;
        DictUnit& dict = dictMap_[head];
        dict.fileName_ = head;

        ptr += JMA_DICT_FILE_NAME_SIZE;
        MeCab::read_static<unsigned int>(&ptr, dict.length_);

        dict.text_ = const_cast<char *>(content);

        // round to multiple of block size
        content += roundBlockSize(dict.length_);
        ptr = head + JMA_DICT_BLOCK_SIZE;
    }

    // check end position
    if(content != sysDictAddr_ + totalSize)
    {
        cerr << "error: dictionary file " << fileName << " is broken at end position." << endl;
        return false;
    }
    return true;
}

DictUnit* JMA_Dictionary::getDict(const char* fileName)
{
    assert(fileName);

    string str = getFileName(fileName);
    std::map<std::string, DictUnit>::iterator it = dictMap_.find(str);
    if(it != dictMap_.end())
        return &it->second;

    if(binUserDict_ && binUserDict_->fileName_ == str)
        return binUserDict_;

    if(txtUserDict_ && txtUserDict_->fileName_ == str)
        return txtUserDict_;

    return 0;
}

bool JMA_Dictionary::copyStrToDict(const std::string& str, const char* fileName)
{
    DictUnit* dict = getDict(fileName);
    if(! dict)
        return false;

    // in case of already allocated
    delete[] dict->text_;

    dict->length_ = str.size();
    dict->text_ = new char[dict->length_];
    if(str.copy(dict->text_, dict->length_) != dict->length_)
        return false;

    return true;
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

} // namespace jma
