/** \file test_jma_encoding.cpp
 * Test character encoding "EUC-JP" and "SHIFT-JIS" in JMA analysis.
 * Below is the usage examples:
 * \code
 * Test character encoding "EUC-JP".
 * $./test_jma_encoding --encode eucjp
 *
 * Test character encoding "SHIFT-JIS".
 * $./test_jma_encoding --encode sjis
 * \endcode
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Jun 24, 2009
 */

#include "jma_factory.h"
#include "analyzer.h"
#include "knowledge.h"

#include <iostream>
#include <cassert>
#include <cstdlib>

using namespace std;
using namespace jma;

/**
 * Print the test usage.
 */
void printUsage()
{
    cerr << "Usage: ./test_jma_encoding --encode [eucjp,sjis]" << endl;
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

    Knowledge::EncodeType encode;

    // check command option
    if(strcmp(argv[1], "--encode") != 0)
    {
        cerr << "unkown command option " << argv[1] << endl;
        printUsage();
        exit(1);
    }

    if(strcmp(argv[2], "eucjp") == 0)
    {
        encode = Knowledge::ENCODE_TYPE_EUCJP;
    }
    else if(strcmp(argv[2], "sjis") == 0)
    {
        encode = Knowledge::ENCODE_TYPE_SJIS;
    }
    else
    {
        cerr << "unkown encode type " << argv[2] << endl;
        printUsage();
        exit(1);
    }

    // create factory
    JMA_Factory* factory = JMA_Factory::instance();

    // create analyzer and knowledge
    Analyzer* analyzer = factory->createAnalyzer();
    Knowledge* knowledge = factory->createKnowledge();

    // set encoding
    knowledge->setEncodeType(encode);

    // set dictionary files
    const char* sysdict;
    const char* userdict;
#if defined(_WIN32) && !defined(__CYGWIN__)
    if(encode == Knowledge::ENCODE_TYPE_EUCJP)
    {
        sysdict = "../../db/ipadic/bin_eucjp";
    }
    else if(encode == Knowledge::ENCODE_TYPE_SJIS)
    {
        sysdict = "../../db/ipadic/bin_sjis";
    }

    userdict = "../../db/userdic/eucjp.csv";
#else
    if(encode == Knowledge::ENCODE_TYPE_EUCJP)
    {
        sysdict = "../db/ipadic/bin_eucjp";
    }
    else if(encode == Knowledge::ENCODE_TYPE_SJIS)
    {
        sysdict = "../db/ipadic/bin_sjis";
    }

    userdict = "../db/userdic/eucjp.csv";
#endif

    // load dictioanry files
    knowledge->setSystemDict(sysdict);
    knowledge->addUserDict(userdict);
    int result = knowledge->loadDict();
    if(result == 0)
    {
        cerr << "fail to load dictionary files" << endl;
        exit(1);
    }

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
