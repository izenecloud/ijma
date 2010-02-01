/** \file jma_analyzer.cpp
 * Implementation of class JMA_Analyzer.
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
#include "pos_table.h"

#ifndef JMA_DEBUG_PRINT
    #define JMA_DEBUG_PRINT 1
#endif

#define JMA_DEBUG_PRINT_COMBINE 0
#define JMA_DEBUG_PRINT_SEPARATE 0

namespace
{
/** The scale factor for the limitation count of nbest.
 * As the NBest candidates of \e JMA_Analyzer::runWithSentence() is requred to be unique on segmentation/POS,
 * while \e MeCab::Tagger::nextNode() gives results differing in segementation, POS, base form, reading, etc,
 * so that those results duplicated on segmentation/POS would be removed, and \e MeCab::Tagger::nextNode() would be called continuously until it gives the unique result.
 * To avoid calling it forever, limit the maximum count to be (N * NBEST_LIMIT_SCALE_FACTOR).
 */
const int NBEST_LIMIT_SCALE_FACTOR = 100;
}

namespace jma
{

/**
 * Whether the two MorphemeLists is the same
 * \param list1 the MorphemeList 1
 * \param list2 the MorphemeList 2
 * \param printPOS whether also compare POS result
 * \return true for the same, false for not the same
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
    : knowledge_(0), tagger_(0),
    posTable_(0)
{
}

JMA_Analyzer::~JMA_Analyzer()
{
    clear();
}

void JMA_Analyzer::clear()
{
    delete tagger_;
}

bool JMA_Analyzer::isPOSFormatAlphabet() const
{
    return (getOption(OPTION_TYPE_POS_FORMAT_ALPHABET) != 0);
}

bool JMA_Analyzer::isOutputPOS() const
{
    return (getOption(OPTION_TYPE_POS_TAGGING) != 0);
}

bool JMA_Analyzer::isCombineNounAffix() const
{
    return (getOption(OPTION_TYPE_COMBINE_NOUN_AFFIX) != 0);
}

POSTable::POSFormat JMA_Analyzer::getPOSFormat() const
{
    POSTable::POSFormat type = POSTable::POS_FORMAT_DEFAULT;

    if(getOption(Analyzer::OPTION_TYPE_POS_FORMAT_ALPHABET) != 0)
    {
        type = POSTable::POS_FORMAT_ALPHABET;
    }
    else if(getOption(Analyzer::OPTION_TYPE_POS_FULL_CATEGORY) != 0)
    {
        type = POSTable::POS_FORMAT_FULL_CATEGORY;
    }

    return type;
}

void JMA_Analyzer::setKnowledge(Knowledge* pKnowledge)
{
    assert(pKnowledge);

    // release the resources before allocating new ones
    clear();

    knowledge_ = dynamic_cast<JMA_Knowledge*>(pKnowledge);
    assert(knowledge_);

    tagger_ = knowledge_->createTagger();
    // if runWith*() would be called, tagger_ should not be NULL,
    // if only splitSentence() would be called, tagger_ could be NULL as no dictionary needs to be loaded.
    if(tagger_)
    {
        tagger_->set_lattice_level(1);
    }

    posTable_ = knowledge_->getPOSTable();
}

int JMA_Analyzer::runWithSentence(Sentence& sentence)
{
    if(tagger_ == 0)
    {
        cerr << "MeCab::Tagger is not created, please insure that dictionary files are loaded successfully." << endl;
        return 0;
    }

	bool printPOS = isOutputPOS();
	int N = (int)getOption(Analyzer::OPTION_TYPE_NBEST);
    const int maxCount = N * NBEST_LIMIT_SCALE_FACTOR;

	string retStr = knowledge_->getCType()->replaceSpaces(sentence.getString(), ' ');
	const char* strPtr =  retStr.c_str();

	//one best
	if(N <= 1)
	{
		const MeCab::Node* bosNode = tagger_->parseToNode( strPtr );
		MorphemeList list;
        Morpheme morp;
		for (const MeCab::Node *node = bosNode->next; node->next;)
		{
            node = combineNode(node, morp);
			if(knowledge_->isStopWord(morp.lexicon_))
				continue;
			list.push_back(morp);
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
		for (int j=0; i<N && j<maxCount; ++j)
		{
			const MeCab::Node* bosNode = tagger_->nextNode();
			if( !bosNode )
				break;
			MorphemeList list;
            Morpheme morp;
			for (const MeCab::Node *node = bosNode->next; node->next;)
			{
                node = combineNode(node, morp);
                if(knowledge_->isStopWord(morp.lexicon_))
                    continue;
				list.push_back(morp);
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
			{
				if(score > 0 )
					base = score > BASE_NBEST_SCORE ? score - BASE_NBEST_SCORE : 0;
				else
					base = score - BASE_NBEST_SCORE;
			}

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

	bool printPOS = isOutputPOS();

	string retStr = knowledge_->getCType()->replaceSpaces(inStr, ' ');
	const char* strPtr =  retStr.c_str();

	const MeCab::Node* bosNode = tagger_->parseToNode( strPtr );

	strBuf_.clear();
    Morpheme morp;
    for (const MeCab::Node *node = bosNode->next; node->next;){
        node = combineNode(node, morp);
        if(knowledge_->isStopWord(morp.lexicon_))
            continue;

        strBuf_ += morp.lexicon_;
        if(printPOS) {
            strBuf_ += posDelimiter_;
            strBuf_ += morp.posStr_;
        }
        strBuf_ += wordDelimiter_;
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

	bool printPOS = isOutputPOS();

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
    Morpheme morp;
    while (getline(in, line)) {
        string retStr = knowledge_->getCType()->replaceSpaces(line.c_str(), ' ');
        const char* strPtr =  retStr.c_str();

        const MeCab::Node* bosNode = tagger_->parseToNode( strPtr );

        for (const MeCab::Node *node = bosNode->next; node->next;){
            node = combineNode(node, morp);
            if(knowledge_->isStopWord(morp.lexicon_))
                continue;

            out << morp.lexicon_;
            if(printPOS)
                out << posDelimiter_ << morp.posStr_;
            out << wordDelimiter_;
        }

        out << endl;
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

void JMA_Analyzer::getFeatureStr(const char* featureList, int featureOffset, string& retVal) const
{
    assert(featureList && featureOffset >= 0);

	int offset = 0;
	int commaCount = 0;
	int begin = -1;
	int end = -1;
	for( ; featureList[offset]; ++offset )
	{
		if(featureList[offset] == ',')
		{
			++commaCount;
			if(commaCount == featureOffset)
				begin = offset + 1;
			else if(commaCount == featureOffset + 1)
			{
				end = offset;
				break;
			}
		}
	}

	if(begin >= 0)
	{
		if(end < 0)
			end = offset;
		retVal = string(featureList + begin, end - begin);
	}

	if(retVal == "*")
		retVal = "";
}

Morpheme JMA_Analyzer::getMorpheme(const MeCab::Node* node) const
{
    assert(node);

    Morpheme result;

    result.lexicon_.assign(node->surface, node->length);

    getFeatureStr(node->feature, knowledge_->getBaseFormOffset(), result.baseForm_);
    if(result.baseForm_.empty())
        result.baseForm_ = result.lexicon_;

    getFeatureStr(node->feature, knowledge_->getReadFormOffset(), result.readForm_);

    result.posCode_ = (int)node->posid;
    result.posStr_ = posTable_->getPOS(result.posCode_, getPOSFormat());

    return result;
}

MeCab::Node* JMA_Analyzer::combineNode(const MeCab::Node* startNode, Morpheme& result) const
{
    assert(startNode && "it is invalid to combine NULL node");

    MeCab::Node* nextNode = startNode->next;
    assert(nextNode && "it is invalid to combine EOS node.");

    result = getMorpheme(startNode);

    // check option
    if(! isCombineNounAffix())
    {
        return nextNode;
    }

    for(; nextNode->next; nextNode=nextNode->next)
    {
        int combineID = posTable_->combinePOS(result.posCode_, (int)nextNode->posid);
        if(combineID == -1)
        {
#if JMA_DEBUG_PRINT_SEPARATE
            if(!strcmp(posTable_->getPOS(result.posCode_, POSTable::POS_FORMAT_ALPHABET), "P-N")
                    && !strcmp(posTable_->getPOS((int)nextNode->posid, POSTable::POS_FORMAT_ALPHABET), "NP-C"))
                    //if(!strcmp(posTable_->getPOS(result.posCode_, POSTable::POS_FORMAT_ALPHABET), "P-N"))
            //if(!strcmp(posTable_->getPOS((int)nextNode->posid, POSTable::POS_FORMAT_ALPHABET), "NS-D"))
            {
                Morpheme nextMorph = getMorpheme(nextNode);
#if JMA_DEBUG_PRINT_SEPARATE
                cerr << result.lexicon_ << "/" << result.posStr_ << "/" << posTable_->getPOS(result.posCode_, POSTable::POS_FORMAT_ALPHABET);
                cerr << "\t\t" << nextMorph.lexicon_ << "/" << nextMorph.posStr_ << "/" << posTable_->getPOS(nextMorph.posCode_, POSTable::POS_FORMAT_ALPHABET);
                cerr << endl;
#endif
            }
#endif
            break;
        }

        Morpheme nextMorph = getMorpheme(nextNode);
#if JMA_DEBUG_PRINT_COMBINE
        cerr << result.lexicon_ << "/" << result.posStr_;
        cerr << "\t+\t" << nextMorph.lexicon_ << "/" << nextMorph.posStr_;
#endif
        result.lexicon_ += nextMorph.lexicon_;
        result.baseForm_ += nextMorph.baseForm_;
        result.readForm_ += nextMorph.readForm_;
        result.posCode_ = combineID;
        result.posStr_ = posTable_->getPOS(result.posCode_, getPOSFormat());
#if JMA_DEBUG_PRINT_COMBINE
        cerr << "\t=>\t" << result.lexicon_ << "/" << result.posStr_ << endl;
#endif
    }

    return nextNode;
}

} // namespace jma
