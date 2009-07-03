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
#include "tokenizer.h"


namespace jma
{

JMA_Analyzer::JMA_Analyzer()
    : knowledge_(0), tagger_(0)
{
}

JMA_Analyzer::~JMA_Analyzer()
{
}

void JMA_Analyzer::setKnowledge(Knowledge* pKnowledge)
{
    assert(pKnowledge);

    knowledge_ = dynamic_cast<JMA_Knowledge*>(pKnowledge);
    assert(knowledge_);
    tagger_ = knowledge_->getTagger();
    assert(tagger_);
    tagger_->set_lattice_level(1);
    maxPosCateOffset_ = knowledge_->getPOSCatNum() - 1;
}

int JMA_Analyzer::runWithSentence(Sentence& sentence)
{
	bool printPOS = getOption(OPTION_TYPE_POS_TAGGING) > 0;
	int N = (int)getOption(Analyzer::OPTION_TYPE_NBEST);

	//one best
	if(N <= 1)
	{
		const MeCab::Node* bosNode = tagger_->parseToNode( sentence.getString() );
		MorphemeList list;
		for (const MeCab::Node *node = bosNode->next; node->next; node = node->next)
		{
			string seg(node->surface, node->length);
			if(knowledge_->isStopWord(seg))
				continue;
			list.push_back(Morpheme());
			Morpheme& morp = list.back();
			morp.lexicon_ = seg;
			if(printPOS)
			{
				morp.posCode_ = (int)node->posid;
				morp.posStr_ = string(node->feature, getPOSOffset(node->feature));
			}
		}
		sentence.addList(list, 1.0);
	}
	else
	{
		double totalScore = 0;


		if( !tagger_->parseNBestInit( sentence.getString() ) )
		{
			cerr << "[Error] Cannot parseNBestInit on the " << sentence.getString() << endl;
			return 0;
		}

		long base = 0;
		int i = 0;
		for ( ; i < N; ++i )
		{
			const MeCab::Node* bosNode = tagger_->nextNode();
			if( !bosNode )
				break;
			MorphemeList list;
			for (const MeCab::Node *node = bosNode->next; node->next; node = node->next)
			{
				string seg(node->surface, node->length);
				if(knowledge_->isStopWord(seg))
					continue;
				list.push_back(Morpheme());
				Morpheme& morp = list.back();
				morp.lexicon_ = seg;
				if(printPOS)
				{
					morp.posCode_ = (int)node->posid;
					morp.posStr_ = string(node->feature, getPOSOffset(node->feature));
				}
			}
			long score = tagger_->nextScore();
			if( i == 0 )
				base = score > BASE_NBEST_SCORE ? score - BASE_NBEST_SCORE : 0;

			double dScore = 1.0 / (score - base );
			totalScore += dScore;
			sentence.addList( list, dScore );
		}

		for ( int j = 0; j < i; ++j )
		{
			sentence.setScore(j, sentence.getScore(j) / totalScore );
		}
	}

	return 1;
}

const char* JMA_Analyzer::runWithString(const char* inStr)
{
	bool printPOS = getOption(OPTION_TYPE_POS_TAGGING) > 0;

	const MeCab::Node* bosNode = tagger_->parseToNode( inStr );

	strBuf_.clear();
	if (printPOS) {
		for (const MeCab::Node *node = bosNode->next; node->next; node = node->next){
			strBuf_.append(node->surface, node->length).append(posDelimiter_).
				append(node->feature, getPOSOffset(node->feature) ).append(wordDelimiter_);
		}

	} else {
		for (const MeCab::Node *node = bosNode->next; node->next; node = node->next){
			strBuf_.append(node->surface, node->length).append(wordDelimiter_);
		}
	}

	return strBuf_.c_str();

}

int JMA_Analyzer::runWithStream(const char* inFileName, const char* outFileName)
{
	assert(inFileName);
	assert(outFileName);

	bool printPOS = getOption(OPTION_TYPE_POS_TAGGING) > 0;

	ifstream in(inFileName);
	if(!in)
	{
		cerr<<"[Error] The input file "<<inFileName<<" not exists!"<<endl;
		return 0;
	}
    ofstream out(outFileName);
    string line;
    bool remains = !in.eof();
    while (remains) {
        getline(in, line);
        remains = !in.eof();
        if (!line.length()) {
            if( remains )
				out << endl;
            continue;
        }

        const MeCab::Node* bosNode = tagger_->parseToNode( line.c_str() );

        if (printPOS) {
			for (const MeCab::Node *node = bosNode->next; node->next; node = node->next){
				out.write(node->surface, node->length) << posDelimiter_;
				out.write(node->feature, getPOSOffset(node->feature) ) << wordDelimiter_;
			}

            if (remains)
                out << endl;
            else
                break;

        } else {
			for (const MeCab::Node *node = bosNode->next; node->next; node = node->next){
				out.write(node->surface, node->length) << wordDelimiter_;
			}

			if (remains)
				out << endl;
			else
				break;
        }
    }

    in.close();
    out.close();
    return 1;
}

void JMA_Analyzer::splitSentence(const char* paragraph, std::vector<Sentence>& sentences)
{
    if(! paragraph)
        return;

    Sentence result;
    string sentenceStr;
    CTypeTokenizer tokenizer( knowledge_->getCType() );
    tokenizer.assign(paragraph);
    for(const char* p=tokenizer.next(); p; p=tokenizer.next())
    {
        if( knowledge_->isSentenceSeparator(p) )
        {
            sentenceStr += p;

            result.setString(sentenceStr.c_str());
            sentences.push_back(result);

            sentenceStr.clear();
        }
        /*// white-space characters are also used as sentence separator,
        // but they are ignored in the sentence result
        else if(ctype_->isSpace(p))
        {
            if(! sentenceStr.empty())
            {
                result.setString(sentenceStr.c_str());
                sentences.push_back(result);

                sentenceStr.clear();
            }
        }*/
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
    }
}


int JMA_Analyzer::getPOSOffset(const char* feature){
	if( knowledge_->isOutputFullPOS() )
	{
		// count for the index of the fourth offset
		int offset = 0;
		int commaCount = 0;
		for( ; feature[offset]; ++offset )
		{
			if(feature[offset] == ',')
			{
				++commaCount;
				if( commaCount > maxPosCateOffset_ )
					break;
			}
		}
		return offset;
	}

	 //count for the index of the comma that after non-star pos sections
	int offset = 0;
	int commaCount = 0;
	for( ; feature[offset]; ++offset )
	{
		if(feature[offset] == ',')
		{
			++commaCount;
			if( commaCount > maxPosCateOffset_ )
				break;
		}
		else if(feature[offset] == '*')
		{
			//if offset is 0, contains nothing
			return offset ? offset - 1 : 0 ;
		}
	}

	return offset;
}

} // namespace jma
