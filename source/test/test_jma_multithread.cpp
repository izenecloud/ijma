/** \file test_jma_multithread.cpp
 * Test JMA in multithread environment.
 * Below is the usage examples:
 * The "DICT_PATH" in below examples is the dictionary path, which is "../db/ipadic/bin_eucjp" defautly.
 * \code
 * Each thread prints its own output.
 * $ ./jma_multithread INPUT_FILE OUTPUT_DIR [--dict DICT_PATH] [--num THREAD_NUM]
 * \endcode
 * 
 * \author Jun
 * \version 0.1
 * \date Sep 1, 2010
 */

#include "ijma.h"
#include "test_jma_common.h" // TEST_JMA_DEFAULT_SYSTEM_DICT
#include "thread.h"
#include "file_utils.h" // createFilePath, getFileName, getDirPath

#include <iostream>
#include <sstream>
#include <cassert>
#include <string>
#include <vector>

#include <cstring>
#include <cstdlib>

/** multihreads share single knowledge */
#define TEST_CASE_SINGLE_KNOWLEDGE 0

/** each thread creates its own knowledge, and each knowledge shares single system dictionary */
#define TEST_CASE_MULTI_KNOWLEDGE 1

/** each thread creates its own knowledge, and the knowledges shares multiple system dictionaries */
#define TEST_CASE_MULTI_SYS_DICT 2

/** current test case */
#define TEST_CASE_NUM TEST_CASE_SINGLE_KNOWLEDGE

/** whether test user dictionary */
#define TEST_USER_DICT 0

/** whether pause for user input before destroy resources */
#define PAUSE_BEFORE_DESTROY 0

#if TEST_CASE_NUM == TEST_CASE_MULTI_SYS_DICT
/** file names of input file and system dictionary */
const int SYS_DICT_NUM = 3;
const char* INPUT_FILE_NAMES[SYS_DICT_NUM] = {"asahi_test_raw_eucjp.txt", "asahi_test_raw_sjis.txt", "asahi_test_raw_utf8.txt"};
const char* SYS_DICT_NAMES[SYS_DICT_NUM] = {"bin_eucjp", "bin_sjis", "bin_utf8"};
#endif

using namespace std;
using namespace jma;

class AnalyzerThread : public MeCab::thread {
public:
    unsigned int id_;
    string input_;
    string output_;
    Analyzer* analyzer_;
    Knowledge* knowledge_;

    AnalyzerThread() : id_(-1), analyzer_(0), knowledge_(0) { }

    ~AnalyzerThread() {
        delete analyzer_;
#if TEST_CASE_NUM != TEST_CASE_SINGLE_KNOWLEDGE
        delete knowledge_;
#endif
    }

    void run() {
        ostringstream ss;
        ss << id_;
        string source = input_;
        string dest = createFilePath(output_.c_str(), (getFileName(input_) +  ".thread-" + ss.str()).c_str());

        cerr << "########## Thread No." << id_ << " => test method runWithStream()" << endl;
        cerr << "##### input file: " << source << endl;
        cerr << "##### output file: " << dest << endl;


        int result = analyzer_->runWithStream(source.c_str(), dest.c_str());
        cerr << "Thread No." << id_ << "=> " << (result == 1 ? "succeeded" : "failed") << endl;
    }
};

/**
 * Print the test usage.
 */
void printUsage()
{
    cerr << "Usages:\t input_file output_dir [--dict DICT_PATH] [--num THREAD_NUM]" << endl;
    cerr << "the output of each thread would be output_dir/input_file.thread-*." << endl;
}

/**
 * Main function.
 */
