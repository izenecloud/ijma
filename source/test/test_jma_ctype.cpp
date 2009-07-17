/** \file test_jma_ctype.cpp
 * Test JMA_CType in processing EUC-JP, Shift-JIS, UTF-8 characters.
 * Below is the usage examples:
 * \code
 * Read characters from test data file in UTF-8 encoding, and print those characters information to standard output.
 * $./test_jma_ctype --encode utf8 ../db/test/test_jma_ctype_utf8.txt
 * \endcode
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Jul 17, 2009
 */

#include "jma_ctype.h"

#include <cassert>
#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <string.h>

using namespace std;
using namespace jma;

namespace
{
/** The maximum count of bytes for a character */
const unsigned int MAX_BYTE_LENGTH = 4; // UTF-8 has character with length at most 4 byte, and other encoding such as EUC-JP and Shift-JIS are at most 2 byte.
}

/**
 * Test method getByteCount.
 */
void testGetByteCount(JMA_CType* ctype, const char* str)
{
    cout << "########## test method getByteCount()" << endl;
    cout << "raw string:" << endl << str << endl << endl;

    char unit[MAX_BYTE_LENGTH+1];
    unsigned int bc = 0; // byte count
    unsigned int cc = 0; // character count

    cout << "characters delimited by space:" << endl;
    for(const char* p=str; *p; p+=bc, ++cc)
    {
        bc = ctype->getByteCount(p);
        if(bc == 0 || bc > MAX_BYTE_LENGTH)
        {
            cerr << "invalid byte count: " << bc << endl;
            exit(1);
        }

        for(unsigned int i=0; i<bc; ++i)
            unit[i] = *(p+i);
        unit[bc] = 0;

        cout << unit << " ";
    }

    cout << endl << endl << "character count: " << cc << endl << endl;

    size_t len = ctype->length(str);
    if(cc != len)
    {
        cerr << "the character count is not equal to JMA_CType::length(): " << len << endl;
        exit(1);
    }
}

/**
 * Print the test usage.
 */
void printUsage()
{
    cerr << "usage: ./test_jma_ctype --encode [eucjp,sjis,utf8] testFile" << endl;
}

/**
 * Main function.
 */
int main(int argc, char* argv[])
{
    if(argc != 4)
    {
        printUsage();
        exit(1);
    }

    if(strcmp(argv[1], "--encode") != 0)
    {
        cerr << "unkown command option " << argv[1] << endl;
        printUsage();
        exit(1);
    }

    Knowledge::EncodeType encode = Knowledge::decodeEncodeType(argv[2]);
    if(encode == Knowledge::ENCODE_TYPE_NUM)
    {
        cerr << "unkown encode type " << argv[2] << endl;
        printUsage();
        exit(1);
    }

    JMA_CType* ctype = JMA_CType::instance(encode);

    ifstream from(argv[3]);
    if(! from)
    {
        cerr << "erro open file: " << argv[3] << endl;
        exit(1);
    }

    string line;
    if(! getline(from, line))
    {
        cerr << "no input string for test method getByteCount()" << endl;
        exit(1);
    }
    testGetByteCount(ctype, line.c_str());

    return 0;
}
