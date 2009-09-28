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
#include "pos_table.h"

#include <string>
#include <set>
#include <map>
#include <fstream>

using std::string;
using std::set;
using std::map;
using std::ofstream;
using std::ifstream;

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
     * Create tagger by loading dictionary files.
     * \return pointer to tagger. 0 for fail, otherwise the life cycle of the tagger should be maintained by the caller.
     */
    MeCab::Tagger* createTagger() const;

    /**
     * Get the part-of-speech tags table.
     * \return pointer to the table instance.
     */
    const POSTable* getPOSTable() const;

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
     * Get the current JMA_CType
     * \return the current JMA_CType
     */
    JMA_CType* getCType();

    /**
	 * Load the sentence separator configuration file, which is in text format.
	 * This file each separator character(only one character) per line.
	 * \param fileName the file name
	 * \return 0 for fail, 1 for success
	 */
	virtual int loadSentenceSeparatorConfig(const char* fileName);

protected:
    /**
     * Invoked when the encode type is changed (except for the initialization)
     * \param type the new EncodeType
     */
    virtual void onEncodeTypeChange(EncodeType type);

private:
    /**
     * Whether any user dictionary is added by \e Knowledge::addUserDict().
     * \return true for user dictionary is added, false for no user dictionary is added.
     */
    bool hasUserDict() const;

    /**
     * Compile the user dictionary file from text to binary format.
     * \return true for success, false for fail.
     * \pre \e systemDictPath_ is assumed as the directory path of system dictionary.
     * \pre \e userDictNames_ is assumed as file names of user dictionaries in text format.
     * \post as the compilation result, \e tempUserDic_ is the file name of the temporary user dictionary in binary format.
     */
    bool compileUserDict();

    /**
     * Load "pos-id.def" to get POS category number, and also POS table.
     * \return true for success, false for failure.
     */
    bool loadPOSDef();

    /**
     * Load property config file, with format key = value
     * \param filename the target file name
     * \param map the return values are stored in this map
     * \return true for success, false for failure.
     */
    bool loadConfig0(const char *filename, map<string, string>& map);

    /**
     * Load dictionary config file "dicrc" to get the values of entry defined by iJMA, such as "base-form-feature-offset" entry.
     * \return true for success, false for failure.
     */
    bool loadDictConfig();

    /**
     * Load the POS to feature mapping
     * \return true for success, false for failure.
     */
    bool loadPOSFeatureMapping();

    /**
     * Convert the User's txt file to CSV format, also change POS to feature
     * \param userDicFile user dictionary file
     * \param ostream csv output stream
     * \return how many entries are written into \e ostream
     */
    unsigned int convertTxtToCSV(const char* userDicFile, ofstream& ostream);

    /**
     * Whether the str is the dictionary feature, if it is, do not convert
     * \param the string to be checked
     * \param includedWord whether the str include the word
     * \return true if the str is the dictionary feature
     */
    bool isDictFeature( const char* str, bool includeWord = false );

    /**
     * Create a unique temporary file and output its file name.
     * \param tempName the string to save the temporary file name
     * \return true for success and \e tempName is set as the temporary file name. false for fail and \e tempName is not modified.
     */
    static bool createTempFile(std::string& tempName);

    /**
     * Delete the file on disk.
     * \param fileName the name of the file to be deleted
     * \return true for success and false for fail.
     */
    static bool removeFile(const std::string& fileName);

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
    /** the table of part-of-speech tags */
    POSTable posTable_;

    /** temporary file name for binary user dictionary */
    std::string tempUserDic_;

    /** stop words set */
    set<string> stopWords_;

    /** the pos to feature mapping */
    map<string, string> posFeatureMap_;

    /** whether POS result is in the format of full category */
    bool isOutputFullPOS_;

    /** POS category number, which value is got from "pos-id.def" in the directory of system dictionary in binary type */
    int posCatNum_;

    /** the feature offset (starting from zero) of base form, which value is got from entry "base-form-feature-offset" in "dicrc" in the directory of system dictionary in binary type */
    int baseFormOffset_;

    /** the tokens size in a feature */
    size_t featureTokenSize_;

    /** default pos in the user dictionary if not set or set to wrong (not exists) */
    string defaultPOS_;

    /** default feature which gained from defaultPOS_ */
    string* defaultFeature_;

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
