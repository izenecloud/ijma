/** \file test_gen_posid_def.cpp
 * Test to generate "pos-id.def" from "*.csv" files in source directory of system dictionary.
 * Below is the usage examples:
 * \code
 * "pos-id.def" defines POS index number for each POS string, which appears in "*.csv" files in source directory of system dictionary.
 *
 * Below is an example of generating "pos-id.def", which was then moved into source directory as a definition file for POS index number.
 * $ ./jma_gen_posid_def ../db/unidic/src pos-id.def
 * how many POS columns in *.csv files? (input 4 as there are four columns as POS category strings)
 * $ mv pos-id.def ../db/unidic/src/
 * \endcode
 *
 * \author Jun Jiang
 * \version 0.1
 * \date Jun 29, 2009
 */

#include <vector>
#include <set>
#include <iostream>
#include <fstream>
#include <cassert>
#include <cstdlib>
#include <string.h>

#if defined(_WIN32) && !defined(__CYGWIN__)
#include <windows.h> // FindFirstFile, FindClose
#else
#include <dirent.h> // opendir, closedir
#endif

//#define JMA_DEBUG_PRINT 1

using namespace std;

bool toLower(string& s)
{
    for (size_t i = 0; i < s.size(); ++i)
    {
        char c = s[i];
        if ((c >= 'A') && (c <= 'Z'))
        {
            c += 'a' - 'A';
            s[i] = c;
        }
    }

    return true;
}

string create_filename(const string &path, const string &file)
{
    string s = path;
#if defined(_WIN32) && !defined(__CYGWIN__)
    if (s.size() && s[s.size()-1] != '\\') s += '\\';
#else
    if (s.size() && s[s.size()-1] != '/') s += '/';
#endif
    s += file;
    return s;
}

bool enum_csv_dictionaries(const char *path, vector<string>& dics)
{
    dics.clear();

#if defined(_WIN32) && !defined(__CYGWIN__)
    WIN32_FIND_DATA wfd;
    HANDLE hFind;
    const string pat = create_filename(path, "*.csv");
    hFind = FindFirstFile(pat.c_str(), &wfd);
    if(hFind == INVALID_HANDLE_VALUE)
    {
        cerr << "Invalid File Handle. Get Last Error reports";
        return false;
    }
    do {
        string tmp = create_filename(path, wfd.cFileName);
        dics.push_back(tmp);
    } while (FindNextFile(hFind, &wfd));
    FindClose(hFind);
#else
    DIR *dir = opendir(path);
    if(dir == 0)
    {
        cerr << "no such directory: " << path << endl;
        return false;
    }

    for (struct dirent *dp = readdir(dir); dp; dp = readdir(dir)) {
        const string tmp = dp->d_name;
        if (tmp.size() >= 5) {
            string ext = tmp.substr(tmp.size() - 4, 4);
            toLower(ext);
            if (ext == ".csv")
                dics.push_back(create_filename(path, tmp));
        }
    }
    closedir(dir);
#endif
    return true;
}

/**
 * Find the index of the nth separator "," in string.
 * \param str string value to find
 * \param n count of separator, starting from 1
 * \return the separator index, string::npos means no result found.
 */
string::size_type findSeparator(const string& str, size_t n)
{
    string::size_type r = 0;

#if JMA_DEBUG_PRINT
    cout << str << endl;
#endif
    for(size_t i=0; i<n && r<str.size(); ++i)
    {
        // skip the element between ""
        if(str[r] == '"')
        {
            r = str.find('"', r+1);
        }

        r = str.find(',', r);
#if JMA_DEBUG_PRINT
        cout << r << endl;
#endif
        if(r == string::npos)
        {
            break;
        }

        // forward index
        ++r;
    }
#if JMA_DEBUG_PRINT
    cout << endl;
#endif

    if(r != string::npos)
    {
        // backward index to separator
        --r;
    }

    return r;
}

/**
 * Extract POS string from "*.csv" files.
 * \param dicts the file names of "*.csv"
 * \param posSet the extraction result
 * \param posColumn the number of POS columns in "*.csv"
 */
bool extractPOS(const vector<string>& dics, set<string>& posSet, size_t posColumn)
{
    // start column of POS in "*.csv" file
    size_t posStart = 4;

    for(size_t i=0; i<dics.size(); ++i)
    {
        ifstream from(dics[i].c_str());
        if(! from)
        {
            cerr << "failed to open file " << dics[i] << endl;
            return false;
        }

        string line;
        string::size_type i, j;
        while(getline(from, line))
        {
            if(! line.empty())
            {
#if JMA_DEBUG_PRINT
                cout << line << endl;
#endif
                i = findSeparator(line, posStart);
                j = findSeparator(line, posStart + posColumn);
#if JMA_DEBUG_PRINT
                cout << i << ", " << j << endl;
#endif

                if(i != string::npos && j != string::npos)
                {
                    posSet.insert(line.substr(i+1, j-i-1));
#if JMA_DEBUG_PRINT
                    cout << line.substr(i+1, j-i-1) << endl;
                    cout << endl;
#endif
                }
            }
        }
    }

    return true;
}

/**
 * Print the test usage.
 */
void printUsage()
{
    cerr << "Usage: ./gen_posid_def SOURCE_DIR POSID_DEF" << endl;
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

    const char* srcPath = argv[1];
    const char* posFile = argv[2];

    vector<string> dics;
    bool r = enum_csv_dictionaries(srcPath, dics);
    if(! r)
    {
        cerr << "error in enum_csv_dictionaries()" << endl;
        exit(1);
    }

#if JMA_DEBUG_PRINT
    cout << dics.size() << " *.csv in srcPath: " << srcPath << endl;
    for(size_t i=0; i<dics.size(); ++i)
    {
        cout << dics[i] << endl;
    }
    cout << endl;
#endif

    size_t posColumn;
    cout << "how many POS columns in *.csv files?" << endl;
    cin >> posColumn;

    set<string> posSet;
    r = extractPOS(dics, posSet, posColumn);
    if(! r)
    {
        cerr << "error in extractPOS()" << endl;
        exit(1);
    }

#if JMA_DEBUG_PRINT
    cout << posSet.size() << " POS:" << endl;
    for(set<string>::const_iterator i=posSet.begin(); i!=posSet.end(); ++i)
    {
        cout << *i << endl;
    }
    cout << endl;
#endif

    ofstream to(posFile);
    if(! to)
    {
        cerr << "failed to create file " << posFile << endl;
        exit(1);
    }

    size_t index = 0;
    for(set<string>::const_iterator i=posSet.begin(); i!=posSet.end(); ++i)
    {
        to << *i << " " << index++ << endl;
    }

    cout << "succeeded in generating " << posFile << " from " << srcPath << endl;

    return 0;
}
