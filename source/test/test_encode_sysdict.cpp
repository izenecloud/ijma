/** \file test_encode_sysdict.cpp
 * Test for encoding the system dictionary from text type to binary type.
 * Below is the usage examples:
 * \code
 * The encoding type of those CSV files in source directory is predefined by the "dictionary-charset" entry of "dicrc" file.
 * While the encoding type of binary files in destination directory could be set to "eucjp" or "sjis" like below, which is "eucjp" defaultly.
 * $ ./jma_encode_sysdict --encode eucjp ../db/ipadic/src ../db/ipadic/bin_eucjp
 * $ ./jma_encode_sysdict --encode sjis ../db/ipadic/src ../db/ipadic/bin_sjis
 * \endcode
 *
 * \author Jun Jiang
 * \version 0.1
 * \date Jun 22, 2009
 */

#include "jma_factory.h"
#include "knowledge.h"

#include <iostream>
#include <cassert>
#include <cstdlib>
#include <string.h>

using namespace std;
using namespace jma;

/**
 * Print the test usage.
 */
void printUsage()
{
    cerr << "Usage: ./test_encode_sysdict [--encode [eucjp,sjis]] SOURCE_DIR DEST_DIR" << endl;
    cerr << "       (please ensure that both 'SOURCE_DIR' and 'DEST_DIR' exists.)" << endl;
}

/**
 * Main function.
 */
int main(int argc, char* argv[])
{
    if(argc < 3)
    {
        printUsage();
        exit(1);
    }

    Knowledge::EncodeType encode = Knowledge::ENCODE_TYPE_EUCJP;

    const char* srcDir;
    const char* destDir;
    srcDir = destDir = 0;

    if(argc == 5)
    {
        if(strcmp(argv[1], "--encode") != 0)
        {
            cerr << "unknown command option " << argv[1] << endl;
            printUsage();
            exit(1);
        }

        if(strcmp(argv[2], "eucjp") == 0)
        {
            encode = Knowledge::ENCODE_TYPE_EUCJP;
        }
        else if(strcmp(argv[2], "sjis") == 0)
        {
            encode = Knowledge::ENCODE_TYPE_SJIS;
        }
        else
        {
            cerr << "unknown encode type " << argv[2] << endl;
            printUsage();
            exit(1);
        }

        srcDir = argv[3];
        destDir = argv[4];
    }
    else if(argc == 3)
    {
        srcDir = argv[1];
        destDir = argv[2];
    }
    else
    {
        cerr << "The number of command options is wrong." << endl;
        printUsage();
        exit(1);
    }

    // create knowledge
    JMA_Factory* factory = JMA_Factory::instance();
    Knowledge* knowledge = factory->createKnowledge();

    // set encoding of destination files
    knowledge->setEncodeType(encode);

    // encoding
    int r = knowledge->encodeSystemDict(srcDir, destDir);

    delete knowledge;

    // print result
    if(r == 1)
    {
        cout << "succeeded";
    }
    else
    {
        cout << "failed";
    }
    cout << " in encoding from " << srcDir << " to " << destDir << endl;

    return 0;
}
