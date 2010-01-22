/** \file analyzer.h
 * Definition of class Analyzer.
 *
 * \author Jun Jiang
 * \version 0.1
 * \date Jun 12, 2009
 */

#ifndef JMA_ANALYZER_H
#define JMA_ANALYZER_H

#include <vector>

/**
 * namespace of library JMA (Japanese Morphological Analyzer)
 */
namespace jma
{

class Knowledge;
class Sentence;

/**
 * Analyzer executes the Japanese morphological analysis.
 * Typically, the usage is like below:
 *
 * \code
 * // create instances
 * JMA_Factory* factory = JMA_Factory::instance();
 * Analyzer* analyzer = factory->createAnalyzer();
 * Knowledge* knowledge = factory->createKnowledge();
 *
 * // load dictionaries (optional)
 * knowledge->setSystemDict("...");
 * knowledge->addUserDict("...");
 * knowledge->loadDict();
 * knowledge->loadStopWordDict("...");
 *
 * // set knowledge
 * analyzer->setKnowledge(knowledge);
 *
 * // analyze a sentence
 * Sentence s;
 * s.setString("...");
 * analyzer->runWithSentence(s);
 * ...
 *
 * // analyze a paragraph
 * const char* result = analyzer->runWithString("...");
 * ...
 *
 * // analyze a file
 * analyzer->runWithStream("...", "...");
 *
 * // split paragraphs into sentences
 * string line;
 * vector<Sentence> sentVec;
 * while(getline(cin, line)) // get paragraph string from standard input
 * {
 *      sentVec.clear(); // remove previous sentences
 *      analyzer->splitSentence(line.c_str(), sentVec);
 *      for(size_t i=0; i<sentVec.size(); ++i)
 *      {
 *          analyzer->runWithSentence(sentVec[i]); // analyze each sentence
 *          ...
 *      }
 *  }
 *
 * delete knowledge;
 * delete analyzer;
 * \endcode
 */
class Analyzer
{
public:
    /**
     * Constructor.
     */
    Analyzer();

    /**
     * Destructor.
     */
    virtual ~Analyzer();

    /**
     * Set the \e Knowledge for analysis.
     * \param pKnowledge the pointer of \e Knowledge
     */
    virtual void setKnowledge(Knowledge* pKnowledge) = 0;

    /**
     * Execute the morphological analysis based on a sentence.
     * \param sentence the instance containing the raw sentence string and also to save the analysis result
     * \return 0 for fail, 1 for success
     * \pre the raw sentence string could be got by \e sentence.getString().
     * \post on successful return, the n-best results could be got by \e sentence.getListSize(), etc.
     */
    virtual int runWithSentence(Sentence& sentence) = 0;

    /**
     * Execute the morphological analysis based on a paragraph string.
     * \param inStr paragraph string
     * \return 0 for fail, otherwise a non-zero string pointer for the one-best result
     */
    virtual const char* runWithString(const char* inStr) = 0;

    /**
     * Execute the morphological analysis based on a file.
     * \param inFileName input file name
     * \param outFileName output file name
     * \return 0 for fail, 1 for success
     * \post on successful return, the one-best result is printed to file \e outFileName.
     */
    virtual int runWithStream(const char* inFileName, const char* outFileName) = 0;

    /**
     * Split a paragraph string into sentences.
     * \param paragraph paragraph string
     * \param sentences sentence vector
     * \attention the original elements in \e sentences would not be removed, and the splited sentences are appended into \e sentences.
     */
    virtual void splitSentence(const char* paragraph, std::vector<Sentence>& sentences) = 0;

    /**
     * Option type for analysis.
     */
    enum OptionType
    {
        /** Configure whether to print part-of-speech tags.
         * If a non-zero value is configured, part-of-speech tags are printed in below APIs:
         * \e runWithString(), \e runWithStream(),
         * output example: "通常/名詞,一般  は/助詞,係助詞  ".
         *
         * If a zero value is configured, part-of-speech tags are not printed in above APIS,
         * output example: "通常  は  ".
         *
         * Default value: 1
         */
        OPTION_TYPE_POS_TAGGING,

        /** Configure the number of n-best results.
         * If a positive value is configured, the maximum number of n-best results are got in below APIs:
         * \e runWithSentence().
         *
         * If non-positive value is configured, it is ignored as invalid value.
         *
         * If the value of option \e OPTION_TYPE_POS_TAGGING is zero,
         * the n-best results are different on \e Morpheme::lexicon_,
         * otherwise, they are different on either \e Morpheme::lexicon_ or \e Morpheme::posStr_.
         *
         * Default value: 1
         */
        OPTION_TYPE_NBEST,

