/** \file test_jma_encoding.cpp
 * Test character encoding "EUC-JP", "SHIFT-JIS" and "UTF-8" in JMA analysis.
 * Below is the usage examples:
 * \code
 * Test character encoding "EUC-JP".
 * $./test_jma_encoding --encode eucjp
 *
 * Test character encoding "SHIFT-JIS".
 * $./test_jma_encoding --encode sjis
 *
 * Test character encoding "UTF-8".
 * $./test_jma_encoding --encode utf8
 * \endcode
 *
 * \author Jun Jiang
 * \version 0.1
 * \date Jul 17, 2009
 */

#include "ijma.h"
#include "test_jma_common.h" // TEST_JMA_DEFAULT_SYSTEM_DICT, TEST_JMA_DEFAULT_USER_DICT

#include <iostream>
#include <string>
#include <cassert>
#include <cstdlib>
#include <string.h>

using namespace std;
using namespace jma;

/**
 * Print the test usage.
 */
void printUsage()
{
    cerr << "Usage: ./test_jma_encoding --encode [eucjp,sjis,utf8]" << endl;
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

    // check command option
    if(strcmp(argv[1], "--encode") != 0)
    {
        cerr << "unknown command option " << argv[1] << endl;
        printUsage();
        exit(1);
    }

    Knowledge::EncodeType encode = Knowledge::decodeEncodeType(argv[2]);
    if(encode == Knowledge::ENCODE_TYPE_NUM)
    {
        cerr << "unknown encode type " << argv[2] << endl;
        printUsage();
        exit(1);
    }

    // create factory
    JMA_Factory* factory = JMA_Factory::instance();

    // create analyzer and knowledge
    Analyzer* analyzer = factory->createAnalyzer();
    Knowledge* knowledge = factory->createKnowledge();

    // set dictionary files
    string sysdict = TEST_JMA_DEFAULT_SYSTEM_DICT;
    string userdict = TEST_JMA_DEFAULT_USER_DICT;
    if(encode == Knowledge::ENCODE_TYPE_EUCJP)
    {
        sysdict += "/../bin_eucjp";
    }
    else if(encode == Knowledge::ENCODE_TYPE_SJIS)
    {
        sysdict += "/../bin_sjis";
    }
    else if(encode == Knowledge::ENCODE_TYPE_UTF8)
    {
        sysdict += "/../bin_utf8";
    }

    // load dictioanry files
    cout << "system dictionary: " << sysdict << endl;
    cout << "user dictionary: " << userdict << endl;
    knowledge->setSystemDict(sysdict.c_str());
    knowledge->addUserDict(userdict.c_str());
    if(knowledge->loadDict() == 0)
    {
        cerr << "fail to load dictionary files" << endl;
        exit(1);
    }
    cout << "encoding type of system dictionary: " << Knowledge::encodeStr(knowledge->getEncodeType()) << endl;

    // set knowledge
    analyzer->setKnowledge(knowledge);

    // analyze string
    const char* str = "abc";
    cout << "input: " << str << endl;
    cout << "output: " << analyzer->runWithString(str) << endl;

    delete knowledge;
    delete analyzer;

    return 0;
}
