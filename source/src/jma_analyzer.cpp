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
#include <string>
#include <cstring> // strlen
#include <algorithm> // find

#include "jma_analyzer.h"
#include "tokenizer.h"
#include "char_table.h"

#define JMA_DEBUG_PRINT_COMBINE 0

using namespace std;

namespace
{
/** The scale factor for the limitation count of nbest.
 * As the NBest candidates of \e JMA_Analyzer::runWithSentence() is requred to be unique on segmentation/POS,
 * while \e MeCab::Tagger::nextNode() gives results differing in segementation, POS, base form, reading, etc,
 * so that those results duplicated on segmentation/POS would be removed, and \e MeCab::Tagger::nextNode() would be called continuously until it gives the unique result.
 * To avoid calling it forever, limit the maximum count to be (N * NBEST_LIMIT_SCALE_FACTOR).
 */
const int NBEST_LIMIT_SCALE_FACTOR = 100;

/**
 * In JMA_Analyzer::iterateNode(), used as MorphemeProcessor to append morpheme to list. 
 */
class MorphemeToList
{
public:
    /**
     * Constructor.
     * \param list the morpheme list
     */
    MorphemeToList(jma::MorphemeList& list) :morphList_(list) {}

    /**
     * The process method appends morpheme to list.
     * \param morp the morpheme to append
     */
    void process(const jma::Morpheme& morp) { morphList_.push_back(morp); }

private:
    /** the morpheme list */
    jma::MorphemeList& morphList_;
};

/**
 * In JMA_Analyzer::iterateNode(), used as MorphemeProcessor to output morpheme to string. 
 */
class MorphemeToString
{
public:
    /**
     * Constructor.
     * \param str the string to output
     * \param isPOS whether output POS
     * \param posDelimiter the delimiter between word and POS tag
     * \param wordDelimiter the delimiter between each pair consisting of word and POS tag
     */
    MorphemeToString(std::string& str, bool isPOS, const char* posDelimiter, const char* wordDelimiter)
        :buffer_(str), isPOS_(isPOS) , posDelimiter_(posDelimiter), wordDelimiter_(wordDelimiter) {}

    /**
     * The process method outputs morpheme to string.
     * \param morp the morpheme to output
     */
    void process(const jma::Morpheme& morp)
    {
        buffer_ += morp.lexicon_;
        if(isPOS_) {
            buffer_ += posDelimiter_;
            buffer_ += morp.posStr_;
        }
        buffer_ += wordDelimiter_;
    }

private:
    /** the string buffer to output */
    std::string& buffer_;

    /** whether output POS */
    bool isPOS_;

    /** the delimiter between word and POS tag */
    const char* posDelimiter_;

    /** the delimiter between each pair consisting of word and POS tag */
    const char* wordDelimiter_;
};

/**
 * In JMA_Analyzer::iterateNode(), used as MorphemeProcessor to output morpheme to stream. 
 */
class MorphemeToStream
{
public:
    /**
     * Constructor.
     * \param ost the output stream
     * \param isPOS whether output POS
     * \param posDelimiter the delimiter between word and POS tag
     * \param wordDelimiter the delimiter between each pair consisting of word and POS tag
     */
    MorphemeToStream(std::ostream& ost, bool isPOS, const char* posDelimiter, const char* wordDelimiter)
        :ost_(ost), isPOS_(isPOS) , posDelimiter_(posDelimiter), wordDelimiter_(wordDelimiter) {}

    /**
     * The process method outputs morpheme to stream.
     * \param morp the morpheme to output
     */
    void process(const jma::Morpheme& morp)
    {
        ost_ << morp.lexicon_;
        if(isPOS_) {
            ost_ << posDelimiter_ << morp.posStr_;
        }
        ost_ << wordDelimiter_;
    }

private:
    /** the output stream */
    std::ostream& ost_;

    /** whether output POS */
    bool isPOS_;

    /** the delimiter between word and POS tag */
    const char* posDelimiter_;

