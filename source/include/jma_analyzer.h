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
#include "pos_table.h"
#include "mecab.h"

#include <string>

/** Base N-Best Score */
#define BASE_NBEST_SCORE 200

using namespace std;

namespace jma
{

class JMA_Knowledge;
class JMA_CType;

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
     * Destructor. Delete the knowledge in this function.
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

private:
	/**
	 * Set the base form of a string, the return value is stored into retVal
	 *
	 * \param origForm the original form of the word
	 * \param feature the feature list, like "動詞,自立,*,*,一段,未然形,見る,ミ,ミ"
	 * \param retVal to store the base form
	 */
	inline void setBaseForm(const string& origForm, const char* feature, string& retVal) const;

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

private:
	/**
	 * hold the JMA_Knowledge Object
	 */
	JMA_Knowledge* knowledge_;

	/**
	 * The Tagger from the Mecab (created by JMA_Knowledge, owned by JMA_Analyzer)
	 */
	MeCab::Tagger* tagger_;

	string strBuf_;

	/**
	 * Previous comma index of the base form offset
	 */
	int preBaseFormOffset_;

    /** POS table for POS string and index code */
    const POSTable* posTable_;
};

} // namespace jma

#endif // JMA_CRF_Analyzer_H
