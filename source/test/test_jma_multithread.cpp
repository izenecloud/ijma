/** \file t_cmac_run2.cpp
 * \author HyunWoo Lee
 * \version 0.1
 * \date Sep 1, 2009
 */

#include "jma_factory.h"
#include "analyzer.h"
#include "knowledge.h"
#include "sentence.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <cassert>
#include <string>

#include <ctime>

#include <string.h>
#include <stdlib.h>

#include <pthread.h>

using namespace std;
using namespace jma;

//#define USE_SAME_ANALYZER
#define USE_SAME_KNOWLEDGE


const unsigned int NUM_THREADS = 10;
int done[NUM_THREADS];

struct pthread_t_data{
	unsigned int thread_id;
	string input_filename;
	string output_filename;
	Analyzer* analyzer;
};


void* analyzerWithThread(void* threadarg)	{
	pthread_t_data* data;
	data = (struct pthread_t_data*)threadarg;
	Analyzer* _analyzer = data->analyzer;
	done[data->thread_id] = 0;
	
	ostringstream ss;
	ss << data->thread_id;
	string source = data->input_filename;
	string dest = data->output_filename +  ".output-" + ss.str();
	
	cerr << "########## Thread No." << data->thread_id << " => test method runWithStream()" << endl;
	cerr << "##### input file: " << source << endl;
	cerr << "##### output file: " << dest << endl;


	int result = _analyzer->runWithStream(source.c_str(), dest.c_str());
	if(result == 1)
	{
		cerr << "Thread No." << data->thread_id << "=> succeed in runWithStream() from " << source << " to " << dest << endl;
	}
	else
	{
		cerr << "Thread No." << data->thread_id << "=> fail in runWithStream() from " << source << " to " << dest << endl;
	}
	
	done[data->thread_id] = 1;

	pthread_exit(NULL);
}

/**
 * Print the test usage.
 */
void printUsage()
{
    cerr << "Usages:\t raw_file [result_file]" << endl;
    cerr << "if result_file is not given, it would be raw_file.output-* defaultly." << endl;
}

/**
 * Main function.
 */
int main(int argc, char* argv[])
{
	cerr << "Japanese Morphological Analyzer Multi-Thread Test, " << __DATE__ << " " << __TIME__ << endl;
	
	if(argc < 2)
	{
		printUsage();
		exit(1);
	}

	// create instances
	JMA_Factory* factory = JMA_Factory::instance();

    // set default dictionary file
    const char* sysdict = 0;
#if defined(_WIN32) && !defined(__CYGWIN__)
    sysdict = "../../db/ipadic/bin_eucjp";
#else
    sysdict = "../db/ipadic/bin_eucjp";
#endif

    // set encoding type from the dictionary path
	string sysdictStr(sysdict);
	size_t first = sysdictStr.find_last_of('_');
	size_t last = sysdictStr.find_last_not_of('/');
	string encodeStr = sysdictStr.substr(first+1, last-first);
	Knowledge::EncodeType encode = Knowledge::decodeEncodeType(encodeStr.c_str());
	
#ifdef USE_SAME_KNOWLEDGE
	// create instances
	Knowledge* knowledge = factory->createKnowledge();
	knowledge->setSystemDict(sysdict);
	if(knowledge->loadDict() == 0)
	{
		cerr << "fail to load dictionary files "<< sysdict << endl;
		exit(1);
	}

	if(encode != Knowledge::ENCODE_TYPE_NUM)
		knowledge->setEncodeType(encode);
#endif

#ifdef USE_SAME_ANALYZER
	Analyzer *analyzer = factory->createAnalyzer();
	analyzer->setOption(Analyzer::OPTION_TYPE_POS_TAGGING, 0);
	analyzer->setKnowledge(knowledge);
#endif

	// multi-thread test
	int rc;
	unsigned int tid = 0;
	
	pthread_t threads[NUM_THREADS];
	pthread_t_data thread_data[NUM_THREADS];
	
    // file name of input and output
    string input = argv[1];
    string output;
    if(argc < 3)
    {
        output = input;
    }
    else
    {
        output = argv[2];
    }

	while(tid < NUM_THREADS)	{
		thread_data[tid].thread_id = tid;
		thread_data[tid].input_filename = input;
		thread_data[tid].output_filename = output;

	#ifndef USE_SAME_KNOWLEDGE
		Knowledge* knowledge = factory->createKnowledge();
		knowledge->setSystemDict(sysdict);
		if(knowledge->loadDict() == 0)
		{
			cerr << "fail to load dictionary files "<< sysdict << endl;
			exit(1);
		}

		if(encode != Knowledge::ENCODE_TYPE_NUM)
			knowledge->setEncodeType(encode);
	#endif

	#ifdef USE_SAME_ANALYZER
		thread_data[tid].analyzer = analyzer;
	#else
		thread_data[tid].analyzer = factory->createAnalyzer();
		thread_data[tid].analyzer->setOption(Analyzer::OPTION_TYPE_POS_TAGGING, 0);
		thread_data[tid].analyzer->setKnowledge(knowledge);
	#endif

		rc = pthread_create(&threads[tid], NULL, analyzerWithThread, (void *) &thread_data[tid]);
		if (rc){
			printf("ERROR; return code from pthread_create() is %d\n", rc);
			exit(-1);
		}

		++tid;
	}
	
	while(true)	{
		unsigned int _done = 0;
		for(unsigned int i = 0; i < NUM_THREADS; i++)	{
			_done += done[i];
		}
		
		//cout<<"Progress: "<<_done<<"/"<<NUM_THREADS<<endl;

		if(_done == NUM_THREADS)	{
			break;
		}
	}
	
	// destroy instances
#ifdef USE_SAME_KNOWLEDGE
	delete knowledge;
#endif

#ifdef USE_SAME_ANALYZER
	delete analyzer;
#endif

	pthread_exit(NULL);
}