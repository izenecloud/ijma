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

#include "ijma.h"
#include "test_jma_common.h" // TEST_JMA_DEFAULT_SYSTEM_DICT

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

    // set dictionary files
    const char* sysdict = TEST_JMA_DEFAULT_SYSTEM_DICT;
    cout << "system dictionary: " << sysdict << endl;

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

    // analyze string
    const char* str = "abc";
    cout << "input: " << str << endl;
    cout << "output: " << analyzer->runWithString(str) << endl;

    delete knowledge;
    delete analyzer;

    return 0;
}
