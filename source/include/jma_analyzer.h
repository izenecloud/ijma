/** \file jma_analyzer.h
 * Definition of class JMA_Analyzer.
 *
 * \author Jun Jiang
 * \version 0.1
 * \date Mar 2, 2009
 */

#ifndef JMA_CRF_ANALYZER_H
#define JMA_CRF_ANALYZER_H

#include "analyzer.h"
#include "sentence.h"
#include "jma_knowledge.h"
#include "pos_table.h"
#include "mecab.h" // MeCab::Node, Tagger

/** Base N-Best Score */
#define BASE_NBEST_SCORE 200

namespace jma
{

class JMA_CType;
class CharTable;

/**
 * JMA_Analyzer executes the Japanese morphological analysis based on conditional random field.
 */
class JMA_Analyzer : public Analyzer
{
public:
    /**
     * Constructor.
     */
    JMA_Analyzer();

    /**
     * Destructor.
     */
    ~JMA_Analyzer();

    /**
     * Set the \e Knowledge for analysis.
     * \param pKnowledge the pointer of \e Knowledge
     */
    virtual void setKnowledge(Knowledge* pKnowledge);

    /**
     * Execute the morphological analysis based on a sentence.
     * \param sentence the instance containing the raw sentence string and also to save the analysis result
     * \return 0 for fail, 1 for success
     * \pre the raw sentence string could be got by \e sentence.getString().
     * \post on successful return, the n-best results could be got by \e sentence.getListSize(), etc.
     * \attention white-space characters are ignored in the analysis, which include " \t\n\v\f\r", and also space character in specific encoding.
     */
    virtual int runWithSentence(Sentence& sentence);

    /**
     * Execute the morphological analysis based on a paragraph string.
     * \param inStr paragraph string
     * \return 0 for fail, otherwise a non-zero string pointer for the one-best result
     */
    virtual const char* runWithString(const char* inStr);

    /**
     * Execute the morphological analysis based on a file.
     * \param inFileName input file name
     * \param outFileName output file name
     * \return 0 for fail, 1 for success
     * \post on successful return, the one-best result is printed to file \e outFileName.
     */
    virtual int runWithStream(const char* inFileName, const char* outFileName);

    /**
     * Split a paragraph string into sentences.
     * \param paragraph paragraph string
     * \param sentences sentence vector
     * \attention the original elements in \e sentences would not be removed, and the splited sentences are appended into \e sentences.
     */
    virtual void splitSentence(const char* paragraph, std::vector<Sentence>& sentences);

    /**
     * The characters, configured by OPTION_TYPE_CONVERT_TO_*, are converted to their target types, other characters are kept as original.
     * \param str string to convert from
     * \return the conversion result
     */
    virtual std::string convertCharacters(const char* str) const;

private:
	/**
	 * Get feature string from list.
	 * \param featureList the feature list, like "動詞,自立,*,*,一段,未然形,見る,ミ,ミ"
     * \param featureOffset the feature offset indexed from zero, each feature is delimited by comma ","
	 * \param retVal to store the feature string
	 */
    void getFeatureStr(const char* featureList, int featureOffset, std::string& retVal) const;

    /**
     * Release the resources owned by \e JMA_Analyzer itself.
     */
    void clear();

    /**
     * Check whether output POS in the format of alphabet.
     * \return true for alphabet format such like "NP-S", false for Japanese format such like "名詞,固有名詞,人名,姓"
     */
    bool isPOSFormatAlphabet() const;

    /**
     * Check whether output POS.
     * \return true for output POS, false for not to output POS.
     */
    bool isOutputPOS() const;

    /**
     * Check whether combine into compound words.
     * \return true to combine, false for not to combine.
     */
    bool isCombineCompound() const;

    /**
     * Check whether decompose user coumpound into nouns.
     * \return true to decompose, false for not to decompose.
     */
    bool isDecomposeUserNound() const;

    /**
     * Get POS output format, such as "名詞,一般", "NC-G", "名詞,一般,*,*", which format type is configured by \e Analyzer::setOption().
     * \return POS output format
     */
    POSTable::POSFormat getPOSFormat() const;

    /**
     * Convert from \e MeCab::Node to \e Morpheme.
     * \param node mecab node to convert from
     * \return morpheme result
     */
    Morpheme getMorpheme(const MeCab::Node* node) const;

    /**
     * Combine MeCab nodes to morpheme using combination rules based on POS.
     * \param[in] startNode the nodes starting from \e startNode are checked whether to combine by \e POSTable::combinePOS()
     * \param[out] result the morpheme as combination result
     * \return the including end node in the combination range
     */
    MeCab::Node* combineNode(MeCab::Node* startNode, Morpheme& result) const;

    /**
     * Iterate MeCab nodes from the node next to \e bosNode, and until the node before and excluding the last node.
     * \param bosNode the node as the begin of sentence
     * \param processor the morpheme processor, in iteration, its method \e process() would be called with parameter \e Moepheme for each node
     */
    template<class MorphemeProcessor> void iterateNode(const MeCab::Node* bosNode, MorphemeProcessor& processor) const;

    /**
     * Check whether to filter out the morpheme.
     * \param morph the morpheme
     * \return true to filter out, false to reserve
     */
    bool isFilter(const Morpheme& morph) const;

private:
	/** hold the JMA_Knowledge Object */
	JMA_Knowledge* knowledge_;

	/** the tagger from Mecab (created by JMA_Knowledge, owned by JMA_Analyzer) */
	MeCab::Tagger* tagger_;

    std::string strBuf_;

    /** POS table for POS string and index code */
    const POSTable* posTable_;

    /** mapping table between Hiragana and Katakana characters */
    const CharTable* kanaTable_;

    /** mapping table between half and full width characters */
    const CharTable* widthTable_;

    /** mapping table between lower and upper case characters */
    const CharTable* caseTable_;

    /** decomposition map to decompose user defined noun */
    const JMA_Knowledge::DecompMap* decompMap_;
};

} // namespace jma

#endif // JMA_CRF_Analyzer_H
