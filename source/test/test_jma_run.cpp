/** \file test_jma_run.cpp
 * Test JMA in Japanese word segmentation and POS tagging analysis.
 * Below is the usage examples:
 * \code
 * To analyze a sentence from standard input and print 5 n-best results to standard output.
 * $ ./jma_run --sentence 5
 * 
 * To analyze a paragraph string from standard input and print the one-best result to standard output.
 * $ ./jma_run --string
 *
 * To exit the loop in the above examples, please press CTRL-C.
 *
 * To analyze the raw input file "SOURCE", and print the one-best result to "DEST".
 * (the example of "SOURCE" file could be available as "../db/test/asahi_test_raw_eucjp.txt")
 * $ ./jma_run --stream SOURCE DEST
 *
 * To split paragraphs in file "SOURCE", and print each sentence in one line to "DEST".
 * $ ./jma_run --split SOURCE DEST
 * \endcode
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Jul 2, 2009
 */

#include "jma_factory.h"
#include "analyzer.h"
#include "knowledge.h"
#include "sentence.h"

#include <iostream>
#include <fstream>
#include <cassert>

#include <ctime>

#include <string.h>
#include <stdlib.h>

using namespace std;
using namespace jma;

namespace
{
    /** command options */
    const char* options[] = {"--sentence", "--string", "--stream", "--split"};
}

/**
 * Analyze a sentence.
 */
void testWithSentence(Analyzer* analyzer)
{
    cout << "########## test method runWithSentence()" << endl;
    
    Sentence s;
    string line;
    cout << "please input sentence ended with newline:" << endl;
    while(getline(cin, line))
    {
        s.setString(line.c_str());

        int result = analyzer->runWithSentence(s);
        if(result != 1)
        {
            cerr << "fail in Analyzer::runWithSentence()" << endl;
            exit(1);
        }

        // get n-best results
        if(s.getListSize() == 0)
        {
            cout << "no n-best result exists." << endl;
        }
        else
        {
            cout << "n-best result:" << endl;
            for(int i=0; i<s.getListSize(); ++i)
            {
                for(int j=0; j<s.getCount(i); ++j)
                {
                    if(j == 0)
                        cout << "\t" << i << ": ";

                    const char* pLexicon = s.getLexicon(i, j);
                    cout << pLexicon << "/" << s.getStrPOS(i, j) << "  ";
                }
                cout << "\t" << s.getScore(i) << "\t#words: " << s.getCount(i) << endl;
            }
            cout << endl;
        }

        // get one-best result
        int i= s.getOneBestIndex();
        if(i == -1)
        {
            cout << "no one-best result exists." << endl;
        }
        else
        {
            cout << "one-best result:" << endl;
            for(int j=0; j<s.getCount(i); ++j)
            {
                if(j == 0)
                    cout << "\t" << i << ": ";

                const char* pLexicon = s.getLexicon(i, j);
                cout << pLexicon << "/" << s.getStrPOS(i, j) << "  ";
            }
            cout << "\t" << s.getScore(i) << endl;
        }

        cout << endl << "please input sentence ended with newline:" << endl;
    }
}

/**
 * Analyze a string.
 */
void testWithString(Analyzer* analyzer)
{
    cout << "########## test method runWithString()" << endl;
    cout << "please input string ended with newline:" << endl;

    string line;
    while(getline(cin, line))
    {
        cout << endl << "result:" << endl << analyzer->runWithString(line.c_str()) << endl;
        cout << endl << "please input string ended with newline:" << endl;
    }
}

/**
 * Analyze a stream.
 */
void testWithStream(Analyzer* analyzer, const char* source, const char* dest)
{
    cout << "########## test method runWithStream()" << endl;

    int result = analyzer->runWithStream(source, dest);
    if(result == 1)
    {
        cout << "succeed in runWithStream() from " << source << " to " << dest << endl;
    }
    else
    {
        cout << "fail in runWithStream() from " << source << " to " << dest << endl;
    }
}

/**
 * Split paragraphs in file \e source into sentences, and print each sentence in one line to \e dest.
 */
