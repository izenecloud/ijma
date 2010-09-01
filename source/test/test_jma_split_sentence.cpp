/** \file test_jma_split_sentence.cpp
 * Test sentence split in JMA analysis.
 * Below is the usage examples to split the sentences from "INPUT" to "OUTPUT" with each sentence in one line:
 * \code
 * When the character encoding of "INPUT" is "EUC-JP".
 * $./jma_split_sentence INPUT OUTPUT --encode eucjp --config ../db/config/sen-eucjp.config
 *
 * When the character encoding of "INPUT" is "SHIFT-JIS".
 * $./jma_split_sentence INPUT OUTPUT --encode sjis --config ../db/config/sen-sjis.config
 *
 * When the character encoding of "INPUT" is "UTF-8".
 * $./jma_split_sentence INPUT OUTPUT --encode utf8 --config ../db/config/sen-utf8.config
 * \endcode
 *
 * \author Jun Jiang
 * \version 0.1
 * \date Jul 27, 2009
 */

#include "ijma.h"
#include "strutil.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
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
    cerr << "Usage: ./jma_split_sentnece INPUT OUTPUT --encode [eucjp,sjis,utf8] --config CONFIG_PATH" << endl;
}

/**
 * Main function.
 */
int main(int argc, char* argv[])
{
    if(argc < 7)
    {
        printUsage();
        exit(1);
    }

    // check command option "--encode"
    if(strcmp(argv[3], "--encode") != 0)
    {
        cerr << "unknown command option " << argv[3] << endl;
        printUsage();
        exit(1);
    }

    Knowledge::EncodeType encode = Knowledge::decodeEncodeType(argv[4]);
    if(encode == Knowledge::ENCODE_TYPE_NUM)
    {
        cerr << "unknown encode type " << argv[4] << endl;
        printUsage();
        exit(1);
    }

    // check command option "--config"
    if(strcmp(argv[5], "--config") != 0)
    {
        cerr << "unknown command option " << argv[5] << endl;
        printUsage();
        exit(1);
    }
    // save config file name
    const char* senConfig = argv[6];

    // create factory
    JMA_Factory* factory = JMA_Factory::instance();

    // create analyzer and knowledge
    Analyzer* analyzer = factory->createAnalyzer();
    Knowledge* knowledge = factory->createKnowledge();

    // set encoding
    //knowledge->setEncodeType(encode);

    //knowledge->setSystemDict(sysdict);
    //int result = knowledge->loadDict();

    // load configure file
    int result = knowledge->loadSentenceSeparatorConfig(senConfig);
    if(result == 0)
    {
        cerr << "fail to load config file: " << senConfig << endl;
        exit(1);
    }

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
            trimSelf(str);
            if(!str.empty())
                to << str << endl;
        }
    }

    // destroy instances
    delete knowledge;
    delete analyzer;

    return 0;
}
