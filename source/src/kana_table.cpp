/** \file kana_table.cpp
 * Implementation of class KanaTable.
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Feb 10, 2010
 */

#include "kana_table.h"
#include "iconv_utils.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>

#ifndef JMA_DEBUG_PRINT
    #define JMA_DEBUG_PRINT 1
#endif

using namespace std;

namespace jma
{
KanaTable::KanaTable()
{
}

bool KanaTable::loadConfig(const char* fileName, Knowledge::EncodeType src, Knowledge::EncodeType dest)
{
    assert(fileName);

    Iconv iconv;
    if(! iconv.open(src, dest))
    {
        cerr << "error to open encoding conversion from " << src << " to " << dest << endl;
        return false;
    }

    // remove the previous tables if exist
    mapToHiragana_.clear();
    mapToKatakana_.clear();

    // open file
    ifstream from(fileName);
    if(! from)
    {
        return false;
    }

    // read file
    string line, hira, kata;
    istringstream iss;

#if JMA_DEBUG_PRINT
    cout << "load kana mapping table: " << fileName << endl;
    cout << "src charset: " << src << endl;
    cout << "dest charset: " << dest << endl;
    cout << "Hiragana\tKatakana" << endl;
#endif

    // each line is assumed in the format "Hiragana Katakana",
    // those lines not in this format would be ignored
    while(getline(from, line))
    {
        if (line.empty() || line[0] == ';' || line[0] == '#')
            continue;

        iss.clear();
        iss.str(line);
        iss >> hira >> kata;

        if(! iss)
        {
            cerr << "ignore the invalid kana mapping line: " << line << endl;
            continue;
        }

        // convert encoding
        if(! iconv.convert(&hira))
        {
            cerr << "error to convert encoding from " << src << " to " << dest << " for character " << hira << endl;
            return false;
        }
        if(! iconv.convert(&kata))
        {
            cerr << "error to convert encoding from " << src << " to " << dest << " for character " << kata << endl;
            return false;
        }

#if JMA_DEBUG_PRINT
        cout << hira << "\t" << kata << endl;
#endif

        mapToHiragana_[kata] = hira;
        mapToKatakana_[hira] = kata;
    }

    assert(mapToHiragana_.size() == mapToKatakana_.size() && "the Kana mapping table size should be the same.");
#if JMA_DEBUG_PRINT
    cout << "total " << mapToHiragana_.size() << " kanas mapping are loaded" << endl << endl;
#endif

    return true;
}

const char* KanaTable::toHiragana(const char* str) const
{
    KanaMap::const_iterator it = mapToHiragana_.find(str);
    if(it == mapToHiragana_.end())
        return 0;

    return it->second.c_str();
}

const char* KanaTable::toKatakana(const char* str) const
{
    KanaMap::const_iterator it = mapToKatakana_.find(str);
    if(it == mapToKatakana_.end())
        return 0;

    return it->second.c_str();
}

} // namespace jma
