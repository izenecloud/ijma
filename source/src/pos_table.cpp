/** \file pos_table.cpp
 * Implementation of class POSTable.
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Sep 11, 2009
 */

#include "pos_table.h"
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
POSTable::POSTable()
    : strTableVec_(POS_FORMAT_NUM), tableSize_(0)
{
}

bool POSTable::loadConfig(const char* fileName, Knowledge::EncodeType src, Knowledge::EncodeType dest)
{
    assert(fileName);

    Iconv iconv;
    if(! iconv.open(src, dest))
    {
        cerr << "error to open encoding conversion from " << src << " to " << dest << endl;
        return false;
    }

    // remove the previous table if exists
    tableSize_ = 0;
    for(unsigned int i=0; i<strTableVec_.size(); ++i)
        strTableVec_[i].clear();

    // open file
    ifstream from(fileName);
    if(! from)
    {
        return false;
    }

    // read file
    string line, fullPOS, partPOS, alphabetPOS;
    string::size_type i, j, k;
    const char* whitespaces = " \t";

    // to convert from index string to int value
    istringstream iss;
    int indexValue;

#if JMA_DEBUG_PRINT
    cout << "load POS table: " << fileName << endl;
    cout << "src charset: " << src << endl;
    cout << "dest charset: " << dest << endl;
    cout << "fullPOS\t\tindex\tAlphabet partPOS" << endl;
#endif

    // each line is assumed in the format "fullPOS index AlphabetPOS",
    // those lines not in this format would be ignored
    while(getline(from, line))
    {
        line = line.substr(0, line.find('\r'));

        if(line.empty())
            continue;

        // set fullPOS
        j = line.find_first_of(whitespaces);
        if(j == string::npos)
            continue;
        fullPOS = line.substr(0, j);

        k = fullPOS.find('*');
        if(k == string::npos)
            partPOS = fullPOS;
        else if(k == 0)
            partPOS = "";
        else
            partPOS = fullPOS.substr(0, k-1); // get "AAA" from "AAA,*"

        // set index
        i = line.find_first_not_of(whitespaces, j+1);
        if(i == string::npos)
            continue;
        j = line.find_first_of(whitespaces, i);
        if(j == string::npos)
            continue;
        iss.clear();
        iss.str(line.substr(i, j-i));
        iss >> indexValue;

        // set alphabetPOS
        i = line.find_first_not_of(whitespaces, j+1);
        if(i == string::npos)
            continue;
        j = line.find_first_of(whitespaces, i);
        if(j == string::npos)
            alphabetPOS = line.substr(i);
        else
            alphabetPOS = line.substr(i, j-i);

        // convert encoding
        if(! iconv.convert(&partPOS) || !iconv.convert(&fullPOS))
        {
            cerr << "error to convert encoding from " << src << " to " << dest << endl;
            return false;
        }
#if JMA_DEBUG_PRINT
        cout << fullPOS << "\t" << indexValue << "\t" << alphabetPOS << "\t" << partPOS << endl;
#endif

        if(indexValue < 0)
        {
            cerr << "the index code of POS (" << fullPOS << ", " << alphabetPOS << ") should not be a negative value: " << indexValue << endl;
            continue;
        }

        if(indexValue >= tableSize_)
        {
            tableSize_ = indexValue + 1;
            for(unsigned int i=0; i<strTableVec_.size(); ++i)
                strTableVec_[i].resize(tableSize_);
        }

        strTableVec_[POS_FORMAT_DEFAULT][indexValue] = partPOS;
        strTableVec_[POS_FORMAT_ALPHABET][indexValue] = alphabetPOS;
        strTableVec_[POS_FORMAT_FULL_CATEGORY][indexValue] = fullPOS;
    }

#if JMA_DEBUG_PRINT
    cout << "total " << tableSize_ << " POS loaded" << endl << endl;
#endif

    return true;
}

const char* POSTable::getPOS(int index, POSFormat format) const
{
    if(index < 0 || index >= tableSize_)
        return "";

    return strTableVec_[format][index].c_str();
}

} // namespace jma