    /** the delimiter between each pair consisting of word and POS tag */
    const char* wordDelimiter_;
};
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
    posTable_(0), kanaTable_(0),
    widthTable_(0), caseTable_(0)
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

bool JMA_Analyzer::isCombineCompound() const
{
    return (getOption(OPTION_TYPE_COMPOUND_MORPHOLOGY) != 0);
}

bool JMA_Analyzer::isDecomposeUserNound() const
{
    return (getOption(OPTION_TYPE_DECOMPOSE_USER_NOUN) != 0);
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

int JMA_Analyzer::setKnowledge(Knowledge* pKnowledge)
{
    assert(pKnowledge);

    // release the resources before allocating new ones
    clear();

    knowledge_ = dynamic_cast<JMA_Knowledge*>(pKnowledge);
    assert(knowledge_);

    tagger_ = knowledge_->createTagger();
    if(tagger_ == NULL)
    {
        cerr << "error: fail to create tagger, please insure that Knowledge::loadDict() returns 1 before this function is called." << endl;
        return 0;
    }
    tagger_->set_lattice_level(1);
    if(knowledge_->getCType() == NULL)
    {
        cerr << "error: fail to get character type, please insure that Knowledge::loadDict() returns 1 before this function is called." << endl;
        return 0;
    }

    posTable_ = &knowledge_->getPOSTable();
    kanaTable_ = &knowledge_->getKanaTable();
    widthTable_ = &knowledge_->getWidthTable();
    caseTable_ = &knowledge_->getCaseTable();
    decompMap_ = &knowledge_->getDecompMap();

    return 1;
}

int JMA_Analyzer::runWithSentence(Sentence& sentence)
{
    assert(knowledge_ && knowledge_->getCType() && tagger_);

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
        MorphemeToList processor(list);
        iterateNode(bosNode, processor);

        // ignore empty result
        if(! list.empty())
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
        // j to count steps to avoid iterating forever
        for (int j=0; sentence.getListSize()<N && j<maxCount; ++j)
        {
            const MeCab::Node* bosNode = tagger_->nextNode();
            if( !bosNode )
                break;
            MorphemeList list;
            MorphemeToList processor(list);
            iterateNode(bosNode, processor);

            // ignore empty result
            if(list.empty())
                continue;

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
            if( sentence.getListSize() == 0 )
            {
                if(score > 0 )
                    base = score > BASE_NBEST_SCORE ? score - BASE_NBEST_SCORE : 0;
                else
                    base = score - BASE_NBEST_SCORE;
            }

            double dScore = 1.0 / (score - base );
            totalScore += dScore;
            sentence.addList( list, dScore );
        }

        for ( int j = 0; j < sentence.getListSize(); ++j )
        {
            sentence.setScore(j, sentence.getScore(j) / totalScore );
        }
    }

    return 1;
}

const char* JMA_Analyzer::runWithString(const char* inStr)
{
    assert(knowledge_ && knowledge_->getCType() && tagger_);
    assert(inStr);

    string retStr = knowledge_->getCType()->replaceSpaces(inStr, ' ');
    const MeCab::Node* bosNode = tagger_->parseToNode(retStr.c_str());

    strBuf_.clear();
    MorphemeToString processor(strBuf_, isOutputPOS(), posDelimiter_, wordDelimiter_);
    iterateNode(bosNode, processor);

    return strBuf_.c_str();
}

int JMA_Analyzer::runWithStream(const char* inFileName, const char* outFileName)
{
    assert(knowledge_ && knowledge_->getCType() && tagger_);
    assert(inFileName);
    assert(outFileName);

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
    while (getline(in, line)) {
        string retStr = knowledge_->getCType()->replaceSpaces(line.c_str(), ' ');
        const MeCab::Node* bosNode = tagger_->parseToNode(retStr.c_str());

        MorphemeToStream processor(out, isOutputPOS(), posDelimiter_, wordDelimiter_);
        iterateNode(bosNode, processor);
        out << endl;
    }

    in.close();
    out.close();
    return 1;
}

void JMA_Analyzer::splitSentence(const char* paragraph, std::vector<Sentence>& sentences)
{
    assert(knowledge_ && knowledge_->getCType());
    assert(paragraph);

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

void JMA_Analyzer::getFeatureStr(const char* featureList, int featureOffset, std::string& retVal) const
{
    assert(featureList && featureOffset >= 0);
    
    const char delimit = ',';
    const char* str = featureList;
    const char* stre = featureList + strlen(featureList);
    const char* begin = 0;
    const char* end = 0;

    const int size = featureOffset + 1; // how many ',' to find
    int count = 0; // how many ',' have been found

    while(count < size && str < stre)
    {
        begin = str;
        end = find(str, stre, delimit);
        ++count; // also when 'end' reaches 'stre'

        if(end == stre)
            break;

        str = end + 1;
    }

    if(count == size)
    {
        retVal = string(begin, end - begin);
        if(retVal == "*")
            retVal = "";
    }
    else
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

    getFeatureStr(node->feature, knowledge_->getNormFormOffset(), result.normForm_);
    if(result.normForm_.empty())
        result.normForm_ = result.lexicon_;

    result.posCode_ = (int)node->posid;
    result.posStr_ = posTable_->getPOS(result.posCode_, getPOSFormat());

    return result;
}

MeCab::Node* JMA_Analyzer::combineNode(MeCab::Node* startNode, Morpheme& result) const
{
    assert(startNode && startNode->next && "it is invalid to combine NULL or EOS node");

    result = getMorpheme(startNode);

    // check option
    if(! isCombineCompound())
        return startNode;

    MeCab::Node* node = startNode;
    int startPOS = (int)node->posid;
    while(node->next)
    {
        const RuleNode* ruleNode = posTable_->getCombineRule(startPOS, node->next);
        if(! ruleNode)
            break;

#if JMA_DEBUG_PRINT_COMBINE
        cerr << result.lexicon_ << "/" << result.posStr_;
#endif

        // start from next node
        for(int i=1; i<ruleNode->level_; ++i)
        {
            node = node->next;
            assert(node->next && "the node should not be end-of-sentence node.");

            Morpheme morp = getMorpheme(node);
#if JMA_DEBUG_PRINT_COMBINE
            cerr << "\t+\t" << morp.lexicon_ << "/" << morp.posStr_;
#endif
            result.baseForm_ = result.lexicon_ + morp.baseForm_; // combined base form = previous lexicon + last base form
            result.lexicon_ += morp.lexicon_;
            result.readForm_ += morp.readForm_;
            result.normForm_ += morp.normForm_;
        }

        result.posCode_ = ruleNode->target_;
        result.posStr_ = posTable_->getPOS(result.posCode_, getPOSFormat());
        startPOS = result.posCode_; // to match rules from this combined result

#if JMA_DEBUG_PRINT_COMBINE
        cerr << "\t=>\t" << result.lexicon_ << "/" << result.posStr_ << endl;
#endif
    }

    return node;
}

template<class MorphemeProcessor> void JMA_Analyzer::iterateNode(const MeCab::Node* bosNode, MorphemeProcessor& processor) const
{
    Morpheme morp;
    bool isDecompose = isDecomposeUserNound();
    JMA_Knowledge::DecompMap::const_iterator iter;
    for(MeCab::Node *node = bosNode->next; node->next; node=node->next)
    {
        node = combineNode(node, morp);

        if(isDecompose
                && morp.posCode_ == knowledge_->getUserNounPOSIndex()
                && (iter = decompMap_->find(morp.lexicon_)) != decompMap_->end())
        {
            // decompose into morpheme list
            const MorphemeList& morphList = iter->second;
            for(MorphemeList::const_iterator miter = morphList.begin(); miter!=morphList.end(); ++miter)
            {
                if(isFilter(*miter))
                    continue;

                Morpheme decomp(*miter);
                decomp.baseForm_ = decomp.normForm_ = decomp.lexicon_; // no variant for user noun
                decomp.posCode_ = morp.posCode_; // index of POS user noun
                decomp.posStr_ = morp.posStr_; // string of POS user noun
                processor.process(decomp);
            }
        }
        else
        {
            if(isFilter(morp))
                continue;

            processor.process(morp);
        }
    }
}

std::string JMA_Analyzer::convertCharacters(const char* str) const
{
    assert(knowledge_ && knowledge_->getCType());
    assert(str);

    string result;
    CTypeTokenizer tokenizer(knowledge_->getCType());
    tokenizer.assign(str);

    for(const char* p=tokenizer.next(); p; p=tokenizer.next())
    {
        const char* q;
        if(getOption(OPTION_TYPE_CONVERT_TO_HIRAGANA) != 0)
        {
            if((q = kanaTable_->toLeft(p)) != NULL)
                p = q;
        }
        if(getOption(OPTION_TYPE_CONVERT_TO_KATAKANA) != 0)
        {
            if((q = kanaTable_->toRight(p)) != NULL)
                p = q;
        }
        if(getOption(OPTION_TYPE_CONVERT_TO_HALF_WIDTH) != 0)
        {
            if((q = widthTable_->toLeft(p)) != NULL)
                p = q;
        }
        if(getOption(OPTION_TYPE_CONVERT_TO_FULL_WIDTH) != 0)
        {
            if((q = widthTable_->toRight(p)) != NULL)
                p = q;
        }
        if(getOption(OPTION_TYPE_CONVERT_TO_LOWER_CASE) != 0)
        {
            if((q = caseTable_->toLeft(p)) != NULL)
                p = q;
        }
        if(getOption(OPTION_TYPE_CONVERT_TO_UPPER_CASE) != 0)
        {
            if((q = caseTable_->toRight(p)) != NULL)
                p = q;
        }

        result += p;
    }

    return result;
}

bool JMA_Analyzer::isFilter(const Morpheme& morph) const
{
    if(knowledge_->isStopWord(morph.lexicon_) || ! knowledge_->isKeywordPOS(morph.posCode_))
        return true;

    return false;
}

} // namespace jma
