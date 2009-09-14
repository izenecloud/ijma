/** \file pos_table.cpp
 * Implementation of class POSTable.
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Sep 11, 2009
 */

#include "pos_table.h"

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
bool POSTable::loadConfig(const char* fileName)
{
    assert(fileName);

    // remove the previous table if exists
    posAlphabetTable_.clear();

    // open file
    ifstream from(fileName);
    if(! from)
    {
        return false;
    }

    // read file
    string line, japanesePOS, alphabetPOS;
    string::size_type i, j;
    const char* whitespaces = " \t";

    // to convert from index string to int value
    istringstream iss;
    int indexValue;

#if JMA_DEBUG_PRINT
    cout << "load POS table: " << fileName << endl;
    cout << "JapanesePOS\tindex\tAlphabet" << endl;
#endif

    // each line is assumed in the format "JapanesePOS index AlphabetPOS",
    // those lines not in this format would be ignored
    while(getline(from, line))
    {
        line = line.substr(0, line.find('\r'));

        if(line.empty())
            continue;

        // set JapanesePOS
        j = line.find_first_of(whitespaces);
        if(j == string::npos)
            continue;
        japanesePOS = line.substr(0, j);

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
        {
            alphabetPOS = line.substr(i);
        }
        else
        {
            alphabetPOS = line.substr(i, j-i);
        }

#if JMA_DEBUG_PRINT
        cout << japanesePOS << "\t" << indexValue << "\t" << alphabetPOS << endl;
#endif

        if(indexValue < 0)
        {
            cerr << "the index code of POS (" << japanesePOS << ", " << alphabetPOS << ") should not be a negative value: " << indexValue << endl;
            continue;
        }

        if(static_cast<unsigned int>(indexValue) >= posAlphabetTable_.size())
        {
            posAlphabetTable_.resize(indexValue+1);
        }
        posAlphabetTable_[indexValue] = alphabetPOS;
    }

#if JMA_DEBUG_PRINT
    cout << endl;
#endif

    return true;
}

const char* POSTable::getAlphabetPOS(int index) const
{
    if(index < 0 || static_cast<unsigned int>(index) >= posAlphabetTable_.size())
	return "";

    return posAlphabetTable_[index].c_str();
}

} // namespace jma
