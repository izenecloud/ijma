/** \file test_jma_knowledge.cpp
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
#include "mecab.h"
#include "jma_analyzer.h"

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

    // set system dictionary directory
    knowledge->setSystemDict("../db/ipadic/bin");

    // add user dictionary files
    knowledge->addUserDict("../db/userdic/1.csv");
    //knowledge->addUserDict("../db/userdic/2.csv");

    // load dictioanry files
    int result = knowledge->loadDict();
    if(result == 0)
    {
        cerr << "fail to load dictionary files" << endl;
        exit(1);
    }

    // get tagger
    MeCab::Tagger* tagger = knowledge->getTagger();
    assert(tagger && "MeCab::Tagger is accessible after dictionary is loaded");

    JMA_Analyzer* analyzer = new JMA_Analyzer;
    analyzer->setKnowledge(knowledge);
    analyzer->setOption(Analyzer::OPTION_TYPE_NBEST, 3);
    analyzer->setOption(Analyzer::OPTION_TYPE_POS_TAGGING, 1);

    /*
    string line;
    do{
    	cout<<"Enter ('exit' to exit): ";
    	cin >> line;
    	if(line == "exit")
    		break;
    	cout<<analyzer->runWithString(line.c_str())<<endl;
    }while(true);
    */

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
				cout<< s.getLexicon(i, j) /*<< "/" << s.getStrPOS(i, j)*/ << " ";
			}
			cout<<endl;
		}
		cout<<endl;
	}while(true);


    delete knowledge;
    delete analyzer;
    return 0;
}
