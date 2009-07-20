/** \file jma_knowledge.cpp
 * Implementation of class JMA_Knowledge.
 *
 * \author Jun Jiang
 * \version 0.1
 * \date Jun 17, 2009
 */

#include "jma_knowledge.h"
#include "sentence.h"
#include "dictionary.h"

#include "mecab.h" // MeCab::Tagger

#include <iostream>
#include <fstream> // ifstream, ofstream
#include <cstdlib> // mkstemp, atoi
#include <cassert>

#if defined(_WIN32) && !defined(__CYGWIN__)
#include <windows.h> // GetTempPath, GetTempFileName, FindFirstFile, FindClose
#else
#include <dirent.h> // opendir, closedir
#endif

#ifndef JMA_DEBUG_PRINT
	#define JMA_DEBUG_PRINT 1
#endif

using namespace std;

namespace
{
/** Tag string in JMA config file for whether output POS result in full category */
const char* CONFIG_TAG_OUTPUT_FULL_POS = "OUTPUT_FULL_POS";

/** Default value of POS category number */
const int POS_CAT_NUM_DEFAULT = 4;

/** Dictionary configure and definition files */
const char* DICT_CONFIG_FILES[] = {"dicrc", "rewrite.def", "left-id.def", "right-id.def"};

/** POS index definition file name */
const char* POS_ID_DEF_FILE = "pos-id.def";
}

