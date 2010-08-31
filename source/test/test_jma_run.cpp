/** \file test_jma_run.cpp
 * Test JMA in Japanese word segmentation and POS tagging analysis.
 * Below is the usage examples:
 * The "DICT_PATH" in below examples is the dictionary path, which is "../db/jumandic/bin_eucjp" defautly.
 * \code
 * To analyze a sentence from standard input and print 5 n-best results to standard output.
 * $ ./jma_run --sentence 5 [--dict DICT_PATH]
 * 
 * To analyze a paragraph string from standard input and print the one-best result to standard output.
 * $ ./jma_run --string [--dict DICT_PATH]
 *
 * To exit the loop in the above examples, please press CTRL-C.
 *
 * To analyze the raw input file "INPUT", and print the one-best result to "OUTPUT".
 * (the example of "INPUT" file could be available as "../db/test/asahi_test_raw_eucjp.txt")
 * $ ./jma_run --stream INPUT OUTPUT [--dict DICT_PATH]
 * \endcode
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Jul 2, 2009
 */

#include "ijma.h"
#include "test_jma_common.h" // TEST_JMA_DEFAULT_SYSTEM_DICT

#include <iostream>
#include <fstream>
#include <cassert>
#include <string>

#include <ctime>
#include <cstring>
#include <cstdlib>

using namespace std;
using namespace jma;

namespace
{
    /** command options */
    const char* OPTIONS[] = {"--sentence", "--string", "--stream"};

