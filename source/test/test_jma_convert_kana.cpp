/** \file test_jma_convert_kana.cpp
 * Test to convert Japanese Hiragana and Katakana characters.
 * Below is the usage examples:
 * \code
 * Test to create and use JMA_Factory.
 * $ ./test_jma_convert_kana --to [hira|kata] [--dict DICT_PATH]
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

#include <string>
#include <iostream>
#include <cassert>
#include <cstdlib>

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
    cerr << "Usages:\t" << OPTION_TO << " [hira|kata] [--dict DICT_PATH]" << endl;
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

    bool isToHiragana = true;
    if(! strcmp(argv[1], OPTION_TO))
    {
        if(! strcmp(argv[2], "kata"))
            isToHiragana = false;
        else if(strcmp(argv[2], "hira"))
        {
            cerr << "unknown option value: " << argv[2] << endl;
            printUsage();
            exit(1);
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

    string line;
    while(getline(cin, line))
    {
        if(isToHiragana)
            cout << "Hiragana:" << endl << analyzer->convertToHiragana(line.c_str()) << endl << endl;
        else
            cout << "Katakana:" << endl << analyzer->convertToKatakana(line.c_str()) << endl << endl;
    }

    delete knowledge;
    delete analyzer;

    return 0;
}
