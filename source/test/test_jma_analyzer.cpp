/** \file test_jma_analyzer.cpp
 * Test to create an instance of JMA_Analyzer.
 * Below is the usage examples:
 * \code
 * Test to create and use Knowledge.
 * $./test_jma_analyzer
 * \endcode
 *
 * \author Vernin
 * \version 0.1
 * \date Jun 23, 2009
 */
#include "jma_knowledge.h"
#include "jma_analyzer.h"
#include "test_jma_common.h" // TEST_JMA_DEFAULT_SYSTEM_DICT, TEST_JMA_DEFAULT_USER_DICT_CSV, TEST_JMA_DEFAULT_USER_DICT_TXT

#include <iostream>
#include <cassert>
#include <stdlib.h>

using namespace std;
using namespace jma;

/**
 * Main function.
 */
int main()
{
    // create knowledge
    JMA_Knowledge* knowledge = new JMA_Knowledge;

    // set dictionary files
    const char* sysdict = TEST_JMA_DEFAULT_SYSTEM_DICT;
    const char* userdict = TEST_JMA_DEFAULT_USER_DICT_CSV;
    const char* userdict1 = TEST_JMA_DEFAULT_USER_DICT_TXT;
    cout << "system dictionary: " << sysdict << endl;
    cout << "user dictionary 0: " << userdict << endl;
    cout << "user dictionary 1: " << userdict1 << endl;

    // load dictioanry files
    knowledge->setSystemDict(sysdict);
    knowledge->addUserDict(userdict);
    knowledge->addUserDict(userdict1);
    int result = knowledge->loadDict();
    if(result == 0)
    {
        cerr << "fail to load dictionary files" << endl;
        exit(1);
    }

    JMA_Analyzer* analyzer = new JMA_Analyzer;
    analyzer->setKnowledge(knowledge);
    analyzer->setOption(Analyzer::OPTION_TYPE_NBEST, 5);
    analyzer->setOption(Analyzer::OPTION_TYPE_POS_TAGGING, 1);

//#define TEST_CMD_STR

#ifdef TEST_CMD_STR

    string line;
    do{
    	cout<<"Enter ('exit' to exit): ";
    	cin >> line;
    	if(line == "exit")
    		break;
    	cout<<analyzer->runWithString(line.c_str())<<endl;
    }while(true);

#else

    Sentence s;
    string line;
	do{
		cout<<"Enter ('exit' to exit): ";
		getline(cin, line);
		if(line == "exit")
			break;
		s.setString(line.c_str());
		analyzer->runWithSentence(s);
		for(int i=0; i<s.getListSize(); ++i)
		{
			cout<<"#"<<(i+1)<<" "<<s.getScore(i)<<" : ";
			for(int j=0; j<s.getCount(i); ++j)
			{
				cout<< s.getLexicon(i, j) << "/" << s.getStrPOS(i, j) << "@" << s.getBaseForm(i, j) << " ";
			}
			cout<<endl;
		}
		cout<<endl;
	}while(true);
#endif

    delete knowledge;
    delete analyzer;
    return 0;
}
