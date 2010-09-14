/** \file test_jma_stopword.cpp
 * Test to filter stop word.
 * Below is the usage examples:
 * \code
 * Specify stop word file by command option "--stop":
 * $ ./test_jma_stopword [--stop STOPWORD_PATH] [--dict DICT_PATH]
 * \endcode
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Sep 08, 2010
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
    cerr << "Usages:\t [--stop STOPWORD_PATH] [--dict DICT_PATH]" << endl;
}

/**
 * Main function.
 */
int main(int argc, char* argv[])
{
    const char* sysdict = TEST_JMA_DEFAULT_SYSTEM_DICT;
    const char* stopdict = TEST_JMA_DEFAULT_STOPWORD_DICT;

    for(int optIndex=1; optIndex+1<argc; optIndex+=2)
    {
        if(! strcmp(argv[optIndex], "--stop"))
            stopdict = argv[optIndex+1];
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
    cout << "stop word dictionary: " << stopdict << endl;

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

    // load stop word dictionary
    if(knowledge->loadStopWordDict(stopdict) == 0)
    {
        cerr << "error: fail to load stop word dictionary" << endl;
        exit(1);
    }

    // set knowledge
    if(analyzer->setKnowledge(knowledge) == 0)
    {
        cerr << "fail to set knowledge" << endl;
        exit(1);
    }

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
        if(i == -1)
            cout << "no one-best result exists." << endl;
        else
        {
            for(int j=0; j<s.getCount(i); ++j)
                cout << s.getLexicon(i, j) << "/" << s.getStrPOS(i, j) << "  ";

            cout << endl;
        }
    }

    delete knowledge;
    delete analyzer;

    return 0;
}
