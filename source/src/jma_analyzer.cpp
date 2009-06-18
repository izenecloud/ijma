/** \file crf_analyzer.cpp
 * Implementation of class CRF_Analyzer.
 *
 * \author Jun Jiang
 * \version 0.1
 * \date Mar 2, 2009
 */


#include <cassert>
#include <iostream>
#include <sstream>
#include <fstream>

#include "jma_analyzer.h"
#include "jma_knowledge.h"
#include "mecab.h"

using namespace std;

namespace jma
{

JMA_Analyzer::JMA_Analyzer()
    : knowledge_(0), tagger_(0)
{
}

JMA_Analyzer::~JMA_Analyzer()
{
	delete knowledge_;
}

void JMA_Analyzer::setKnowledge(Knowledge* pKnowledge)
{
    assert(pKnowledge);

    knowledge_ = dynamic_cast<JMA_Knowledge*>(pKnowledge);
    assert(knowledge_);
    tagger_ = knowledge_->getTagger();
    assert(tagger_);
}

int JMA_Analyzer::runWithSentence(Sentence& sentence)
{
	int N = (int)getOption(Analyzer::OPTION_TYPE_NBEST);
	return 1;
}

const char* JMA_Analyzer::runWithString(const char* inStr)
{
    return 0;
}

int JMA_Analyzer::runWithStream(const char* inFileName, const char* outFileName)
{

    return 1;
}

void JMA_Analyzer::splitSentence(const char* paragraph, std::vector<Sentence>& sentences)
{
    /*if(! paragraph)
        return;

    Sentence result;
    string sentenceStr;
    Tokenizer tokenizer(*ctype_);
    tokenizer.assign(paragraph);
    for(const char* p=tokenizer.next(); p; p=tokenizer.next())
    {
        if(ctype_->isSentenceSeparator(p))
        {
            sentenceStr += p;

            result.setString(sentenceStr.c_str());
            sentences.push_back(result);

            sentenceStr.clear();
        }
        // white-space characters are also used as sentence separator,
        // but they are ignored in the sentence result
        else if(ctype_->isSpace(p))
        {
            if(! sentenceStr.empty())
            {
                result.setString(sentenceStr.c_str());
                sentences.push_back(result);

                sentenceStr.clear();
            }
        }
        else
        {
            sentenceStr += p;
        }
    }

    // in case the last character is not space or sentence separator
    if(! sentenceStr.empty())
    {
        result.setString(sentenceStr.c_str());
        sentences.push_back(result);

        sentenceStr.clear();
    }*/
}

void JMA_Analyzer::analyzerSentence(const char *str,
		vector<const MeCab::Node*>& nodes, vector<double>* scores, int N){
	//one best
	if(N <= 1){
		const MeCab::Node* ret = tagger_->parseToNode(str);
		nodes.push_back(ret);
		//don't update scores vector here
		return;
	}

	assert(scores);

	//N-best
	vector<long> lgScore; //score with long type
	long lgTotalScore = 0; // total of score with long type

	//TODO check return value of parseNBestInit here
	tagger_->parseNBestInit(str);
	int i = 0;
	for ( ; i < N; ++i) {
		const MeCab::Node* ret = tagger_->nextNode();
		if(!ret)
			break;
		nodes.push_back(ret);
		long score = tagger_->nextScore();
		lgTotalScore += score;
		lgScore.push_back(score);
	}

	//update the N-best score
	for( int j=0; j<i; ++j ){
		scores->push_back(lgScore[j] * 1.0 / lgTotalScore);
	}
}

} // namespace jma
