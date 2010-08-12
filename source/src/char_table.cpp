/** \file char_table.cpp
 * Implementation of class CharTable.
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Aug 12, 2010
 */

#include "char_table.h"
#include "jma_dictionary.h"
#include "knowledge.h" // Knowledge::encodeStr()
#include "iconv_utils.h" // MeCab::Iconv

#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <strstream>

#ifndef JMA_DEBUG_PRINT
    #define JMA_DEBUG_PRINT 1
#endif

using namespace std;

namespace jma
{
CharTable::CharTable()
{
}

bool CharTable::loadConfig(const char* fileName, Knowledge::EncodeType src, Knowledge::EncodeType dest)
{
    assert(fileName);

    MeCab::Iconv iconv;
    if(! iconv.open(Knowledge::encodeStr(src), Knowledge::encodeStr(dest)))
    {
        cerr << "error to open encoding conversion from " << src << " to " << dest << endl;
        return false;
    }

    // remove the previous tables if exist
    mapToLeft_.clear();
    mapToRight_.clear();

    // open file
    const DictUnit* dict = JMA_Dictionary::instance()->getDict(fileName);
    if(! dict)
    {
        cerr << "cannot find configuration file: " << fileName << endl;
        return false;
    }

    istrstream from(dict->text_, dict->length_);
    if(! from)
    {
        cerr << "cannot read configuration file: " << fileName << endl;
        return false;
    }

    // read file
    string line, left, right;
    istringstream iss;

#if JMA_DEBUG_PRINT
    cout << "load char mapping table: " << fileName << endl;
    cout << "src charset: " << src << endl;
    cout << "dest charset: " << dest << endl;
    cout << "Left\tRight" << endl;
#endif

    // each line is assumed in the format "LeftType RightType",
    // those lines not in this format would be ignored
    while(getline(from, line))
    {
        line = line.substr(0, line.find('\r'));
        if (line.empty() || line[0] == ';' || line[0] == '#')
            continue;

        iss.clear();
        iss.str(line);
        iss >> left >> right;

        if(! iss)
        {
            cerr << "ignore the invalid char mapping line: " << line << endl;
            continue;
        }

        // convert encoding
        if(! iconv.convert(&left))
        {
            cerr << "error to convert encoding from " << src << " to " << dest << " for character " << left << endl;
            return false;
        }
        if(! iconv.convert(&right))
        {
            cerr << "error to convert encoding from " << src << " to " << dest << " for character " << right << endl;
            return false;
        }

#if JMA_DEBUG_PRINT
        cout << left << "\t" << right << endl;
#endif

        mapToLeft_[right] = left;
        mapToRight_[left] = right;
    }

    assert(mapToLeft_.size() == mapToRight_.size() && "the char mapping table size should be the same.");
#if JMA_DEBUG_PRINT
    cout << "total " << mapToLeft_.size() << " char mapping are loaded" << endl << endl;
#endif

    return true;
}

const char* CharTable::toLeft(const char* str) const
{
    CharMap::const_iterator it = mapToLeft_.find(str);
    if(it == mapToLeft_.end())
        return 0;

    return it->second.c_str();
}

const char* CharTable::toRight(const char* str) const
{
    CharMap::const_iterator it = mapToRight_.find(str);
    if(it == mapToRight_.end())
        return 0;

    return it->second.c_str();
}

} // namespace jma
