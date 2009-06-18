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

#ifndef JMA_DEBUG_PRINT
#define JMA_DEBUG_PRINT 1
#endif

using namespace std;

namespace jma
{

JMA_Knowledge::JMA_Knowledge()
    : tagger_(0)
{
}

JMA_Knowledge::~JMA_Knowledge()
{
    delete tagger_;
}

int JMA_Knowledge::loadDict()
{
    const size_t userDictNum = userDictNames_.size();

#if JMA_DEBUG_PRINT
    cout << "loading dictionary:" << endl;
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

    string param("-d ");
    param += systemDictPath_;

    if(userDictNum != 0)
    {
        vector<char*> compileParam;
        compileParam.push_back("JMA_Knowledge");
        compileParam.push_back("-d");
        compileParam.push_back(const_cast<char*>(systemDictPath_.c_str()));
        compileParam.push_back("-u");
        compileParam.push_back("tmp.dic");
        for(size_t i=0; i<userDictNum; ++i)
        {
            compileParam.push_back(const_cast<char*>(userDictNames_[i].c_str()));
        }

        int r = mecab_dict_index(compileParam.size(), &compileParam[0]);
        if(r != 0)
        {
            cerr << "fail in compiling user ditionary" << endl;
            return 0;
        }

        param += " -u ";
        param += "tmp.dic";
    }

#if JMA_DEBUG_PRINT
    cout << "parameter to create MeCab::Tagger: " << param << endl;
#endif

    if(tagger_)
    {
        delete tagger_;
    }

    tagger_ = MeCab::createTagger(param.c_str());

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

    // the lines below (from CMA code) is commented out,
    // this function needs to be implemented in JMA.
    //SystemDictionary dict;
    //if(dict.loadText(txtFileName) == false)
    //{
        //return 0;
    //}

    //if(dict.saveBinary(binFileName) == false)
    //{
        //return 0;
    //}

    return 1;
}

MeCab::Tagger* JMA_Knowledge::getTagger() const
{
    return tagger_;
}

} // namespace jma
