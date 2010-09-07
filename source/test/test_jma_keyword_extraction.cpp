/** \file test_jma_keyword_extraction.cpp
 * Test to extract keywords by POS tags.
 * Below is the usage examples:
 * \code
 * Test to extract keywords, specify the keywords POS tags by command option "--pos".
 * $ ./test_jma_keyword_extraction [--pos NC-G,V-I] [--dict DICT_PATH]
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
    cerr << "Usages:\t [--pos POS_TAG,...] [--dict DICT_PATH]" << endl;
}

/**
 * Main function.
 */
int main(int argc, char* argv[])
{
    const char* sysdict = TEST_JMA_DEFAULT_SYSTEM_DICT;
    vector<string> posVec;

    for(int optIndex=1; optIndex+1<argc; optIndex+=2)
    {
        if(! strcmp(argv[optIndex], "--pos"))
        {
            char* p = strtok(argv[optIndex+1], ",");
            for(; p; p=strtok(0, ","))
                posVec.push_back(p);
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
    cout << "keyword POS: ";
    for(vector<string>::const_iterator it=posVec.begin(); it!=posVec.end(); ++it)
        cout << *it << " ";
    cout << endl;

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

    int num = knowledge->setKeywordPOS(posVec);
    if(num != static_cast<int>(posVec.size()))
    {
        cerr << "error: only " << num << " POS are set as keyword POS, while " << posVec.size() << " is expected." << endl;
        exit(1);
    }

    // set knowledge
    analyzer->setKnowledge(knowledge);

    // output POS in alphabet format
    analyzer->setOption(Analyzer::OPTION_TYPE_POS_FORMAT_ALPHABET, 1);

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

            cout << endl << endl;
        }
    }

    delete knowledge;
    delete analyzer;

    return 0;
}
