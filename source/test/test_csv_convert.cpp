/** \file test_csv_convert.cpp
 * Executable to convert dictionary CSV file from source pattern to destination pattern, which conversion rule is defined in a configuration file, such as "csv-convert-ipa-juman-eucjp.def".
 * Below is the usage examples:
 * \code
 * The encoding type of those CSV files in source directory is predefined by the "dictionary-charset" entry of "dicrc" file.
 * While the encoding type of binary files in destination directory could be set to "eucjp", "sjis" or "utf8" like below, which is "eucjp" defaultly.
 * $ ./jma_csv_convert --config ../db/config/csv-convert-ipa-juman-eucjp.def --output ipa_proper.csv ../db/ipadic/src/Noun.proper.csv ../db/ipadic/src/Noun.name.csv ../db/ipadic/src/Noun.org.csv ../db/ipadic/src/Noun.place.csv
 * \endcode
 *
 * \author Jun Jiang
 * \version 0.1
 * \date Sep 18, 2009
 */

#include "dictionary_rewriter.h"
#include "utils.h"
#include "common.h"

#include <iostream>
#include <fstream>
#include <cassert>
#include <cstdlib>
#include <cstring>

using namespace std;
using namespace MeCab;

namespace
{
/**
 * Print the test usage.
 */
void printUsage()
{
    cerr << "Usage: ./jma_csv_convert --config DEF_FILE --output DEST_FILE SOURCE_CSV..." << endl;
}

/**
 * Check whether the command \e option matches the \e name.
 * \param str the command option string given by user.
 * \param option the standard option name.
 * \attention if the match is failed, print out the command usage information, and terminate the whole pragram. 
 */
void checkOptionName(const char* str, const char* option)
{
    if(strcmp(str, option) != 0)
    {
        cerr << "unknown command option " << str << ", which should be " << option << endl;
        printUsage();
        exit(1);
    }
}

/**
 * Append the rule in string \e str to the rule vector \e r.
 * \param r the rule vector.
 * \param str the rule string consisting of source and destination pattern.
 */
void append_rewrite_rule(RewriteRules *r, char* str)
{
    char *col[3];
    const size_t n = tokenize2(str, " \t", col, 3);
    CHECK_DIE(n >= 2) << "format error: " << str;
    r->resize(r->size() + 1);
    string tmp;
    if (n >= 3) {
        tmp = col[1];
        tmp += ' ';
        tmp += col[2];
        col[1] = const_cast<char *>(tmp.c_str());
    }
    r->back().set_pattern(col[0], col[1]);
}

/**
 * Rewrite dictionary CSV file.
 */
class CSVRewriter
{
private:
    /** rules consisting of source and destination pattern */
    RewriteRules rules_;

public:
    /**
     * Open configuration file, which defines the source and destination patterns.
     * \param fileName configuration file name
     * \return true for success, false for failure.
     */
    bool open(const char* fileName);

    /**
     * Rewrite the string \e src to \e dest, basing on the patterns in configuration file by \e open().
     * \param src source string to convert from
     * \param dest destination string as output
     * \return true for success, false for failure.
     */
    bool rewrite(const string& src, string& dest) const;
};

bool CSVRewriter::open(const char* fileName)
{
    ifstream ifs(fileName);
    if(! ifs)
    {
        cerr << "fail to open CSVRewriter configuration file: " << fileName << endl;
        return false;
    }

    string line;
    while(getline(ifs, line))
    {
        if(line.empty() || line[0] == '#')
        {
            continue;
        }

        char* str = const_cast<char*>(line.c_str());
        append_rewrite_rule(&rules_, str);
    }

    return true;
}

bool CSVRewriter::rewrite(const string& src, string& dest) const
{
    char buf[BUF_SIZE];
    char *col[BUF_SIZE];
    CHECK_DIE(src.size() < sizeof(buf) - 1) << "too many characters in CSV entry";
    strncpy(buf, src.c_str(), sizeof(buf) - 1);
    size_t n = tokenizeCSV(buf, col, sizeof(col));
    CHECK_DIE(n < sizeof(col)) << "too many columns in CSV entry";
    return rules_.rewrite(n, const_cast<const char **>(col), &dest);
}

}

/**
 * Main function.
 */
int main(int argc, char* argv[])
{
    if(argc < 6)
    {
        printUsage();
        exit(1);
    }

    checkOptionName(argv[1], "--config");
    checkOptionName(argv[3], "--output");

    const char* configFile = argv[2];
    cout << "config file: " << configFile << endl;

    // CSV file rewriter
    CSVRewriter rewriter;
    if(! rewriter.open(configFile))
    {
        cerr << "fail to open config file: " << configFile << endl;
        exit(1);
    }

    const char* destFile = argv[4];
    ofstream to(destFile);
    if(! to)
    {
        cerr << "failed to create file " << destFile << endl;
        exit(1);
    }

    unsigned int count = 0;
    unsigned int ignore = 0;
    for(int i=5; i<argc; ++i)
    {
        const char* sourceFile = argv[i];
        cout << "converting source file: " << sourceFile << endl;

        ifstream from(sourceFile);
        if(! from)
        {
            cerr << "failed to open source file: " << sourceFile << endl;
            exit(1);
        }

        string line, dest;
        while(getline(from, line))
        {
            if(line.empty())
                continue;

            if(! rewriter.rewrite(line, dest))
            {
                cerr << "as no pattern is matched, ignoring line: " << line << endl;
                ++ignore;
                continue;
            }

            to << dest << endl;
            ++count;
        }
    }

    cout << "succeeded output to " << destFile << endl;
    cout << "total ignored entries: " << ignore << endl;
    cout << "total converted entries: " << count << endl;
    return 0;
}
