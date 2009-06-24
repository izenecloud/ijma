/** \file test_jma_factory.cpp
 * Test to create an instance of JMA_Factory.
 * Below is the usage examples:
 * \code
 * Test to create and use JMA_Factory.
 * $./test_jma_factory
 * \endcode
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Jun 22, 2009
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
 * Main function.
 */
int main()
{
    // create factory
    JMA_Factory* factory = JMA_Factory::instance();

    // ensure only one instance exists
    assert(factory == JMA_Factory::instance());

    // create analyzer and knowledge
    Analyzer* analyzer = factory->createAnalyzer();
    Knowledge* knowledge = factory->createKnowledge();

    // set encoding of destination files
    //knowledge->setEncodeType(Knowledge::ENCODE_TYPE_SJIS);

    // set system dictionary directory
#if defined(_WIN32) && !defined(__CYGWIN__)
    knowledge->setSystemDict("../../db/ipadic/bin");
#else
    knowledge->setSystemDict("../db/ipadic/bin");
    knowledge->addUserDict("../db/userdic/eucjp.csv");
#endif

    // load dictioanry files
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
