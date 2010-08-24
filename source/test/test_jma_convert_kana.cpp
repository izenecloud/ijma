/** \file test_jma_convert_kana.cpp
 * Test to convert Japanese Hiragana and Katakana characters.
 * Below is the usage examples:
 * \code
 * Test to convert characters.
 * $ ./test_jma_convert_kana --to hira,kata,half,full,lower,upper [--dict DICT_PATH]
 * \endcode
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Feb 10, 2010
 */

#include "jma_factory.h"
#include "analyzer.h"
#include "knowledge.h"
#include "test_jma_common.h" // TEST_JMA_DEFAULT_SYSTEM_DICT

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <cassert>
#include <cstdlib>
#include <cstring>

using namespace std;
using namespace jma;

namespace
{
    /** command option for the type (Hiragana or Katakana) to convert to */
    const char* OPTION_TO = "--to";

    /** optional command option for dictionary path */
    const char* OPTION_DICT = "--dict";
}

/**
 * Print the test usage.
 */
void printUsage()
{
    cerr << "Usages:\t" << OPTION_TO << " hira,kata,half,full,lower,upper [--dict DICT_PATH]" << endl;
}

/**
 * Main function.
 */
int main(int argc, char* argv[])
{
    if(argc < 3)
    {
        printUsage();
        exit(1);
    }

    // map from option string to type value
    typedef map<string, Analyzer::OptionType> OptionMapT;
    OptionMapT optionMap;
    optionMap["kata"] = Analyzer::OPTION_TYPE_CONVERT_TO_KATAKANA;
    optionMap["hira"] = Analyzer::OPTION_TYPE_CONVERT_TO_HIRAGANA;
    optionMap["half"] = Analyzer::OPTION_TYPE_CONVERT_TO_HALF_WIDTH;
    optionMap["full"] = Analyzer::OPTION_TYPE_CONVERT_TO_FULL_WIDTH;
    optionMap["lower"] = Analyzer::OPTION_TYPE_CONVERT_TO_LOWER_CASE;
    optionMap["upper"] = Analyzer::OPTION_TYPE_CONVERT_TO_UPPER_CASE;

    // option types
    vector<Analyzer::OptionType> optionVec;
    if(! strcmp(argv[1], OPTION_TO))
    {
        char* p = strtok(argv[2], ",");
        for(; p; p=strtok(0, ","))
        {
            OptionMapT::const_iterator it = optionMap.find(p);
            if(it == optionMap.end())
            {
                cerr << "unknown option type: " << p << endl;
                printUsage();
                exit(1);
            }
            optionVec.push_back(it->second);
        }
    }
    else
    {
        cerr << "unknown command option: " << argv[1] << endl;
        printUsage();
        exit(1);
    }

    // set dictionary files
    const char* sysdict = TEST_JMA_DEFAULT_SYSTEM_DICT;
    if(argc > 3)
    {
        if(! strcmp(argv[3], OPTION_DICT))
        {
            if(argc < 5)
            {
                cerr << "no option value: " << argv[3] << endl;
                printUsage();
                exit(1);
            }
            sysdict = argv[4];
        }
        else
        {
            cerr << "unknown command option: " << argv[3] << endl;
            printUsage();
            exit(1);
        }
    }
    cout << "system dictionary: " << sysdict << endl;

    // create factory
    JMA_Factory* factory = JMA_Factory::instance();

    // ensure only one instance exists
    assert(factory == JMA_Factory::instance());

    // create analyzer and knowledge
    Analyzer* analyzer = factory->createAnalyzer();
    Knowledge* knowledge = factory->createKnowledge();

    // set encoding type from the dictionary path
    string sysdictStr(sysdict);
    size_t first = sysdictStr.find_last_of('_');
    size_t last = sysdictStr.find_last_not_of('/');
    string encodeStr = sysdictStr.substr(first+1, last-first);
    Knowledge::EncodeType encode = Knowledge::decodeEncodeType(encodeStr.c_str());
    if(encode != Knowledge::ENCODE_TYPE_NUM)
    {
        cout << "encoding type: " << encodeStr << endl;
        knowledge->setEncodeType(encode);
    }

    // load dictioanry files
    knowledge->setSystemDict(sysdict);
    int result = knowledge->loadDict();
    if(result == 0)
    {
        cerr << "fail to load dictionary files" << endl;
        exit(1);
    }

    // set knowledge
    analyzer->setKnowledge(knowledge);

    // set convert options
    cout << "convert option values: ";
    for(unsigned int i=0; i<optionVec.size(); ++i)
    {
        cout << optionVec[i] << " ";
        analyzer->setOption(optionVec[i], 1);
    }
    cout << endl << endl;

    string line;
    while(getline(cin, line))
    {
        cout << analyzer->convertCharacters(line.c_str()) << endl << endl;
    }

    delete knowledge;
    delete analyzer;

    return 0;
}