void testSplitSentence(Analyzer* analyzer, const char* source, const char* dest)
{
    cout << "########## test method splitSentence()" << endl;

    assert(source && dest);

    ifstream from(source);
    if(! from)
    {
        cerr << "error in opening file: " << source << endl;
        return;
    }

    ofstream to(dest);
    if(! to)
    {
        cerr << "error in opening file: " << dest << endl;
        return;
    }

    string line;
    vector<Sentence> sentVec;
    while(getline(from, line))
    {
        sentVec.clear();
        analyzer->splitSentence(line.c_str(), sentVec);
        for(size_t i=0; i<sentVec.size(); ++i)
        {
            to << sentVec[i].getString() << endl;
        }
    }
}

/**
 * Print the test usage.
 */
void printUsage()
{
    cerr << "Usages:\t" << options[0] << " [N-best]" << endl;
    cerr << "  or:\t" << options[1] << endl;
    cerr << "  or:\t" << options[2] << " SOURCE DEST" << endl;
    cerr << "  or:\t" << options[3] << " SOURCE DEST" << endl;
}

/**
 * Main function.
 */
int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        printUsage();
        exit(1);
    }

    unsigned int optionIndex = 0;
    unsigned int optionSize = sizeof(options) / sizeof(options[0]);
    for(; optionIndex<optionSize; ++optionIndex)
    {
        if(! strcmp(argv[1], options[optionIndex]))
            break;
    }

    // check argument
    if((optionIndex == optionSize)
	    || (optionIndex == 2 && argc != 4) // command option "--stream SOURCE DEST"
	    || (optionIndex == 3 && argc != 4)) // command option "--split SOURCE DEST"
    {
        printUsage();
        exit(1);
    }

    // time evaluation for command option "--stream SOURCE DEST"
    clock_t stime = 0;
    if(optionIndex == 2)
    {
        stime = clock();
    }
    
    // create instances
    JMA_Factory* factory = JMA_Factory::instance();
    Analyzer* analyzer = factory->createAnalyzer();
    Knowledge* knowledge = factory->createKnowledge();

    // set dictionary files
    const char* sysdict;
    const char* userdict;
#if defined(_WIN32) && !defined(__CYGWIN__)
    sysdict = "../../db/ipadic/bin_eucjp";
    userdict = "../../db/userdic/eucjp.csv";
#else
    sysdict = "../db/ipadic/bin_eucjp";
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

    // load sentence separator config file
    const char* senConfig;
#if defined(_WIN32) && !defined(__CYGWIN__)
    senConfig = "../../db/config/sen-eucjp.config";
#else
    senConfig = "../db/config/sen-eucjp.config";
#endif
    result = knowledge->loadSentenceSeparatorConfig(senConfig);
    if(result == 0)
    {
        cerr << "fail to load config file: " << senConfig << endl;
        exit(1);
    }

    // set knowledge
    analyzer->setKnowledge(knowledge);

    if(optionIndex == 0)
    {
        if(argc > 2)
        {
            // (optional) set the number of N-best results,
            // if this function is not called, one-best analysis is performed defaultly on Analyzer::runWithSentence().
            analyzer->setOption(Analyzer::OPTION_TYPE_NBEST, atoi(argv[2]));
        }
        testWithSentence(analyzer);
    }
    else if(optionIndex == 1)
    {
        testWithString(analyzer);
    }
    else if(optionIndex == 2)
    {
        clock_t etime = clock();
        double dif = (double)(etime - stime) / CLOCKS_PER_SEC;
        cout << "knowledge loading time: " << dif << endl;

        testWithStream(analyzer, argv[2], argv[3]);

        dif = (double)(clock() - etime) / CLOCKS_PER_SEC;
        cout << "stream analysis time: " << dif << endl;

        dif = (double)(clock() - stime) / CLOCKS_PER_SEC;
        cout << "total time: " << dif << endl;
    }
    else if(optionIndex == 3)
    {
        testSplitSentence(analyzer, argv[2], argv[3]);
    }
    else
    {
        printUsage();
        exit(1);
    }

    // destroy instances
    delete knowledge;
    delete analyzer;

    return 0;
}
