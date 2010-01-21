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

#define JMA_DEBUG_PRINT_COMBINE 0

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

        // convert encoding
        if(! iconv.convert(&fullPOS))
        {
            cerr << "error to convert encoding from " << src << " to " << dest << " for POS string " << fullPOS << endl;
            return false;
        }
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

        alphaPOSMap_[alphabetPOS] = indexValue;
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

bool POSTable::loadCombineRule(const char* fileName)
{
    assert(fileName);

    // remove the previous table if exists
    ruleVec_.clear();

    // open file
    ifstream from(fileName);
    if(! from)
    {
        return false;
    }

    // read file
    string line, pos1, pos2, target;
    istringstream iss;

#if JMA_DEBUG_PRINT
    cout << "load POS rule: " << fileName << endl;
    cout << "pos1\tpos2\ttarget" << endl;
#endif

    // each line is assumed in the format "pos1 pos2 target",
    // those lines not in this format would be ignored
    while(getline(from, line))
    {
        if (line.empty() || line[0] == ';' || line[0] == '#')
            continue;

        iss.clear();
        iss.str(line);
        iss >> pos1 >> pos2 >> target;

        // TODO is check iss OK?
        if(! iss)
        {
            cerr << "invalid format in rule: " << line << endl;
            cerr << "ignore this rule." << endl;
            continue;
        }

#if JMA_DEBUG_PRINT
        cout << pos1 << "\t" << pos2 << "\t" << target << endl;
#endif

        POSRule rule;
        rule.src1_ = getIndexFromAlphaPOS(pos1);
        rule.src2_ = getIndexFromAlphaPOS(pos2);
        rule.target_ = getIndexFromAlphaPOS(target);
        if(rule.src1_ < 0 || rule.src2_ < 0 || rule.target_ < 0)
        {
            cerr << "POS string is invalid in rule: " << line << endl;
            cerr << "ignore this rule." << endl;
            continue;
        }

        ruleVec_.push_back(rule);
    }

#if JMA_DEBUG_PRINT
    cout << ruleVec_.size() << " rules are loaded." << endl;
#endif

    return true;
}

int POSTable::combinePOS(int pos1, int pos2) const
{
    assert(pos1 >= 0 && pos2 >= 0 && "POS code to combine should not be negative.");

#if JMA_DEBUG_PRINT_COMBINE
    cout << strTableVec_[POS_FORMAT_ALPHABET][pos1] << " + " << strTableVec_[POS_FORMAT_ALPHABET][pos2] << " => ";
#endif

    for(vector<POSRule>::const_iterator it=ruleVec_.begin(); it!=ruleVec_.end(); ++it)
    {
        if(pos1 == it->src1_ && pos2 == it->src2_)
        {
#if JMA_DEBUG_PRINT_COMBINE
            cout << strTableVec_[POS_FORMAT_ALPHABET][it->target_] << endl;
#endif
            return it->target_;
        }
    }

#if JMA_DEBUG_PRINT_COMBINE
    cout << "no rule found" << endl;
#endif
    return -1;
}

int POSTable::getIndexFromAlphaPOS(const std::string& posStr) const
{
    map<std::string, int>::const_iterator it = alphaPOSMap_.find(posStr);
    if(it != alphaPOSMap_.end())
        return it->second;

    return -1;
}

} // namespace jma