namespace jma
{

JMA_Knowledge::JMA_Knowledge()
    : tagger_(0), isOutputFullPOS_(false), posCatNum_(POS_CAT_NUM_DEFAULT), ctype_(0)
{
    onEncodeTypeChange( getEncodeType() );
}

JMA_Knowledge::~JMA_Knowledge()
{
    clear();

    if(ctype_)
    {
        delete ctype_;
    }
}

void JMA_Knowledge::clear()
{
    // destroy the tagger if exists
    if(tagger_)
    {
        delete tagger_;
        tagger_ = 0;
    }

    // remove the temporary file if exists
    if(! tempUserDic_.empty())
    {
        int r = unlink(tempUserDic_.c_str());
        assert(r == 0 && "temporary file is removed successfully");
    }
}

int JMA_Knowledge::loadDict()
{
    const size_t userDictNum = userDictNames_.size();

#if JMA_DEBUG_PRINT
    cout << "JMA_Knowledge::loadDict()" << endl;
    cout << "system dictionary path: " << systemDictPath_ << endl;
    if(userDictNum != 0)
    {
        cout << "user dictionary: ";
        for(size_t i=0; i<userDictNum; ++i)
        {
            cout << userDictNames_[i] << " ";
        }
        cout << endl;
    }
#endif

    // remove the previous tagger and temporary file
    clear();

    // construct parameter to create tagger
    string taggerParam("-d ");
    taggerParam += systemDictPath_;

    if(userDictNum != 0)
    {
        // create a unique temporary file for user dictionary in binary type
        bool tempResult = createTempFile(tempUserDic_);
        if(! tempResult)
        {
            cerr << "fail to create a temporary user ditionary file" << endl;
            return false;
        }
#if JMA_DEBUG_PRINT
        cout << "temporary file name of binary user dictionary: " << tempUserDic_ << endl;
#endif

        // construct parameter to compile user dictionary
        vector<char*> compileParam;
        compileParam.push_back((char*)"JMA_Knowledge");
        compileParam.push_back((char*)"-d");
        compileParam.push_back(const_cast<char*>(systemDictPath_.c_str()));
        compileParam.push_back((char*)"-u");
        compileParam.push_back(const_cast<char*>(tempUserDic_.c_str()));

        // the encoding type of text user dictionary could be predefined by the "dictionary-charset" entry in "dicrc" file under binary system directory path,
        // if the text encoding type is not predefined in "dicrc", it would be "EUC-JP" defaultly.
        // below is to set the encoding type of binary user dictionary, which is "EUC-JP" defaultly.
        compileParam.push_back((char*)"-t");
        compileParam.push_back(const_cast<char*>(ENCODE_TYPE_STR_[getEncodeType()]));

        // append source files of user dictionary
        for(size_t i=0; i<userDictNum; ++i)
        {
            compileParam.push_back(const_cast<char*>(userDictNames_[i].c_str()));
        }

#if JMA_DEBUG_PRINT
        cout << "parameter of mecab_dict_index() to compile user dictionary: ";
        for(size_t i=0; i<compileParam.size(); ++i)
        {
            cout << compileParam[i] << " ";
        }
        cout << endl;
#endif

        // compile user dictionary files into binary type
        int compileResult = mecab_dict_index(compileParam.size(), &compileParam[0]);
        if(compileResult != 0)
        {
            cerr << "fail to compile user ditionary" << endl;
            return 0;
        }

        // append the name of user dictionary binary file to the parameter of tagger creation
        taggerParam += " -u ";
        taggerParam += tempUserDic_;
    }

#if JMA_DEBUG_PRINT
    cout << "parameter of MeCab::createTagger() to create MeCab::Tagger: " << taggerParam << endl << endl;
#endif

    // create tagger by loading dictioanry files
    tagger_ = MeCab::createTagger(taggerParam.c_str());

    // get POS category number from "pos-id.def"
    string posFileName = createFilePath(systemDictPath_.c_str(), POS_ID_DEF_FILE);
    ifstream posFile(posFileName.c_str());
    // if no "pos-id.def" exists, the default value of POS category number would be used.
    if(posFile)
    {
        // read in a non-emtpy line
        string line;
        while(getline(posFile, line) && line.empty()) ;

        // the format of the line is assumed such like "その他,間投,*,* 0"
        if(! line.empty())
        {
            // remove characters starting from space
            line.erase(line.find(' '));

            // count the number of separator ','
            int count = 0;
            for(size_t i=0; i<line.size(); ++i)
            {
                if(line[i] == ',')
                {
                    ++count;
                }
            }
            posCatNum_ = count + 1;
        }
#if JMA_DEBUG_PRINT
        cout << posCatNum_ << " POS categories in " << posFileName << endl;
#endif
    }

    return tagger_ ? 1 : 0;
}

int JMA_Knowledge::loadStopWordDict(const char* fileName)
{
	ifstream in(fileName);
    if(!in)
    {
        cerr << "cannot open file: " << fileName << endl;
        return 0;
    }

	string line;
	while(!in.eof())
	{
		getline(in, line);
		if(line.length() <= 0)
			continue;
		stopWords_.insert(line);
	}
	in.close();
	return 1;
}

int JMA_Knowledge::loadConfig(const char* fileName)
{
    // open file
    assert(fileName);
    ifstream from(fileName);
    if(!from) {
        cerr << "cannot open file: " << fileName << endl;
        return 0;
    }

#if JMA_DEBUG_PRINT
    cout << "load configuration: " << fileName << endl;
    cout << "tag\t\tvalue\t\tflag" << endl;
#endif

    // read file, which consists of lines in the format "tag = value"
    string line, tag, value;
    string::size_type i, j, k;
    const char* whitespaces = " \t";

    while(getline(from, line))
    {
        // remove carriage return character
        line = line.substr(0, line.find('\r'));

        // ignore the comment start with '#'
        if(!line.empty() && line[0] != '#')
        {
            // set k as the position of '='
            i = line.find_first_not_of(whitespaces);
            if(i == string::npos)
            {
                break;
            }

            k = line.find_first_of('=', i);
            if(k == string::npos || k == 0)
            {
                break;
            }

            // set tag
            j = line.find_last_not_of(whitespaces, k-1);
            if(j == string::npos)
            {
                break;
            }
            tag = line.substr(i, j-i+1);

            // set value
            i = line.find_first_not_of(whitespaces, k+1);
            if(i == string::npos)
            {
                break;
            }

            j = line.find_last_not_of(whitespaces);
            if(j == string::npos)
            {
                value = line.substr(i);
            }
            else
            {
                value = line.substr(i, j-i+1);
            }

            if(tag == CONFIG_TAG_OUTPUT_FULL_POS)
            {
                if(atoi(value.c_str()) == 0)
                {
                    isOutputFullPOS_ = false;
                }
                else
                {
                    isOutputFullPOS_ = true;
                }
            }
#if JMA_DEBUG_PRINT
            cout << tag << "\t" << value << "\t" << isOutputFullPOS_ << endl;
#endif
        }
    }

#if JMA_DEBUG_PRINT
    cout << endl;
#endif

    return 1;
}

int JMA_Knowledge::encodeSystemDict(const char* txtDirPath, const char* binDirPath)
{
    assert(txtDirPath && binDirPath);

#if JMA_DEBUG_PRINT
    cout << "JMA_Knowledge::encodeSystemDict()" << endl;
    cout << "path of source system dictionary: " << txtDirPath << endl;
    cout << "path of binary system dictionary: " << binDirPath << endl;
#endif

    // check if the directory paths exist
    if(isDirExist(txtDirPath) == false)
    {
        cerr << "directory path not exist to compile system dictionary: " << txtDirPath << endl;
        return 0;
    }
    if(isDirExist(binDirPath) == false)
    {
        cerr << "directory path not exist to compile system dictionary: " << binDirPath << endl;
        return 0;
    }

    // construct parameter to compile system dictionary
    vector<char*> compileParam;
    compileParam.push_back((char*)"JMA_Knowledge");
    compileParam.push_back((char*)"-d");
    compileParam.push_back(const_cast<char*>(txtDirPath));
    compileParam.push_back((char*)"-o");
    compileParam.push_back(const_cast<char*>(binDirPath));

    // the source encoding type could be predefined by the "dictionary-charset" entry in "dicrc" file under source directory path,
    // if the source encoding type is not predefined in "dicrc", it would be "EUC-JP" defaultly.
    // below is to set the destination encoding type, which is "EUC-JP" defaultly.
    compileParam.push_back((char*)"-t");
    compileParam.push_back(const_cast<char*>(ENCODE_TYPE_STR_[getEncodeType()]));

#if JMA_DEBUG_PRINT
    cout << "parameter of mecab_dict_index() to compile system dictionary: ";
    for(size_t i=0; i<compileParam.size(); ++i)
    {
        cout << compileParam[i] << " ";
    }
    cout << endl;
#endif

    // compile system dictionary files into binary type
    int compileResult = mecab_dict_index(compileParam.size(), &compileParam[0]);
    if(compileResult != 0)
    {
        cerr << "fail to compile system ditionary" << endl;
        return 0;
    }

    // copy configure and definition files to the destination directory
    size_t configNum = sizeof(DICT_CONFIG_FILES) / sizeof(DICT_CONFIG_FILES[0]);
    string src, dest;
    for(size_t i=0; i<configNum; ++i)
    {
        src = createFilePath(txtDirPath, DICT_CONFIG_FILES[i]);
        dest = createFilePath(binDirPath, DICT_CONFIG_FILES[i]);

        if(copyFile(src.c_str(), dest.c_str()) == false)
        {
            return 0;
        }
    }

    // if pos-id.def exists, copy it to the destination directory
    src = createFilePath(txtDirPath, POS_ID_DEF_FILE);
    dest = createFilePath(binDirPath, POS_ID_DEF_FILE);
    if(copyFile(src.c_str(), dest.c_str()) == false)
    {
        cout << POS_ID_DEF_FILE << " is not found in " << txtDirPath << ", default POS index would be 1." << endl;
    }

    return 1;
}

MeCab::Tagger* JMA_Knowledge::getTagger() const
{
    return tagger_;
}

bool JMA_Knowledge::isStopWord(const string& word) const
{
	return stopWords_.find(word) != stopWords_.end();
}

bool JMA_Knowledge::isOutputFullPOS() const
{
    return isOutputFullPOS_;
}

int JMA_Knowledge::getPOSCatNum() const
{
    return posCatNum_;
}

bool JMA_Knowledge::createTempFile(std::string& tempName)
{
#if defined(_WIN32) && !defined(__CYGWIN__)
    // directory name for temporary file
    char dirName[MAX_PATH];

    // retrieve the directory path designated for temporary files
    DWORD dirResult = GetTempPath(MAX_PATH, dirName);
    if(dirResult == 0)
    {
        cerr << "fail in GetTempPath() to get the directory path for temporary file" << endl;
        return false;
    }

    // file name for temporary file
    char fileName[MAX_PATH];

    // create a unique temporary file
    UINT nameResult = GetTempFileName(dirName, "jma", 0, fileName);
    if(nameResult == 0)
    {
        cerr << "fail in GetTempFileName() to create a temporary file" << endl;
        return false;
    }

    tempName = fileName;
#else
    // file name for temporary file
    char fileName[] = "/tmp/jmaXXXXXX";

    // create a unique temporary file
    int tempFd = mkstemp(fileName);
    if(tempFd == -1)
    {
        cerr << "fail in mkstemp() to create a temporary file" << endl;
        return false;
    }

    tempName = fileName;
#endif

    return true;
}

bool JMA_Knowledge::isDirExist(const char* dirPath)
{
    if(dirPath == 0)
    {
        return false;
    }

    bool result = false;

#if defined(_WIN32) && !defined(__CYGWIN__)
    WIN32_FIND_DATA wfd;
    HANDLE hFind;

    // the parameter string in function "FindFirstFile()" would be invalid if it ends with a trailing backslash (\) or slash (/),
    // so the trailing backslash or slash is removed if it exists
    string dirStr(dirPath);
    size_t len = dirStr.size();
    if(len > 0 && (dirPath[len-1] == '\\'  || dirPath[len-1] == '/' ))
    {
        dirStr.erase(len-1, 1);
    }

    hFind = FindFirstFile(dirStr.c_str(), &wfd);
    if(hFind != INVALID_HANDLE_VALUE)
    {
        result = true;
    }
    FindClose(hFind);
#else
    DIR *dir = opendir(dirPath);
    if(dir)
    {
        result = true;
    }
    closedir(dir);
#endif

    return result;
}

bool JMA_Knowledge::copyFile(const char* src, const char* dest)
{
    assert(src && dest);

    // open files
    ifstream from(src);
    if(! from)
    {
        cerr << "cannot open source file: " << src << endl;
        return false;
    }

    ofstream to(dest);
    if(! to)
    {
        cerr << "cannot open destinatioin file: " << dest << endl;
        return false;
    }

    // copy characters
    char ch;
    while(from.get(ch))
    {
        to.put(ch);
    }

    // check file state
    if(!from.eof() || !to)
    {
        cerr << "invalid file state after copy from " << src << " to " << dest << endl;
        return false;
    }

    return true;
}

std::string JMA_Knowledge::createFilePath(const char* dir, const char* file)
{
    assert(file && "the file name is assumed as non empty");

    string result = dir;

#if defined(_WIN32) && !defined(__CYGWIN__)
    if(result.size() && result[result.size()-1] != '\\')
    {
        result += '\\';
    }
#else
    if(result.size() && result[result.size()-1] != '/')
    {
        result += '/';
    }
#endif

    result += file;
    return result;
}

JMA_CType* JMA_Knowledge::getCType()
{
	return ctype_;
}

void JMA_Knowledge::onEncodeTypeChange(EncodeType type)
{
	delete ctype_;
	ctype_ = JMA_CType::instance(type);
}

bool JMA_Knowledge::isSentenceSeparator(const char* p)
{
	unsigned int bytes = ctype_->getByteCount(p);
	const unsigned char* uc = (const unsigned char*)p;

	switch(bytes)
	{
		case 1:
			return seps_[1].contains( uc[0] );
		case 2:
			return seps_[2].contains( uc[0] << 8 | uc[1]);
		case 3:
			return seps_[3].contains( uc[0] << 16 | uc[1] << 8 | uc[2] );
		case 4:
			return seps_[4].contains( uc[0] << 24 | uc[1] << 16 | uc[2] << 8 | uc[3] );
		default:
			assert(false && "Cannot handle Character's length > 4");
	}

	return false;
}

bool JMA_Knowledge::addSentenceSeparator(unsigned int val)
{
	return seps_[getOccupiedBytes(val)].insert(val);
}

unsigned int JMA_Knowledge::getOccupiedBytes(unsigned int val)
{
	unsigned int ret = 1;
	while( val & 0xffffff00 )
	{
		val >>= 4;
		++ ret;
	}
	assert( ret > 0 && ret < 4 );
	return ret;
}

int JMA_Knowledge::loadSentenceSeparatorConfig(const char* fileName)
{
	ifstream in(fileName);
    if(!in)
    {
        cerr << "cannot open file: " << fileName << endl;
        return 0;
    }

	string line;
	while(!in.eof())
	{
		 getline(in, line);
		//ignore the empty line and comment line(start with '#')
		if( line.empty() || line[0] == '#' )
			continue;
		unsigned int bytes = ctype_->getByteCount( line.c_str() );

		const unsigned char* uc = (const unsigned char*)line.c_str();

		switch(bytes)
		{
			case 1:
				seps_[1].insert( uc[0] );
				break;
			case 2:
				seps_[2].insert( uc[0] << 8 | uc[1]);
				break;
			case 3:
				seps_[3].insert( uc[0] << 16 | uc[1] << 8 | uc[2] );
				break;
			case 4:
				seps_[4].insert( uc[0] << 24 | uc[1] << 16 | uc[2] << 8 | uc[3] );
				break;
			default:
				assert(false && "Cannot handle 'Character's length > 4'");
				break;
		}
	}
	in.close();
	return 1;
}

} // namespace jma
