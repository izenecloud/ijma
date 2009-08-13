/** \file test_jma_pos_nbest.cpp
 * Test JMA in Japanese word segmentation with POS tagging analysis, and output the n-best result.
 * Below is the usage examples:
 * The "DICT_PATH" in below examples is the dictionary path, which is "../db/ipadic/bin_eucjp" defautly.
 * \code
 * To analyze the raw input file "INPUT", and print the n-best result to "OUTPUT".
 * (the example of the parameters below could be:
 * DICT_PATH: ../db/ipadic/bin_eucjp
 * CONIFG_PATH: ../db/config/pos-ipadic.config
 * INPUT: ../db/test/asahi_test_raw_eucjp.txt
 * )
 * $ ./jma_pos_nbest --dict DICT_PATH --config CONFIG_PATH --nbest N INPUT OUTPUT
 *
 * The output format for each sentence is defined below:
 * k (int, the number of candidates for the sentence)
 * prob1 (double, the probability of this candidate)  word1/pos1  word2/pos2  ...
 * prob2  word1/pos1  word2/pos2  ...
 * ...
 * \endcode
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Aug 12, 2009
 */

#include "jma_factory.h"
#include "analyzer.h"
#include "knowledge.h"
#include "sentence.h"

#include <iostream>
#include <fstream>
#include <cassert>
#include <string>
#include <vector>

#include <ctime>

#include <string.h>
#include <stdlib.h>

using namespace std;
using namespace jma;

namespace
{
/** the part-of-speech table mapping from POS index to POS abbreviation form such as NN, NP, V, etc */
vector<string> POSTable;
}

/**
 * load POS table from config file.
 * \param fileName the config file name
 * the format of the config file is like below:
 * index POS
 *
 * example:
 * 0 NN
 * 1 NP
 * 2 V
 * ...
 */
void loadPOSTable(const char* fileName)
{
	assert(fileName);

	ifstream in(fileName);
	if(! in)
	{
		cerr << "[Error] The POS config file " << fileName << " not exists!" << endl;
		exit(1);
	}

    int posIndex = 0;
    string posStr;
    while(in >> posIndex >> posStr)
    {
        if(posIndex >= 0)
        {
            int tableSize = static_cast<int>(POSTable.size()); 
            if(posIndex == tableSize)
            {
                POSTable.push_back(posStr);
            }
            else
            {
                if(posIndex > tableSize)
                {
                    POSTable.resize(posIndex+1);
                }

                POSTable[posIndex] = posStr;
            }
        }
        else
        {
            cerr << "negative POS index is not allowed in " << fileName << endl;
        }
    }
}

/**
 * Analyze a sentence.
 */
void testWithSentence(Analyzer& analyzer, const string& str, ofstream& output)
{
    // ignore the empty line
    if(str.empty())
    {
        return;
    }

    Sentence s(str.c_str());

    int result = analyzer.runWithSentence(s);
    if(result != 1)
    {
        cerr << "fail in Analyzer::runWithSentence()" << endl;
        exit(1);
    }

    // get n-best results
    int count = s.getListSize();
    if(count > 0)
    {
        output << s.getListSize() << endl;
        for(int i=0; i<s.getListSize(); ++i)
        {
            for(int j=0; j<s.getCount(i); ++j)
            {
                if(j == 0)
                {
                    output << s.getScore(i) << analyzer.getWordDelimiter();
                }

                const char* pLexicon = s.getLexicon(i, j);
                // print word with POS complete string
                //output << pLexicon << analyzer.getPOSDelimiter() << s.getStrPOS(i, j) << analyzer.getWordDelimiter();

                // print word with POS abbreviation string
                output << pLexicon << analyzer.getPOSDelimiter() << POSTable[s.getPOS(i, j)] << analyzer.getWordDelimiter();
            }
            output << endl;
        }
    }
    else
    {
        cerr << "no n-best result exists for input string: " << str << endl;
    }
}

/**
 * Analyze a stream.
 */
void testWithStream(Analyzer& analyzer, const char* source, const char* dest)
{
	assert(source);
	assert(dest);

	ifstream in(source);
	if(! in)
	{
		cerr << "[Error] The input file " << source << " not exists!" << endl;
		exit(1);
	}

    ofstream out(dest);
	if(! out)
	{
		cerr << "[Error] The output file " << dest << " could not be created!" << endl;
		exit(1);
	}

    string line;
    while(getline(in, line))
    {
        testWithSentence(analyzer, line, out);
    }
}

/**
 * Print the test usage.
 */
void printUsage()
{
    cerr << "Usages: ./jma_pos_nbest --dict DICT_PATH --config CONFIG_PATH --nbest N INPUT OUTPUT" << endl;
}

/**
 * Main function.
 */
int main(int argc, char* argv[])
{
    if(argc < 9)
    {
        printUsage();
        exit(1);
    }

    // check argument
    if(strcmp(argv[1], "--dict") || strcmp(argv[3], "--config") || strcmp(argv[5], "--nbest"))
    {
        printUsage();
        exit(1);
    }

    const char* sysdict = argv[2];
    const char* posconfig = argv[4];
    int nbest = atoi(argv[6]);
    if(nbest < 1)
    {
        cerr << "invalid number for n-best value" << endl;
        printUsage();
        exit(1);
    }

    // time evaluation for command option "--stream INPUT OUTPUT"
    clock_t stime = clock();

    // load POS config file
    loadPOSTable(posconfig);

    // create instances
    JMA_Factory* factory = JMA_Factory::instance();
    Analyzer* analyzer = factory->createAnalyzer();
    Knowledge* knowledge = factory->createKnowledge();

    // set the number of N-best results,
    // if this function is not called, one-best analysis is performed defaultly on Analyzer::runWithSentence().
    analyzer->setOption(Analyzer::OPTION_TYPE_NBEST, nbest);

    // load dictioanry files
    knowledge->setSystemDict(sysdict);
    int result = knowledge->loadDict();
    if(result == 0)
    {
        cerr << "fail to load dictionary files" << endl;
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
        cout << "set encoding type: " << encodeStr << endl;
        knowledge->setEncodeType(encode);
    }

    // set knowledge
    analyzer->setKnowledge(knowledge);

    // no POS output
    //analyzer->setOption(Analyzer::OPTION_TYPE_POS_TAGGING, 0);

    clock_t etime = clock();
    double dif = (double)(etime - stime) / CLOCKS_PER_SEC;
    cout << "knowledge loading time: " << dif << endl;

    testWithStream(*analyzer, argv[7], argv[8]);

    dif = (double)(clock() - etime) / CLOCKS_PER_SEC;
    cout << "stream analysis time: " << dif << endl;

    dif = (double)(clock() - stime) / CLOCKS_PER_SEC;
    cout << "total time: " << dif << endl;

    // destroy instances
    delete knowledge;
    delete analyzer;

    return 0;
}
