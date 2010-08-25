/** \file jma_dictionary.h
 * Definition of class JMA_Dictionary.
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Aug 02, 2010
 */

#ifndef JMA_DICTIONARY_H
#define JMA_DICTIONARY_H

#include <vector>
#include <string>
#include <map>

namespace jma
{

/**
 * DictUnit is one dictionary file.
 */
struct DictUnit
{
    /** start address */
    char* text_;

    /** total length */
    unsigned int length_;

    /** file name (without path) */
    std::string fileName_;

    /**
     * Constructor.
     */
    DictUnit() : text_(0), length_(0) {}
};

/**
 * JMA_Dictionary is an archive for dictionary files.
 */
class JMA_Dictionary
{
public:
    /**
     * Get the instance of \e JMA_Dictionary.
     * \return the pointer to instance
     */
    static JMA_Dictionary* instance();

    /**
     * Load the archive file.
     * \param fileName the full file name including path
     * \return true for success, false for failure
     */
    bool open(const char* fileName);

    /**
     * Close the opened system and user dictionary.
     */
    void close();

    /**
     * Create an empty binary user dictionary instance.
     * \param fileName the file name, the path would be ignored and only file name is saved
     */
    void createEmptyBinaryUserDict(const char* fileName);

    /**
     * Create an empty text user dictionary instance.
     * \param fileName the file name, the path would be ignored and only file name is saved
     */
    void createEmptyTextUserDict(const char* fileName);

    /**
     * Get a dictionary file.
     * \param fileName the file name, the path would be ignored and only file name is used to find the dictionary file
     * \return the pointer to dictionary file, 0 is returned if not loaded
     */
    DictUnit* getDict(const char* fileName);

    /**
     * Copy the string content to dictionary.
     * \param str the string content
     * \param fileName the file name, the path would be ignored and only file name is used to find the dictionary file
     * \return true for success, false for failure
     */
    bool copyStrToDict(const std::string& str, const char* fileName);

    /**
     * Complile dictionary files \e srcVec into archive \e destFile.
     * \param srcFiles the file names of dictionary files
     * \param destFile the archive file name
     * \return true for success, false for failure
     */
    static bool compile(const std::vector<std::string>& srcFiles, const char* destFile);

protected:
    /**
     * Constructor.
     */
    JMA_Dictionary();

    /**
     * Destructor.
     */
    virtual ~JMA_Dictionary();

private:
    /**
     * Close the system dictionary.
     */
    void closeSysDict();

    /**
     * Close binary user dictionary.
     */
    void closeBinUserDict();

    /**
     * Close text user dictionary.
     */
    void closeTxtUserDict();

private:
    /** the instance of dictionary */
    static JMA_Dictionary* instance_;

    /** the start address of system dictionary archive */
    char* sysDictAddr_;

    /** mapping from dictionary file name (without path) to dictionary instance */
    std::map<std::string, DictUnit> dictMap_;

    /** the binary user dictionary instance */
    DictUnit* binUserDict_;

    /** the text user dictionary instance */
    DictUnit* txtUserDict_;
};

} // namespace jma

#endif // JMA_DICTIONARY
