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

#include <iostream>
#include <fstream>
#include <cstring>
#include <cassert>
using namespace std;

namespace
{
unsigned int JMA_DICT_VERSION = 100;
unsigned int JMA_DICT_BLOCK_SIZE = 512;
unsigned int JMA_DICT_FILE_NAME_SIZE = 500;

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
    : mmap_(0)
{
}

JMA_Dictionary::~JMA_Dictionary()
{
    close();
}

bool JMA_Dictionary::open(const char* fileName)
{
    assert(fileName);

    close();

    mmap_ = new MeCab::Mmap<char>;
    if(! mmap_->open(fileName))
    {
        cerr << "error: fail to memory map file " << fileName << endl;
        return false;
    }
    if(mmap_->size() < JMA_DICT_BLOCK_SIZE)
    {
        cerr << "error: dictionary file " << fileName << " is broken." << endl;
        return false;
    }

    const char* start = mmap_->begin();
    const char* ptr = start;

    // read total head
    unsigned int version;
    unsigned int fileCount;
    MeCab::read_static<unsigned int>(&ptr, version);
    if(version != JMA_DICT_VERSION)
    {
        cerr << "error: dictionary file " << fileName << " is broken or its version is unsupported." << endl;
        return false;
    }
    MeCab::read_static<unsigned int>(&ptr, fileCount);
    ptr = start + JMA_DICT_BLOCK_SIZE;

    // read each file head
    const char* content = ptr + JMA_DICT_BLOCK_SIZE * fileCount;
    const unsigned int mask = JMA_DICT_BLOCK_SIZE - 1;
    for(unsigned int i=0; i<fileCount; ++i)
    {
        // file name is the 1st field in head section
        const char* head = ptr;
        DictUnit& dict = dictMap_[head];
        dict.fileName_ = head;

        ptr += JMA_DICT_FILE_NAME_SIZE;
        MeCab::read_static<unsigned int>(&ptr, dict.length_);

        dict.text_ = const_cast<char *>(content);

        // round to multiple of block size (power of 2)
        content += (dict.length_ + mask) & ~mask;
        ptr = head + JMA_DICT_BLOCK_SIZE;
    }

    // check end position
    if(content != mmap_->end())
    {
        cerr << "error: dictionary file " << fileName << " is broken at end position." << endl;
        return false;
    }
    return true;
}

void JMA_Dictionary::close()
{
    dictMap_.clear();
    delete mmap_;
    mmap_ = 0;
}

const DictUnit* JMA_Dictionary::getDict(const char* fileName) const
{
    string str = getFileName(fileName);
    std::map<std::string, DictUnit>::const_iterator it = dictMap_.find(str);
    if(it != dictMap_.end())
        return &it->second;

    return 0;
}

/**
 * Archive format.
 * All the [SECTION] below are rounded up to 512 bytes,
 * the extra space is zero filled.
 *
 * [TOTAL HEAD]
 * version, 4 bytes
 * file count, 4 bytes
 *
 * multiple [FILE HEAD]
 * file name, 500 bytes (terminated with null)
 * file size in bytes, 4 bytes (maximum file size is 4 Gigabytes)
 *
 * multiple [FILE CONTENT]
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
    ofs.write(buffer.get(), buffer.size());

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

        ofs.write(buffer.get(), buffer.size());
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
            ofs.write(buffer.get(), buffer.size());
        }
    }

    ofs.close();
    return true;
}

} // namespace jma
