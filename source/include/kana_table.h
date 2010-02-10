/** \file kana_table.h
 * Definition of class KanaTable.
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Feb 10, 2010
 */

#ifndef JMA_KANA_TABLE_H
#define JMA_KANA_TABLE_H

#include "knowledge.h" // Knowledge::EncodeType

#include <string>
#include <map>

namespace jma
{

/**
 * KanaTable stores the mapping table between Japanese Hiragana and Katakana characters.
 */
class KanaTable
{
public:
    /**
     * Constructor.
     */
    KanaTable();

    /**
     * POS format type.
     */
    enum POSFormat
    {
        POS_FORMAT_DEFAULT, ///< default Japanese format such like "名詞,一般".
        POS_FORMAT_ALPHABET, ///< Alphabet format such like "NC-G".
        POS_FORMAT_FULL_CATEGORY, ///< Japanese format of furl category such like "名詞,一般,*,*".
        POS_FORMAT_NUM ///< the count of format types
    };

    /**
     * Load the configuration file "kana-map.def", which is in text format.
     * This file contains the mapping table between Hiragana and Katakana characters, which format is "Hiragana Katakana".
     * \param fileName the file name
     * \param src source encode type of \e fileName
     * \param dest destination encode type to convert
     * \return true for success, false for fail
     * \attention if this function is already called before, the table previously loaded would be removed.
     */
    bool loadConfig(const char* fileName, Knowledge::EncodeType src, Knowledge::EncodeType dest);

    /**
     * Convert Japanese Katakana character to its Hiragana equivalent, other character is returned as 0.
     * \param str the character string
     * \return if \e str is Katakana character, its Hiragana equivalent is returned, otherwise 0 is returned
     */
    const char* toHiragana(const char* str) const;

    /**
     * Convert Japanese Hiragana character to its Katakana equivalent, other character is returned as 0.
     * \param str the character string
     * \return if \e str is Hiragana character, its Katakana equivalent is returned, otherwise 0 is returned
     */
    const char* toKatakana(const char* str) const;

private:
    /** type of mapping between kana characters */
    typedef std::map<std::string, std::string> KanaMap;

    /** map from Katakana character to its Hiragana equivalent */
    KanaMap mapToHiragana_;

    /** map from Hiragana character to its Katakana equivalent */
    KanaMap mapToKatakana_;
};

} // namespace jma

#endif // JMA_KANA_TABLE_H
