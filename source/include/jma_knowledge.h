/** \file jma_knowledge.h
 * Definition of class JMA_Knowledge.
 *
 * \author Jun Jiang
 * \version 0.1
 * \date Jun 17, 2009
 */

#ifndef JMA_KNOWLEDGE_IMPL_H
#define JMA_KNOWLEDGE_IMPL_H

#include "knowledge.h"
#include "generic_array.h"
#include "jma_ctype.h"

#include <string>
#include <set>

using std::string;
using std::set;

namespace MeCab
{
class Tagger;
} // namespace MeCab

namespace jma
{

/**
 * JMA_Knowledge manages the linguistic information for Japanese morphological analysis.
 */
class JMA_Knowledge : public Knowledge
{
public:
	typedef GenericArray<unsigned int> SepArray;

    /**
     * Constructor.
     */
    JMA_Knowledge();

    /**
     * Destructor.
     */
    virtual ~JMA_Knowledge();

    /**
     * Load the dictionaries, which are set by \e setSystemDict() and \e addUserDict().
     * \return 0 for fail, 1 for success
     * \pre the directory path of system dictionary should be set by \e setSystemDict().
     * \pre if any user dictionary file need to be loaded, the file name should be added by \e addUserDict().
     */
    virtual int loadDict();

    /**
     * Load the stop-word dictionary file, which is in text format.
     * The words in this file are ignored in the morphological analysis result.
     * \param fileName the file name
     * \return 0 for fail, 1 for success
     */
    virtual int loadStopWordDict(const char* fileName);

    /**
     * Load the configuration file, which is in text format.
     * This file contains the configuration used in morphological analysis, such as part-of-speech configuration, etc.
     * \param fileName the file name
     * \return 0 for fail, 1 for success
     */
    virtual int loadConfig(const char* fileName);

    /**
     * Encode the system dictionary files from text to binary format.
     * \param txtDirPath the directory path of text files
     * \param binDirPath the directory path of binary files
     * \return 0 for fail, 1 for success
     * \pre both \e txtDirPath and \e binDirPath are assumed as the directory paths that already exists. If any of the directory paths does not exist, 0 would be returned for fail.
     */
    virtual int encodeSystemDict(const char* txtDirPath, const char* binDirPath);

    /**
     * Get the tagger for analysis.
     * \return pointer to tagger
     */
    MeCab::Tagger* getTagger() const;

    /**
	 * Whether the specific word is stop word
	 *
	 * \param word the word to be checked
	 * \return whether the word is in the stop word list
	 */
	bool isStopWord(const string& word) const;

    /**
     * Whether POS result is in the format of full category.
     * \return true for full category format, which might includes asterisk symbol, such like "名詞,数,*,*";
     *         false for effective category format, which excludes asterisk symbol, such like "名詞,数";
     */
    bool isOutputFullPOS() const;

    /**
     * Get POS category number.
     * For example, if dictionary file contains POS like "名詞,固有名詞,人名,名", the POS category number would be 4.
     * \return POS category number.
     */
    int getPOSCatNum() const;

    /**
     * Get the feature offset of base form.
     * For example, if an entry in dictionary file contains the features like "動詞,自立,*,*,一段,未然形,見る,ミ,ミ", the feature offset of base form "見る" would be 6.
     * \return POS category number.
     */
    int getBaseFormOffset() const;

    /**
     * Check whether is a seperator of sentence.
     * \param p pointer to the character string
     * \return true for separator, false for non separator.
     */
    bool isSentenceSeparator(const char* p);

    /**
     * Add the sentence separator, the duplicated one would be ignored (return false)
     * \param val the sentence separator to be add
     * \return true if the val not exists in the current sentence separators and add successfully
     */
    bool addSentenceSeparator(unsigned int val);

    /**
     * Invoked when the encode type is changed (except for the initialization)
     * \param type the new EncodeType
     */
    virtual void onEncodeTypeChange(EncodeType type);

    /**
     * Get the current JMA_CType
     * \param the current JMA_CType
     */
    JMA_CType* getCType();

    /**
	 * Load the sentence separator configuration file, which is in text format.
	 * This file each separator character(only one character) per line.
	 * \param fileName the file name
	 * \return 0 for fail, 1 for success
	 */
	virtual int loadSentenceSeparatorConfig(const char* fileName);

private:
    /**
     * Remove the tagger and temporary dictionary file if exists.
     */
    void clear();

    /**
     * Create tagger by loading dictionary files.
     * \return the tagger created. 0 is returned when failure.
     */
    MeCab::Tagger* createTagger();

    /**
     * Load "pos-id.def" to get POS category number.
     * \return true for success, false for failure.
     */
    bool loadPOSDef();

    /**
     * Load dictionary config file "dicrc" to get base form feature offset value from entry "base-form-feature-offset".
     * \return true for success, false for failure.
     */
    bool loadDictConfig();

    /**
     * Create a unique temporary file and output its file name.
     * \param tempName the string to save the temporary file name
     * \return true for success and \e tempName is set as the temporary file name. false for fail and \e tempName is not modified.
     */
    static bool createTempFile(std::string& tempName);

    /**
     * Check whether the directory exists.
     * \param dirPath the directory path to be checked
     * \return true for the direcotory exists, false for not exists.
     */
    static bool isDirExist(const char* dirPath);

    /**
     * Copy a file from source path to destination path.
     * \param src the source path
     * \param dest the destination path
     * \return true for copy successfully, false for copy failed.
     */
    static bool copyFile(const char* src, const char* dest);

    /**
     * Create the complete file path from directory path and file name.
     * \param dir the directory path
     * \param file the file name
     * \return the file path string consisting of directory path and file name.
     * \pre the file name \e file is assumed as non empty.
     */
    static std::string createFilePath(const char* dir, const char* file);


	/**
	 * Get the number of bytes the character val occupies.
	 * \param val the character to be checked
	 * \return the number of bytes the character val occupies
	 */
	unsigned int getOccupiedBytes(unsigned int val);

private:
    /** tagger for analysis */
    MeCab::Tagger* tagger_;

    /** temporary file name for binary user dictionary */
    std::string tempUserDic_;

    /** stop words set */
    set<string> stopWords_;

    /** whether POS result is in the format of full category */
    bool isOutputFullPOS_;

    /** POS category number, which value is got from "pos-id.def" in the directory of system dictionary in binary type */
    int posCatNum_;

    /** the feature offset (starting from zero) of base form, which value is got from entry "base-form-feature-offset" in "dicrc" in the directory of system dictionary in binary type */
    int baseFormOffset_;

    /** The Character Type */
    JMA_CType* ctype_;

    /**
	 * Separator Arrays, index 0 is reserved, and index 1 ~ 4 represents
	 * character with 1 ~ 4 bytes separately.
	 */
	SepArray seps_[5];
};

} // namespace jma

#endif // JMA_KNOWLEDGE_IMPL_H
