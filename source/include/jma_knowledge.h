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
     */
    virtual int encodeSystemDict(const char* txtDirPath, const char* binDirPath);

    /**
     * Get the tagger for analysis.
     * \return pointer to tagger
     */
    MeCab::Tagger* getTagger() const;

private:
    /**
     * Remove the tagger and temporary dictionary file if exists.
     */
    void clear();

    /**
     * Create a unique temporary file and output its file name.
     * \param tempName the string to save the temporary file name
     * \return true for success and \e tempName is set as the temporary file name. false for fail and \e tempName is not modified.
     */
    static bool createTempFile(std::string& tempName);

private:
    /** tagger for analysis */
    MeCab::Tagger* tagger_;

    /** temporary file name for binary user dictionary */
    std::string tempUserDic_;
};

} // namespace jma

#endif // JMA_KNOWLEDGE_IMPL_H
