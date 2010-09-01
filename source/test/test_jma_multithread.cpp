/** \file test_jma_multithread.cpp
 * Test JMA in multithread environment.
 * \author Vernkin
 * \version 0.1
 * \date Sep 1, 2009
 */

#include "ijma.h"
#include "test_jma_common.h" // TEST_JMA_DEFAULT_SYSTEM_DICT
#include "thread.h"

#include <iostream>
#include <sstream>
#include <cassert>
#include <string>
#include <vector>

#include <ctime>

#include <cstring>
#include <cstdlib>

using namespace std;
using namespace jma;

class AnalyzerThread : public MeCab::thread {
public:
    unsigned int id_;
    string input_;
    string output_;
    Analyzer* analyzer_;

    AnalyzerThread() : id_(-1), analyzer_(0) { }

    ~AnalyzerThread() {
        delete analyzer_;
    }

    void run() {
        ostringstream ss;
        ss << id_;
        string source = input_;
        string dest = output_ +  ".output-" + ss.str();

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
    cerr << "Usages:\t input_file output_file [--num THREAD_NUM] [--dict DICT_PATH]" << endl;
    cerr << "the output of each thread would be output_file.output-*." << endl;
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
    const char* sysdict = TEST_JMA_DEFAULT_SYSTEM_DICT;
    unsigned int threadNum = 50;
    int optIndex = 3;
    if(argc > optIndex)
    {
        if(strcmp(argv[optIndex], "--num") || argc < optIndex + 2)
        {
            printUsage();
            exit(1);
        }
        threadNum = atoi(argv[optIndex+1]);
    }

    optIndex = 5;
    if(argc > optIndex)
    {
        if(strcmp(argv[optIndex], "--dict") || argc < optIndex + 2)
        {
            printUsage();
            exit(1);
        }
        sysdict = argv[optIndex+1];
    }

    // create instances
    JMA_Factory* factory = JMA_Factory::instance();

    // create instances
    Knowledge* knowledge = factory->createKnowledge();
    knowledge->setSystemDict(sysdict);
    if(knowledge->loadDict() == 0)
    {
        cerr << "fail to load dictionary files "<< sysdict << endl;
        exit(1);
    }
    cout << "encoding type of system dictionary: " << Knowledge::encodeStr(knowledge->getEncodeType()) << endl;

    // create multi threads
    vector<AnalyzerThread> threadVec(threadNum);
    for(unsigned int i=0; i<threadNum; ++i)
    {
        threadVec[i].id_ = i;
        threadVec[i].input_ = input;
        threadVec[i].output_ = output;
        threadVec[i].analyzer_ = factory->createAnalyzer();
        threadVec[i].analyzer_->setKnowledge(knowledge);
    }

    // run
    for(unsigned int i=0; i<threadNum; ++i)
    {
        threadVec[i].start();
    }

    // wait for end
    for(unsigned int i=0; i<threadNum; ++i)
    {
        threadVec[i].join();
    }

    // destroy instance
    delete knowledge;

    return 0;
}