    /** optional command option for dictionary path */
    const char* OPTION_DICT = "--dict";
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
                cout << "\t" << i << ": ";
                for(int j=0; j<s.getCount(i); ++j)
                {
                    cout << s.getLexicon(i, j) << "(" << s.getBaseForm(i, j) << "," << s.getReadForm(i, j) << "," << s.getNormForm(i, j) << ")/" << s.getStrPOS(i, j) << "  ";
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
            cout << "\t" << i << ": ";
            for(int j=0; j<s.getCount(i); ++j)
            {
                cout << s.getLexicon(i, j) << "(" << s.getBaseForm(i, j) << "," << s.getReadForm(i, j) << "," << s.getNormForm(i, j) << ")/" << s.getStrPOS(i, j) << "  ";
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
        cerr << "fail in runWithStream() from " << source << " to " << dest << endl;
        exit(1);
    }
}

/**
 * Print the test usage.
 */
void printUsage()
{
    cerr << "Usages:\t" << OPTIONS[0] << " N-best [--dict DICT_PATH]" << endl;
    cerr << "  or:\t" << OPTIONS[1] << " [--dict DICT_PATH]" << endl;
    cerr << "  or:\t" << OPTIONS[2] << " INPUT OUTPUT [--dict DICT_PATH]" << endl;
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
    unsigned int optionSize = sizeof(OPTIONS) / sizeof(OPTIONS[0]);
    for(; optionIndex<optionSize; ++optionIndex)
    {
        if(! strcmp(argv[1], OPTIONS[optionIndex]))
            break;
    }

    // check argument
    if((optionIndex == optionSize)
        || (optionIndex == 0 && argc < 3) // command option "--sentence N-best"
        || (optionIndex == 2 && argc < 4)) // command option "--stream INPUT OUTPUT"
    {
        printUsage();
        exit(1);
    }

    // time evaluation for command option "--stream INPUT OUTPUT"
    clock_t stime = 0;
    if(optionIndex == 2)
    {
        stime = clock();
    }
    
    // create instances
    JMA_Factory* factory = JMA_Factory::instance();
    Analyzer* analyzer = factory->createAnalyzer();
    Knowledge* knowledge = factory->createKnowledge();

    // set default dictionary file
    const char* sysdict = TEST_JMA_DEFAULT_SYSTEM_DICT;

    switch(optionIndex)
    {
    case 0:
        // set the number of N-best results,
        // if this function is not called, one-best analysis is performed defaultly on Analyzer::runWithSentence().
        analyzer->setOption(Analyzer::OPTION_TYPE_NBEST, atoi(argv[2]));

        // command option: "--sentence N-best --dict DICT_PATH"
        if(argc == 5 && ! strcmp(argv[3], OPTION_DICT))
        {
            sysdict = argv[4];
        }
        break;

    case 1:
        // command option: "--string --dict DICT_PATH"
        if(argc == 4 && ! strcmp(argv[2], OPTION_DICT))
        {
            sysdict = argv[3];
        }
        break;

    case 2:
        // command option: "--stream INPUT OUTPUT --dict DICT_PATH"
        if(argc == 6 && ! strcmp(argv[4], OPTION_DICT))
        {
            sysdict = argv[5];
        }
        break;

    default:
        printUsage();
        exit(1);
    }

    // set encoding type from the dictionary path
    string sysdictStr(sysdict);
    size_t first = sysdictStr.find_last_of('_');
    size_t last = sysdictStr.find_last_not_of('/');
    string encodeStr = sysdictStr.substr(first+1, last-first);
    Knowledge::EncodeType encode = Knowledge::decodeEncodeType(encodeStr.c_str());
    if(encode != Knowledge::ENCODE_TYPE_NUM)
    {
        cout << "encoding type: " << encodeStr << endl;
        knowledge->setEncodeType(encode);
    }

    // load dictioanry files
    knowledge->setSystemDict(sysdict);

    //const char* userdict = TEST_JMA_DEFAULT_USER_DICT;
    //knowledge->addUserDict(userdict, Knowledge::ENCODE_TYPE_EUCJP);
    //cout << "system dictionary: " << sysdict << endl;
    //cout << "user dictionary: " << userdict << endl;

    int result = knowledge->loadDict();
    if(result == 0)
    {
        cerr << "fail to load dictionary files" << endl;
        exit(1);
    }
    cout << "system dictionary: " << sysdict << endl;

    // set knowledge
    analyzer->setKnowledge(knowledge);

    // no POS output
    //analyzer->setOption(Analyzer::OPTION_TYPE_POS_TAGGING, 0);

    // output POS in alphabet format
    //analyzer->setOption(Analyzer::OPTION_TYPE_POS_FORMAT_ALPHABET, 1);

    // output POS in Japanese format of full category
    //analyzer->setOption(Analyzer::OPTION_TYPE_POS_FULL_CATEGORY, 1);

    // disable compound words combination
    //analyzer->setOption(Analyzer::OPTION_TYPE_COMPOUND_MORPHOLOGY, 0);

    // decompose user defined compound into nouns
    //analyzer->setOption(Analyzer::OPTION_TYPE_DECOMPOSE_USER_NOUN, 1);

    // convert from Katakana to Hiragana
    //analyzer->setOption(Analyzer::OPTION_TYPE_CONVERT_TO_HIRAGANA, 1);

    // convert from Hiragana to Katakana
    //analyzer->setOption(Analyzer::OPTION_TYPE_CONVERT_TO_KATAKANA, 1);

    switch(optionIndex)
    {
    case 0:
        testWithSentence(analyzer);
        break;

    case 1:
        testWithString(analyzer);
        break;

    case 2:
    {
        clock_t etime = clock();
        double dif = (double)(etime - stime) / CLOCKS_PER_SEC;
        cout << "knowledge loading time: " << dif << endl;

        testWithStream(analyzer, argv[2], argv[3]);

        dif = (double)(clock() - etime) / CLOCKS_PER_SEC;
        cout << "stream analysis time: " << dif << endl;

        dif = (double)(clock() - stime) / CLOCKS_PER_SEC;
        cout << "total time: " << dif << endl;
        break;
    }

    default:
        printUsage();
        exit(1);
    }

    // destroy instances
    delete knowledge;
    delete analyzer;

    return 0;
}
