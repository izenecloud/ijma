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

namespace jma
{

/**
 * Knowledge manages the linguistic information for Japanese morphological analysis.
 */
class Knowledge
{
public:
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
     * Add the file name of user dictionary file, which is in text format.
     * This function could be called more than once, so that multiple user dictionaries could be added.
     * \param fileName the file name
     */
    void addUserDict(const char* fileName);

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
     * Load the configuration file, which is in text format.
     * This file contains the configuration used in morphological analysis, such as part-of-speech configuration, etc.
     * \param fileName the file name
     * \return 0 for fail, 1 for success
     */
    virtual int loadConfig(const char* fileName) = 0;

    /**
	 * Load the sentence separator configuration file, which is in text format.
	 * This file each separator character(only one character) per line.
	 * \param fileName the file name
	 * \return 0 for fail, 1 for success
	 */
	virtual int loadSentenceSeparatorConfig(const char* fileName) = 0;

    /**
     * Encode the system dictionary files from text to binary format.
     * \param txtDirPath the directory path of text files
     * \param binDirPath the directory path of binary files
     * \return 0 for fail, 1 for success
     */
    virtual int encodeSystemDict(const char* txtDirPath, const char* binDirPath) = 0;

    /**
     * Encode type of characters.
     */
    enum EncodeType
    {
        ENCODE_TYPE_EUCJP, ///< EUC-JP character type
        ENCODE_TYPE_SJIS, ///< SHIFT-JIS character type
        ENCODE_TYPE_NUM ///< the count of character types
    };

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
     * Invoked when the encode type is changed (except for the initialization)
     * \param type the new EncodeType
     */
    virtual void onEncodeTypeChange(EncodeType type) = 0;

protected:
    /** the directory path of system dictionary files */
    std::string systemDictPath_;

    /** the file names of user dictionaries */
    std::vector<std::string> userDictNames_;

    /** the string of each encoding type */
    static const char* ENCODE_TYPE_STR_[ENCODE_TYPE_NUM];

private:
    /** character encode type */
    EncodeType encodeType_;
};

} // namespace jma

#endif // JMA_KNOWLEDGE_H
