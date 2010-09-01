/** \file knowledge.h
 * Definition of class Knowledge.
 *
 * \author Jun Jiang
 * \version 0.1
 * \date Jun 12, 2009
 */

#ifndef JMA_KNOWLEDGE_H
#define JMA_KNOWLEDGE_H

#include <string>
#include <vector>
#include <set>
#include <utility> // std::pair

namespace jma
{

/**
 * Knowledge manages the linguistic information for Japanese morphological analysis.
 */
class Knowledge
{
public:
    /**
     * Encode type of characters.
     */
    enum EncodeType
    {
        ENCODE_TYPE_EUCJP, ///< EUC-JP character type
        ENCODE_TYPE_SJIS, ///< SHIFT-JIS character type
        ENCODE_TYPE_UTF8, ///< UTF-8 character type
        ENCODE_TYPE_NUM ///< the count of character types
    };

    /**
     * Constructor.
     */
    Knowledge();

    /**
     * Destructor.
     */
    virtual ~Knowledge();

    /**
     * Set the directory path of system dictionary files, which are in binary format.
     * \param dirPath the directory path
     */
    void setSystemDict(const char* dirPath);

    /**
     * Add the user dictionary file, which is in text format,
     * the source encoding type of user dictionary file is also given in parameter, which is UTF-8 by default,
     * this function could be called more than once, so that multiple user dictionaries could be added.
     * \param fileName the file path of user dictionary
     * \param type the source encoding type of user dictionary, UTF-8 is assumed if omitted
     */
    void addUserDict(const char* fileName, EncodeType type = ENCODE_TYPE_UTF8);

    /**
     * Set the part-of-speech tags as keywords.
     * If this method is not called or \e posVec is empty,
     * the results of \e Analyzer::runWith*() would contain all words.
     * Otherwise, if \e posVec is not empty,
     * the results of \e Analyzer::runWith*() would only contain the words with part-of-speech tags specified by this method.
     * \param posVec the vector of part-of-speech index codes
     */
    void setKeywordPOS(const std::vector<int>& posVec);

    /**
     * Load the dictionaries, which are set by \e setSystemDict() and \e addUserDict().
     * \return 0 for fail, 1 for success
     * \pre the directory path of system dictionary should be set by \e setSystemDict().
     * \pre if any user dictionary file need to be loaded, the file name should be added by \e addUserDict().
     */
    virtual int loadDict() = 0;

    /**
     * Load the stop-word dictionary file, which is in text format.
     * The words in this file are ignored in the morphological analysis result.
     * \param fileName the file name
     * \return 0 for fail, 1 for success
     */
    virtual int loadStopWordDict(const char* fileName) = 0;

    /**
	 * Load the sentence separator configuration file, which is in text format.
	 * This file each separator character(only one character) per line.
	 * \param fileName the file name
	 * \return 0 for fail, 1 for success
	 */
	virtual int loadSentenceSeparatorConfig(const char* fileName) = 0;

    /**
     * Encode the system dictionary files from text to binary file.
     * \param txtDirPath the directory path of text files
     * \param binDirPath the directory path of binary files
     * \param binEncodeType the encoding type of binary system dictionary
     * \return 0 for fail, 1 for success
     * \pre both \e txtDirPath and \e binDirPath are assumed as the directory paths that already exists. If any of the directory paths does not exist, 0 would be returned for fail.
     */
    virtual int encodeSystemDict(const char* txtDirPath, const char* binDirPath, EncodeType binEncodeType) = 0;

    /**
     * Set the character encode type.
     * If this function is not called, the default value returned by \e getEncodeType() is \e ENCODE_TYPE_EUCJP.
     * \param type the encode type
     */
    void setEncodeType(EncodeType type);

    /**
     * Get the character encode type.
     * \return the encode type
     */
    EncodeType getEncodeType() const;

    /**
     * Get the encode type from the encode type string
     *
     * \param encodeStr encode type string
     * \return the encoding type, note that \e Knowledge::ENCODE_TYPE_NUM would be returned if the encoding type is unkown.
     */
    static EncodeType decodeEncodeType(const char* encodeStr);

    /**
     * Get the encode type string from the encode type
     *
     * \param encodeType encode type
     * \return the encoding type string, note that 0 would be returned if the encoding type is unkown.
     */
    static const char* encodeStr(EncodeType encodeType);

protected:
    /**
     * Invoked when the encode type is changed (except for the initialization)
     * \param type the new EncodeType
     */
    virtual void onEncodeTypeChange(EncodeType type) = 0;

protected:
    /** the directory path of system dictionary files */
    std::string systemDictPath_;

    /** user dictionary file type, it is a pair of file name and its encoding type */
    typedef std::pair<std::string, EncodeType> UserDictFileType;

    /** the file names of user dictionaries */
    std::vector<UserDictFileType> userDictNames_;

    /** the part-of-speech index codes as keywords */
    std::set<int> keywordPOSSet_;

private:
    /** character encode type */
    EncodeType encodeType_;

    /** the string of each encoding type */
    static const char* ENCODE_TYPE_STR_[ENCODE_TYPE_NUM];
};

} // namespace jma

#endif // JMA_KNOWLEDGE_H
