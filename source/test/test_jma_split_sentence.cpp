/** \file test_jma_split_sentence.cpp
 * Test sentence split in JMA analysis.
 * Below is the usage examples to split the sentences from "INPUT" to "OUTPUT" with each sentence in one line,
 * the "DICT_PATH" is the dictionary path, which is "../db/ipadic/bin_eucjp" defautly.
 * \code
 * The character encoding of "INPUT" should be the same to that of "DICT_PATH"
 * $./jma_split_sentence INPUT OUTPUT [--dict DICT_PATH]
 * \endcode
 *
 * \author Jun Jiang
 * \version 0.1
 * \date Jul 27, 2009
 */

#include "ijma.h"
#include "test_jma_common.h" // TEST_JMA_DEFAULT_SYSTEM_DICT

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cassert>
#include <cstdlib>
#include <cstring>

using namespace std;
using namespace jma;

namespace
{
/**
 * Print the test usage.
 */
void printUsage()
{
    cerr << "Usage: ./jma_split_sentnece INPUT OUTPUT [--dict DICT_PATH]" << endl;
}

/**
 * Remove the begining or trailing space characters.
 * \param str the string to modify
 */
void trimString(string& str)
{
    const char* spaces = " \t\r\n";

    string::size_type pos = str.find_last_not_of(spaces);
    if(pos != string::npos)
        str.erase(pos+1);

    pos = str.find_first_not_of(spaces);
    if(pos != 0)
        str.erase(0, pos);
}
} // namespace
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

    // set default dictionary file
    const char* sysdict = TEST_JMA_DEFAULT_SYSTEM_DICT;
    if(argc > 3)
    {
        if(argc == 5 && ! strcmp(argv[3], "--dict"))
            sysdict = argv[4];
        else
        {
            cerr << "unknown command option " << argv[3] << endl;
            printUsage();
            exit(1);
        }
    }

    // create factory
    JMA_Factory* factory = JMA_Factory::instance();

    // create analyzer and knowledge
    Analyzer* analyzer = factory->createAnalyzer();
    Knowledge* knowledge = factory->createKnowledge();

    // load dictioanry files
    knowledge->setSystemDict(sysdict);
    cout << "system dictionary: " << sysdict << endl;
    if(knowledge->loadDict() == 0)
    {
        cerr << "fail to load dictionary files" << endl;
        exit(1);
    }
    cout << "encoding type of system dictionary: " << Knowledge::encodeStr(knowledge->getEncodeType()) << endl;

    // set knowledge
    analyzer->setKnowledge(knowledge);

    // open files
    const char* source = argv[1];
    const char* dest = argv[2];
    assert(source && dest);

    ifstream from(source);
    if(! from)
    {
        cerr << "error in opening file: " << source << endl;
        exit(1);
    }

    ofstream to(dest);
    if(! to)
    {
        cerr << "error in opening file: " << dest << endl;
        exit(1);
    }

    // split sentences
    string line;
    vector<Sentence> sentVec;
    while(getline(from, line))
    {
        sentVec.clear();
        analyzer->splitSentence(line.c_str(), sentVec);
        for(size_t i=0; i<sentVec.size(); ++i)
        {
            string str(sentVec[i].getString());
            trimString(str);

            if(!str.empty())
                to << str << endl;
        }
    }

    // destroy instances
    delete knowledge;
    delete analyzer;

    return 0;
}