        /** Configure whether to print part-of-speech tags in alphabet format.
         * If a non-zero value is configured, part-of-speech tags are in alphabet format in below APIs:
         * \e runWithSentence(), \e runWithString(), \e runWithStream(),
         * output example: "通常/NC-G  は/PL-A  ".
         *
         * If a zero value is configured, part-of-speech tags are in Japanese format in above APIS,
         * output example: "通常/名詞,一般  は/助詞,係助詞  ".
         *
         * Default value: 0
         */
        OPTION_TYPE_POS_FORMAT_ALPHABET,

        /** Configure whether to print part-of-speech tags in Japanese format of full category.
         * If a non-zero value is configured, part-of-speech tags are in Japanese format of full category,
         * which might includes asterisk symbol, it is valid for below APIs:
         * \e runWithSentence(), \e runWithString(), \e runWithStream(),
         * output example: "通常/名詞,一般,*,*  は/助詞,係助詞,*,*  ".
         *
         * If a zero value is configured, part-of-speech tags are in effective category format,
         * which excludes asterisk symbol,
         * output example: "通常/名詞,一般  は/助詞,係助詞  ".
         *
         * If the value of option \e OPTION_TYPE_POS_FORMAT_ALPHABET is non-zero,
         * this option would take no effect.
         *
         * Default value: 0
         */
        OPTION_TYPE_POS_FULL_CATEGORY,

        /** Configure whether to combine affix into noun.
         * If a non-zero value is configured, affix (including prefix and suffix) is combined with adjacent noun,
         * it is valid for below APIs:
         * \e runWithSentence(), \e runWithString(), \e runWithStream(),
         * output example: "長野県/名詞,固有名詞,地域,一般  ".
         *
         * If a zero value is configured, affix is not combined in above APIS,
         * output example: "長野/名詞,固有名詞,地域,一般  県/名詞,接尾,地域  ".
         *
         * If the configuration file "pos-combine.def" does not exist in system dictionary path,
         * no affix would be combined.
         *
         * Default value: 1
         */
        OPTION_TYPE_COMBINE_NOUN_AFFIX,

        OPTION_TYPE_NUM ///< the count of option types
    };

    /**
     * Set the option value for analysis.
     * \param nOption the option type
     * \param nValue the option value
     * \attention when \e nOption is \e OPTION_TYPE_NBEST, the invalid \e nValue less than 1 will take no effect.
     */
    void setOption(OptionType nOption, double nValue);

    /**
     * Get the option value.
     * \param nOption the option type
     * \return the option value
     */
    double getOption(OptionType nOption) const;

    /**
     * Set the delimiter between word and POS tag in the output result of \e runWithString() and \e runWithStream(), which delimiter is "/" defaultly so that the result would be "word/pos  word/pos  ...".
     * \param delimiter the delimiter between word and POS tag in the output result
     */
    void setPOSDelimiter(const char* delimiter);

    /**
     * Get the delimiter between word and POS tag in the output result of \e runWithString() and \e runWithStream(), which delimiter is "/" defaultly so that the result would be "word/pos  word/pos  ...".
     * \return the delimiter between word and POS tag in the output result
     */
    const char* getPOSDelimiter() const;

    /**
     * Set the delimiter between the pairs (word and POS tag) in the output result of \e runWithString() and \e runWithStream(), which delimiter is "  " (double-space) defaultly so that the result would be "word/pos  word/pos  ...".
     * \param delimiter the delimiter between the pairs (word and POS tag) in the output result
     */
    void setWordDelimiter(const char* delimiter);

    /**
     * Get the delimiter between the pairs (word and POS tag) in the output result of \e runWithString() and \e runWithStream(), which delimiter is "  " (double-space) defaultly so that the result would be "word/pos  word/pos  ...".
     * \return the delimiter between the pairs (word and POS tag) in the output result
     */
    const char* getWordDelimiter() const;

private:
    /** option values */
    std::vector<double> options_;

protected:
    /** the delimiter between word and POS tag in the output result */
    const char* posDelimiter_;

    /** the delimiter between the pairs (word and POS tag) in the output result */
    const char* wordDelimiter_;
};

} // namespace jma

#endif // JMA_ANALYZER_H
