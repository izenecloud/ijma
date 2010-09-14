/** \file test_jma_compound.cpp
 * Test to combine morphemes into compound.
 * Below is the usage examples:
 * \code
 * Specify whether to combine into compounds by command option "--comp",
 * 1 for combine, 0 for not combine:
 * $ ./test_jma_compound [--comp [0,1]] [--dict DICT_PATH]
 * \endcode
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Sep 07, 2010
 */

#include "ijma.h"
#include "test_jma_common.h" // TEST_JMA_DEFAULT_SYSTEM_DICT

#include <vector>
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
    cerr << "Usages:\t [--comp [0,1]] [--dict DICT_PATH]" << endl;
}

/**
 * Main function.
 */
int main(int argc, char* argv[])
{
    const char* sysdict = TEST_JMA_DEFAULT_SYSTEM_DICT;
    int compOpt = 1;

    for(int optIndex=1; optIndex+1<argc; optIndex+=2)
    {
        if(! strcmp(argv[optIndex], "--comp"))
        {
            compOpt = atoi(argv[optIndex+1]);
        }
        else if(! strcmp(argv[optIndex], "--dict"))
            sysdict = argv[optIndex+1];
        else
        {
            cerr << "unknown option: " << argv[optIndex] << endl;
            printUsage();
            exit(1);
        }
    }

    cout << "system dictionary: " << sysdict << endl;
    cout << "combine into compound: " << compOpt << endl;

    // create factory
    JMA_Factory* factory = JMA_Factory::instance();

    // create analyzer and knowledge
    Analyzer* analyzer = factory->createAnalyzer();
    Knowledge* knowledge = factory->createKnowledge();

    // load dictioanry files
    knowledge->setSystemDict(sysdict);
    if(knowledge->loadDict() == 0)
    {
        cerr << "error: fail to load dictionary files" << endl;
        exit(1);
    }
    cout << "encoding type of system dictionary: " << Knowledge::encodeStr(knowledge->getEncodeType()) << endl;

    // set knowledge
    if(analyzer->setKnowledge(knowledge) == 0)
    {
        cerr << "fail to set knowledge" << endl;
        exit(1);
    }

    // set compound words combination
    analyzer->setOption(Analyzer::OPTION_TYPE_COMPOUND_MORPHOLOGY, compOpt);

    Sentence s;
    string line;
    while(getline(cin, line))
    {
        s.setString(line.c_str());

        if(analyzer->runWithSentence(s) != 1)
        {
            cerr << "error: fail in Analyzer::runWithSentence()" << endl;
            exit(1);
        }

        // get one-best result
        int i= s.getOneBestIndex();
        if(i != -1)
        {
            for(int j=0; j<s.getCount(i); ++j)
                cout << s.getLexicon(i, j) << "/" << s.getStrPOS(i, j) << "  ";
        }
        cout << endl;
    }

    delete knowledge;
    delete analyzer;

    return 0;
}
