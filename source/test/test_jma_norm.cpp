/** \file test_jma_norm.cpp
 * Test to get normalization forms of Japanese words.
 * Below is the usage examples:
 * \code
 * Test to get normalization form.
 * $ ./test_jma_norm [--dict DICT_PATH]
 * \endcode
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Aug 24, 2010
 */

#include "ijma.h"
#include "test_jma_common.h" // TEST_JMA_DEFAULT_SYSTEM_DICT

#include <string>
#include <iostream>
#include <cassert>
#include <cstdlib>
#include <cstring>

using namespace std;
using namespace jma;

/**
 * Print the test usage.
 */
void printUsage()
{
    cerr << "Usages:\t [--dict DICT_PATH]" << endl;
}

/**
 * Main function.
 */
int main(int argc, char* argv[])
{
    // set dictionary files
    const char* sysdict = TEST_JMA_DEFAULT_SYSTEM_DICT;
    if(argc > 1)
    {
        if(! strcmp(argv[1], "--dict"))
        {
            if(argc < 3)
            {
                cerr << "no option value: " << argv[1] << endl;
                printUsage();
                exit(1);
            }
            sysdict = argv[2];
        }
        else
        {
            cerr << "unknown command option: " << argv[1] << endl;
            printUsage();
            exit(1);
        }
    }

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

    Sentence s;
    string line;
    while(getline(cin, line))
    {
        s.setString(line.c_str());

        int result = analyzer->runWithSentence(s);
        if(result != 1)
        {
            cerr << "fail in Analyzer::runWithSentence()" << endl;
            exit(1);
        }

        // get one-best result
        int i= s.getOneBestIndex();
        if(i == -1)
        {
            cout << "no one-best result exists." << endl;
        }
        else
        {
            for(int j=0; j<s.getCount(i); ++j)
            {
                cout << s.getNormForm(i, j) << "  ";
            }
            cout << endl;
        }
    }

    delete knowledge;
    delete analyzer;

    return 0;
}