int main(int argc, char* argv[])
{
    cerr << "Japanese Morphological Analyzer Multi-Thread Test"<< endl;

    if(argc < 3)
    {
        printUsage();
        exit(1);
    }

    // file name of input and output
    string input = argv[1];
    string output = argv[2];

    // default config
    string dictPath = TEST_JMA_DEFAULT_SYSTEM_DICT;
    unsigned int threadNum = 50;
    for(int optIndex=3; optIndex+1<argc; optIndex+=2)
    {
        if(! strcmp(argv[optIndex], "--num"))
            threadNum = atoi(argv[optIndex+1]);
        else if(! strcmp(argv[optIndex], "--dict"))
            dictPath = argv[optIndex+1];
        else
        {
            cerr << "unknown option: " << argv[optIndex] << endl;
            printUsage();
            exit(1);
        }
    }

    // create instances
    JMA_Factory* factory = JMA_Factory::instance();

    // create instances
#if TEST_CASE_NUM == TEST_CASE_SINGLE_KNOWLEDGE
    Knowledge* knowledge = factory->createKnowledge();
    knowledge->setSystemDict(dictPath.c_str());
#if TEST_USER_DICT
    const char* userdict = TEST_JMA_DEFAULT_USER_DICT;
    knowledge->addUserDict(userdict, Knowledge::ENCODE_TYPE_EUCJP);
    cout << "user dictionary: " << userdict << endl;
#endif
    if(knowledge->loadDict() == 0)
    {
        cerr << "fail to load dictionary files "<< dictPath << endl;
        exit(1);
    }
    cout << "system dictionary: "<< dictPath << endl;
    cout << "encoding type of system dictionary: " << Knowledge::encodeStr(knowledge->getEncodeType()) << endl;
#endif

    // create multi threads
    vector<AnalyzerThread> threadVec(threadNum);
    for(unsigned int i=0; i<threadNum; ++i)
    {
        threadVec[i].id_ = i;
        threadVec[i].analyzer_ = factory->createAnalyzer();
        threadVec[i].input_ = input;
        threadVec[i].output_ = output;

#if TEST_CASE_NUM != TEST_CASE_SINGLE_KNOWLEDGE
        string newDict = dictPath;
#endif

#if TEST_CASE_NUM == TEST_CASE_MULTI_SYS_DICT
        threadVec[i].input_ = createFilePath(getDirPath(input).c_str(), INPUT_FILE_NAMES[i % SYS_DICT_NUM]);
        newDict += "/../";
        newDict += SYS_DICT_NAMES[i % SYS_DICT_NUM];
#endif

#if TEST_CASE_NUM != TEST_CASE_SINGLE_KNOWLEDGE
        Knowledge* knowledge = factory->createKnowledge();
        knowledge->setSystemDict(newDict.c_str());
#if TEST_USER_DICT
        const char* userdict = TEST_JMA_DEFAULT_USER_DICT;
        knowledge->addUserDict(userdict, Knowledge::ENCODE_TYPE_EUCJP);
        cout << "user dictionary: " << userdict << endl;
#endif
        if(knowledge->loadDict() == 0)
        {
            cerr << "fail to load dictionary files "<< newDict << endl;
            exit(1);
        }
        cout << "system dictionary: "<< newDict << endl;
        cout << "encoding type of system dictionary: " << Knowledge::encodeStr(knowledge->getEncodeType()) << endl;
#endif

        threadVec[i].knowledge_ = knowledge;
        threadVec[i].analyzer_->setKnowledge(knowledge);
    }

    // run
    for(unsigned int i=0; i<threadNum; ++i)
        threadVec[i].start();

    // wait for end
    for(unsigned int i=0; i<threadNum; ++i)
        threadVec[i].join();

#if PAUSE_BEFORE_DESTROY
    char ch;
#endif

#if TEST_CASE_NUM == TEST_CASE_SINGLE_KNOWLEDGE
#if PAUSE_BEFORE_DESTROY
    cout << "before destroy single knowledge..." << endl;
    cin >> ch;
#endif
    delete knowledge;
#endif

#if PAUSE_BEFORE_DESTROY
    cout << "before destroy all analyzers..." << endl;
    cin >> ch;
    threadVec.clear();

    cout << "wait for input to quit test case " << TEST_CASE_NUM << "..." << endl;
    cin >> ch;
#endif
    return 0;
}
