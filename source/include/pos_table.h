/** \file pos_table.h
 * Definition of class POSTable.
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Sep 11, 2009
 */

#ifndef JMA_POS_TABLE_H
#define JMA_POS_TABLE_H

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
     * Load the configuration file "pos-id.def", which is in text format.
     * This file contains the part-of-speech configuration, which format is "JapanesePOS index AlphabetPOS".
     * \param fileName the file name
     * \return 0 for fail, 1 for success
     */
    bool loadConfig(const char* fileName);

    /**
     * Get the POS string from the POS index code in the part-of-speech table.
     * Note that the POS string is in Alphabet format such like "NP-S".
     * \param index the POS index code
     * \return POS string, if non POS is available, the empty string is returned.
     */
    const char* getAlphabetPOS(int index) const;

private:
    /** the POS tag table */
    std::vector<std::string> posAlphabetTable_;
};

} // namespace jma

#endif // JMA_POS_TABLE_H
