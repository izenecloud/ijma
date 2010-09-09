/** \file test_jma_userdic.cpp
 * Test to load user dictionary.
 * Below is the usage examples:
 * \code
 * Specify user dictionary file by command option "--user",
 * and specify whether to decompose user defined compound into nouns by command option "--decomp",
 * 1 for decompose, 0 for not decompose:
 * $ ./test_jma_userdic [--user USER_PATH] [--decomp [0,1]] [--dict DICT_PATH]
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
    cerr << "Usages:\t [--user USER_PATH] [--decomp [0,1]] [--dict DICT_PATH]" << endl;
}

/**
 * Main function.
 */
int main(int argc, char* argv[])
{
    const char* sysdict = TEST_JMA_DEFAULT_SYSTEM_DICT;
    const char* userdict = TEST_JMA_DEFAULT_USER_DICT;
    int decompOpt = 0;

    for(int optIndex=1; optIndex+1<argc; optIndex+=2)
    {
        if(! strcmp(argv[optIndex], "--user"))
            userdict = argv[optIndex+1];
        else if(! strcmp(argv[optIndex], "--decomp"))
            decompOpt = atoi(argv[optIndex+1]);
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
    cout << "user dictionary: " << userdict << endl;

    // create factory
    JMA_Factory* factory = JMA_Factory::instance();

    // create analyzer and knowledge
    Analyzer* analyzer = factory->createAnalyzer();
    Knowledge* knowledge = factory->createKnowledge();

    // load dictioanry files
    knowledge->setSystemDict(sysdict);
    // get encoding type from user dict file name such as "xxx.eucjp"
    string str = userdict;
    string encodeStr = str.substr(str.find_last_of(".")+1);
    cout << "encoding type of user dictionary: " << encodeStr << endl;
    knowledge->addUserDict(userdict, Knowledge::decodeEncodeType(encodeStr.c_str()));
    if(knowledge->loadDict() == 0)
    {
        cerr << "error: fail to load dictionary files" << endl;
        exit(1);
    }
    cout << "encoding type of system dictionary: " << Knowledge::encodeStr(knowledge->getEncodeType()) << endl;

    // set knowledge
    analyzer->setKnowledge(knowledge);

    // decompose user defined compound into nouns
    analyzer->setOption(Analyzer::OPTION_TYPE_DECOMPOSE_USER_NOUN, decompOpt);

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
                cout << s.getLexicon(i, j) << "(" << s.getBaseForm(i, j) << "," << s.getReadForm(i, j) << "," << s.getNormForm(i, j) << ")/" << s.getStrPOS(i, j) << "  ";

            cout << endl;
        }
    }

    delete knowledge;
    delete analyzer;

    return 0;
}
