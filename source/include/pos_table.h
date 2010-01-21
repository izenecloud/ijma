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
#include <map>

namespace jma
{

/**
 * POSRule defines rule to combine POS.
 * src1_ src2_ => target_
 * the above rule would combine src1_ and src2_ into target_.
 */
struct POSRule
{
    int src1_; ///< index code of 1st POS source
    int src2_; ///< index code of 2nd POS source
    int target_; ///< index code of POS target
};

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
     * \return true for success, false for fail
     * \attention if this function is already called before, the table previously loaded would be removed.
     */
    bool loadConfig(const char* fileName, Knowledge::EncodeType src, Knowledge::EncodeType dest);

    /**
     * Load the combination rule file "pos-combine.def", which is in text format.
     * Each entry in this file would be like "NC-G    NS-G    NC-G",
     * the left two columns are POS to combine from,
     * the last column is POS as combination result.
     * \param fileName the file name
     * \return true for success, false for fail
     * \attention if this function is already called before, the rules previously loaded would be removed.
     */
    bool loadCombineRule(const char* fileName);

    /**
     * From POS index code, get POS string in specific format.
     * \param index the POS index code
     * \return POS string, if non POS is available, the empty string is returned.
     */
    const char* getPOS(int index, POSFormat format = POS_FORMAT_DEFAULT) const;

    /**
     * Combine tokens using rules on POS, which rules are loaded by \e loadCombineRule().
     * \param pos1 the 1st POS
     * \param pos2 the 2nd POS
     * \return the POS of combination result, -1 for not to combine.
     */
    int combinePOS(int pos1, int pos2) const;

private:
    /**
     * Get POS index code from POS string in alphabet format.
     * \param posStr POS string in alphabet format
     * \return POS index code, -1 for not found.
     */
    int getIndexFromAlphaPOS(const std::string& posStr) const;

private:
    /** the POS tag tables for each format type */
    std::vector< std::vector<std::string> > strTableVec_;

    /** the size of each POS tag table */
    int tableSize_;

    /** map from POS alphabet format to index code */
    std::map<std::string, int> alphaPOSMap_;

    /** rules to combine POS */
    std::vector<POSRule> ruleVec_;
};

} // namespace jma

#endif // JMA_POS_TABLE_H
