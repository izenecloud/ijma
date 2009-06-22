/** \file jma_knowledge.cpp
 * Implementation of class JMA_Knowledge.
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Jun 17, 2009
 */

#include "jma_knowledge.h"
#include "sentence.h"
#include "dictionary.h"

#include "mecab.h" // MeCab::Tagger

#include <iostream>
//#include <sstream>
//#include <string>
#include <cassert>

#if defined(_WIN32) && !defined(__CYGWIN__)
#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif
#endif

#define JMA_DEBUG_PRINT 1

using namespace std;

namespace jma
{

JMA_Knowledge::JMA_Knowledge()
    : tagger_(0)
{
}

JMA_Knowledge::~JMA_Knowledge()
{
    clear();
}

void JMA_Knowledge::clear()
{
    // destroy the tagger if exists
    if(tagger_)
    {
        delete tagger_;
        tagger_ = 0;
    }

    // remove the temporary file if exists
    if(! tempUserDic_.empty())
    {
        int r = unlink(tempUserDic_.c_str());
        assert(r == 0 && "temporary file is removed successfully");
    }
}

int JMA_Knowledge::loadDict()
{
    const size_t userDictNum = userDictNames_.size();

#if JMA_DEBUG_PRINT
    cout << "JMA_Knowledge::loadDict()" << endl;
    cout << "system dictionary path: " << systemDictPath_ << endl;
    if(userDictNum != 0)
    {
        cout << "user dictionary: ";
        for(size_t i=0; i<userDictNum; ++i)
        {
            cout << userDictNames_[i] << " ";
        }
        cout << endl;
    }
#endif

    // remove the previous tagger and temporary file
    clear();

    // construct parameter to create tagger
    string taggerParam("-d ");
    taggerParam += systemDictPath_;

    if(userDictNum != 0)
    {
        // create a unique temporary file for user dictionary in binary type
        bool tempResult = createTempFile(tempUserDic_);
        if(! tempResult)
        {
            cerr << "fail to create a temporary user ditionary file" << endl;
            return false;
        }
#if JMA_DEBUG_PRINT
        cout << "temporary file name of binary user dictionary: " << tempUserDic_ << endl;
#endif

        // construct parameter to compile user dictionary
        vector<char*> compileParam;
        compileParam.push_back("JMA_Knowledge");
        compileParam.push_back("-d");
        compileParam.push_back(const_cast<char*>(systemDictPath_.c_str()));
        compileParam.push_back("-u");
        compileParam.push_back(const_cast<char*>(tempUserDic_.c_str()));

        // set the encoding type,
        // the encoding type of user dictionary both in text type and binary type are the same, which is set by Knowledge::setEncodeType().
        // if it is not set by Knowledge::setEncodeType() before, it would be "euc-jp" defaultly.
        EncodeType userEncode = getEncodeType();
        compileParam.push_back("-f");
        compileParam.push_back(const_cast<char*>(ENCODE_TYPE_STR_[userEncode]));
        compileParam.push_back("-t");
        compileParam.push_back(const_cast<char*>(ENCODE_TYPE_STR_[userEncode]));

        // append source files of user dictionary
        for(size_t i=0; i<userDictNum; ++i)
        {
            compileParam.push_back(const_cast<char*>(userDictNames_[i].c_str()));
        }

#if JMA_DEBUG_PRINT
        cout << "parameter of mecab_dict_index() to compile user dictionary: ";
        for(size_t i=0; i<compileParam.size(); ++i)
        {
            cout << compileParam[i] << " ";
        }
        cout << endl;
#endif

        // compile user dictionary files into binary type
        int compileResult = mecab_dict_index(compileParam.size(), &compileParam[0]);
        if(compileResult != 0)
        {
            cerr << "fail to compile user ditionary" << endl;
            return 0;
        }

        // append the name of user dictionary binary file to the parameter of tagger creation
        taggerParam += " -u ";
        taggerParam += tempUserDic_;
    }

#if JMA_DEBUG_PRINT
    cout << "parameter of MeCab::createTagger() to create MeCab::Tagger: " << taggerParam << endl;
#endif

    // create tagger by loading dictioanry files
    tagger_ = MeCab::createTagger(taggerParam.c_str());

    return tagger_ ? 1 : 0;
}

int JMA_Knowledge::loadStopWordDict(const char* fileName)
{
    // the lines below (from CMA code) is commented out,
    // this function needs to be implemented in JMA.
    //stopWordDict_ = new Dictionary;
    //bool r = stopWordDict_->load(fileName);
    bool r = 0;

    return r ? 1 : 0;
}

int JMA_Knowledge::loadConfig(const char* fileName)
{
    // the lines below (from CMA code) is commented out,
    // this function needs to be implemented in JMA.
    //POSTable* posTable = POSTable::instance();
    //bool r = posTable->loadConfig(fileName);
    bool r = 0;

    return r ? 1 : 0;
}

int JMA_Knowledge::encodeSystemDict(const char* txtDirPath, const char* binDirPath)
{
    assert(txtDirPath && binDirPath);

#if JMA_DEBUG_PRINT
    cout << "JMA_Knowledge::encodeSystemDict()" << endl;
    cout << "path of source system dictionary: " << txtDirPath << endl;
    cout << "path of binary system dictionary: " << binDirPath << endl;
#endif

    // if the destination directory is not exist, create it (TO BE DONE)

    // construct parameter to compile system dictionary
    vector<char*> compileParam;
    compileParam.push_back("JMA_Knowledge");
    compileParam.push_back("-d");
    compileParam.push_back(const_cast<char*>(txtDirPath));
    compileParam.push_back("-o");
    compileParam.push_back(const_cast<char*>(binDirPath));

    // the source encoding type could be predefined by the "dictionary-charset" entry in "dicrc" file,
    // if the source encoding type is not predefined in "dicrc", it would be "euc-jp" defaultly.
    // below is to set the destination encoding type, which is "euc-jp" defaultly.
    compileParam.push_back("-t");
    compileParam.push_back(const_cast<char*>(ENCODE_TYPE_STR_[getEncodeType()]));

#if JMA_DEBUG_PRINT
    cout << "parameter of mecab_dict_index() to compile system dictionary: ";
    for(size_t i=0; i<compileParam.size(); ++i)
    {
        cout << compileParam[i] << " ";
    }
    cout << endl;
#endif

    // compile system dictionary files into binary type
    int compileResult = mecab_dict_index(compileParam.size(), &compileParam[0]);
    if(compileResult != 0)
    {
        cerr << "fail to compile system ditionary" << endl;
        return 0;
    }

    // copy dicrc, rewrite.def, left-id.def, right-id.def to the destination directory (TO BE DONE)

    return 1;
}

MeCab::Tagger* JMA_Knowledge::getTagger() const
{
    return tagger_;
}

bool JMA_Knowledge::createTempFile(std::string& tempName)
{
#if defined(_WIN32) && !defined(__CYGWIN__)
    // directory name for temporary file
    char dirName[MAX_PATH];

    // retrieve the directory path designated for temporary files
    DWORD dirResult = GetTempPath(MAX_PATH, dirName);
    if(dirResult == 0)
    {
        cerr << "fail in GetTempPath() to get the directory path for temporary file" << endl;
        return false;
    }

    // file name for temporary file
    char fileName[MAX_PATH];

    // create a unique temporary file 
    UINT nameResult = GetTempFileName(dirName, "jma", 0, fileName);
    if(nameResult == 0)
    {
        cerr << "fail in GetTempFileName() to create a temporary file" << endl;
        return false;
    }

    tempName = fileName;
#else
    // file name for temporary file
    char fileName[] = "/tmp/jmaXXXXXX";

    // create a unique temporary file 
    int tempFd = mkstemp(fileName);
    if(tempFd == -1)
    {
        cerr << "fail in mkstemp() to create a temporary file" << endl;
        return false;
    }

    tempName = fileName;
#endif

    return true;
}

} // namespace jma
