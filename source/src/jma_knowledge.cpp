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

//#include <iostream>
//#include <sstream>
//#include <string>
#include <cassert>

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
    // the lines below (from CMA code) is commented out,
    // this function needs to be implemented in JMA.
    //userDict_ = new Dictionary;
    //bool r = userDict_->load(fileName);
    bool r = 0;

    return r ? 1 : 0;
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
