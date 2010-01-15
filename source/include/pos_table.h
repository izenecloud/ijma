/** \file pos_table.h
 * Definition of class POSTable.
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Sep 11, 2009
 */

#ifndef JMA_POS_TABLE_H
#define JMA_POS_TABLE_H

#include "knowledge.h" // Knowledge::EncodeType

#include <string>
#include <vector>

namespace jma
{

/**
 * POSTable stores the table of part-of-speech tags, each tag includes a string value and its index code.
 * Note that the string value of part-of-speech tag is in Alphabet format such like "NP-S".
 */
class POSTable
{
public:
    /**
     * Constructor.
     */
    POSTable();

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
     * Load the configuration file "pos-id.def", which is in text format.
     * This file contains the part-of-speech configuration, which format is "JapanesePOS index AlphabetPOS".
     * \param fileName the file name
     * \param src source encode type of \e fileName
     * \param dest destination encode type to convert
     * \return 0 for fail, 1 for success
     * \attention if this function is already called before, the table previously loaded would be removed.
     */
    bool loadConfig(const char* fileName, Knowledge::EncodeType src, Knowledge::EncodeType dest);

    /**
     * From POS index code, get POS string in specific format.
     * \param index the POS index code
     * \return POS string, if non POS is available, the empty string is returned.
     */
    const char* getPOS(int index, POSFormat format = POS_FORMAT_DEFAULT) const;

private:
    /** the POS tag tables for each format type */
    std::vector< std::vector<std::string> > strTableVec_;

    /** the size of each POS tag table */
    int tableSize_;
};

} // namespace jma

#endif // JMA_POS_TABLE_H
