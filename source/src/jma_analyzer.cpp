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

/**
 * Whether the two MorphemeLists is the same
 * \param list1 the MorphemeList 1
 * \param list2 the MorphemeList 2
 */
inline bool isSameMorphemeList( const MorphemeList* list1, const MorphemeList* list2, bool printPOS )
{
	// if one is zero pointer, return null
	if( !list1 || !list2 )
	{
		return false;
	}

	if( list1->size() != list2->size() )
		return false;
	//compare one by one
	size_t N = list1->size();
	if( printPOS )
	{
		for( size_t i = 0; i < N; ++i )
		{
			const Morpheme& m1 = (*list1)[i];
			const Morpheme& m2 = (*list2)[i];
			if( m1.lexicon_ != m2.lexicon_ || m1.posCode_ != m2.posCode_
					|| m1.posStr_ != m2.posStr_ )
			{
				return false;
			}
		}
	}
	else
	{
		for( size_t i = 0; i < N; ++i )
		{
			if( (*list1)[i].lexicon_ != (*list2)[i].lexicon_ )
				return false;
		}
	}
	//all the elements are the same
	return true;
}

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
    // if runWith*() would be called, tagger_ should not be NULL,
    // if only splitSentence() would be called, tagger_ could be NULL as no dictionary needs to be loaded.
    if(tagger_)
    {
        tagger_->set_lattice_level(1);
    }

    maxPosCateOffset_ = knowledge_->getPOSCatNum() - 1;
}

int JMA_Analyzer::runWithSentence(Sentence& sentence)
{
    if(tagger_ == 0)
    {
        cerr << "MeCab::Tagger is not created, please insure that dictionary files are loaded successfully." << endl;
        return 0;
    }

	bool printPOS = getOption(OPTION_TYPE_POS_TAGGING) > 0;
	int N = (int)getOption(Analyzer::OPTION_TYPE_NBEST);

	string retStr = knowledge_->getCType()->replaceSpaces(sentence.getString(), ' ');
	const char* strPtr =  retStr.data();

	//one best
	if(N <= 1)
	{
		const MeCab::Node* bosNode = tagger_->parseToNode( strPtr );
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
	// N-best
	else
	{
		double totalScore = 0;

		if( !tagger_->parseNBestInit( strPtr ) )
		{
			cerr << "[Error] Cannot parseNBestInit on the " << strPtr << endl;
			return 0;
		}

		long base = 0;
		int i = 0;
		for ( ; i < N;  )
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

			bool isDupl = false;
			//check the current result with exits results
			for( int listOffset = sentence.getListSize() - 1 ; listOffset >= 0; --listOffset )
			{
				if( isSameMorphemeList( sentence.getMorphemeList(listOffset), &list, printPOS ) )
				{
					isDupl = true;
					break;
				}
			}
			//ignore the duplicate results
			if( isDupl )
				continue;

			long score = tagger_->nextScore();
			if( i == 0 )
				base = score > BASE_NBEST_SCORE ? score - BASE_NBEST_SCORE : 0;

			double dScore = 1.0 / (score - base );
			totalScore += dScore;
			sentence.addList( list, dScore );
			++i;
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
    if(tagger_ == 0)
    {
        cerr << "MeCab::Tagger is not created, please insure that dictionary files are loaded successfully." << endl;
        return 0;
    }

	bool printPOS = getOption(OPTION_TYPE_POS_TAGGING) > 0;

	string retStr = knowledge_->getCType()->replaceSpaces(inStr, ' ');
	const char* strPtr =  retStr.data();

	const MeCab::Node* bosNode = tagger_->parseToNode( strPtr );

	strBuf_.clear();
	if (printPOS) {
		for (const MeCab::Node *node = bosNode->next; node->next; node = node->next){
			string seg(node->surface, node->length);
			if(knowledge_->isStopWord(seg))
				continue;

			strBuf_.append(node->surface, node->length).append(posDelimiter_).
				append(node->feature, getPOSOffset(node->feature) ).append(wordDelimiter_);
		}

	} else {
		for (const MeCab::Node *node = bosNode->next; node->next; node = node->next){
			string seg(node->surface, node->length);
			if(knowledge_->isStopWord(seg))
				continue;

			strBuf_.append(node->surface, node->length).append(wordDelimiter_);
		}
	}

	return strBuf_.c_str();
}

int JMA_Analyzer::runWithStream(const char* inFileName, const char* outFileName)
{
	assert(inFileName);
	assert(outFileName);

    if(tagger_ == 0)
    {
        cerr << "MeCab::Tagger is not created, please insure that dictionary files are loaded successfully." << endl;
        return 0;
    }

	bool printPOS = getOption(OPTION_TYPE_POS_TAGGING) > 0;

	ifstream in(inFileName);
	if(!in)
	{
		cerr<<"[Error] The input file "<<inFileName<<" not exists!"<<endl;
		return 0;
	}

    ofstream out(outFileName);
	if(!out)
	{
		cerr<<"[Error] The output file "<<outFileName<<" could not be created!"<<endl;
		return 0;
	}

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

        string retStr = knowledge_->getCType()->replaceSpaces(line.c_str(), ' ');
        const char* strPtr =  retStr.data();

        const MeCab::Node* bosNode = tagger_->parseToNode( strPtr );

        if (printPOS) {
			for (const MeCab::Node *node = bosNode->next; node->next; node = node->next){
				string seg(node->surface, node->length);
				if(knowledge_->isStopWord(seg))
					continue;

				out.write(node->surface, node->length) << posDelimiter_;
				out.write(node->feature, getPOSOffset(node->feature) ) << wordDelimiter_;
			}

            if (remains)
                out << endl;
            else
                break;

        } else {
			for (const MeCab::Node *node = bosNode->next; node->next; node = node->next){
				string seg(node->surface, node->length);
				if(knowledge_->isStopWord(seg))
					continue;

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
