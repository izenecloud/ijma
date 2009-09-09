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
#include "param.h" // MeCab::Param

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

/** Default value of base form feature offset */
const int BASE_FORM_OFFSET_DEFAULT = 6;

/** Default tokens size in a feature */
const size_t FEATURE_TOKEN_SIZE_DEFAULT = 12;

/** default pos in the user dictionary if not set or set to wrong (not exists) */
const string DEFAULT_POS_DEFAULT("n");

/** Dictionary configure and definition files */
const char* DICT_CONFIG_FILES[] = {"dicrc", "rewrite.def", "left-id.def",
		"right-id.def", "pos-feature.def"};

/** POS index definition file name */
const char* POS_ID_DEF_FILE = "pos-id.def";

/** POS feature mapping file name */
const char* POS_FEATURE_DEF_FILE = "pos-feature.def";
}

namespace jma
{

inline string* getMapValue(map<string, string>& map, const string& key)
{
	std::map<string, string>::iterator itr = map.find( key );
	if( itr == map.end() )
		return 0;
	return &itr->second;
}

JMA_Knowledge::JMA_Knowledge()
    : isOutputFullPOS_(false), posCatNum_(0), baseFormOffset_(0),
    defaultFeature_(0), ctype_(0)
{
    onEncodeTypeChange( getEncodeType() );
}

JMA_Knowledge::~JMA_Knowledge()
{
    // remove the temporary user file in binary format
    if(hasUserDict())
    {
        if(! removeFile(tempUserDic_))
        {
            cerr << "fail to delete tempUserDic_ file: " << tempUserDic_ << endl;
        }
    }

    if(ctype_)
    {
        delete ctype_;
    }
}

bool JMA_Knowledge::hasUserDict() const
{
    return ! userDictNames_.empty();
}

bool JMA_Knowledge::compileUserDict()
{
    const size_t userDictNum = userDictNames_.size();

#if JMA_DEBUG_PRINT
    cout << "JMA_Knowledge::compileUserDict()" << endl;
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

    if(userDictNum == 0)
    {
        return false;
    }

    // remove the temporary user binary file if exists
    if(! tempUserDic_.empty())
    {
        if(! removeFile(tempUserDic_))
        {
            cerr << "fail to delete tempUserDic_ file: " << tempUserDic_ << endl;
        }
    }

    // create a unique temporary file for user dictionary in binary type
    bool tempResult = createTempFile(tempUserDic_);
    if(! tempResult)
    {
        cerr << "fail to create a temporary user dictionary binary file" << endl;
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

    // temporary csv file for user dictionary
    string tempUserCSVFile;

    // convert user dictionaries to a csv file
    if(! createTempFile(tempUserCSVFile))
    {
        cerr << "fail to create a temporary user dictionary csv file" << endl;
        return false;
    }
#if JMA_DEBUG_PRINT
    cout << "temporary file name of user csv file: " << tempUserCSVFile << endl;
#endif

    ofstream ostream(tempUserCSVFile.c_str());

    // append source files of user dictionary
    for(size_t i=0; i<userDictNum; ++i)
        convertTxtToCSV(userDictNames_[i].c_str(), ostream);

    compileParam.push_back(const_cast<char*>(tempUserCSVFile.c_str()));

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
        return false;
    }

    // as the binary user dictionary is created,
    // remove the temporary user csv file
    if(! removeFile(tempUserCSVFile))
    {
        cerr << "fail to delete tempUserCSVFile file: " << tempUserCSVFile << endl;
    }

    return true;
}

MeCab::Tagger* JMA_Knowledge::createTagger() const
{
    // construct parameter to create tagger
    string taggerParam("-d ");
    taggerParam += systemDictPath_;

    if(hasUserDict())
    {
        assert(! tempUserDic_.empty() && "the temporary file of user dictionary in binary format should have been created by JMA_Knowledge::compileUserDict()");

        // append the name of user dictionary binary file to the parameter of tagger creation
        taggerParam += " -u ";
        taggerParam += tempUserDic_;
    }

#if JMA_DEBUG_PRINT
    cout << "parameter of MeCab::createTagger() to create MeCab::Tagger: " << taggerParam << endl << endl;
#endif

    // create tagger by loading dictioanry files
    return MeCab::createTagger(taggerParam.c_str());
}

bool JMA_Knowledge::loadPOSDef()
{
    // get POS category number from "pos-id.def"
    string posFileName = createFilePath(systemDictPath_.c_str(), POS_ID_DEF_FILE);
    ifstream posFile(posFileName.c_str());

    if(! posFile)
    {
        return false;
    }

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

    return true;
}

bool JMA_Knowledge::loadConfig0(const char *filename, map<string, string>& map) {
  std::ifstream ifs(filename);
  assert(ifs);

  std::string line;
  while (std::getline(ifs, line)) {
    if (!line.size() ||
        (line.size() && (line[0] == ';' || line[0] == '#'))) continue;

    size_t pos = line.find('=');
    assert(pos != std::string::npos && "format error: ");

    size_t s1, s2;
    for (s1 = pos+1; s1 < line.size() && isspace(line[s1]); s1++);
    for (s2 = pos-1; static_cast<long>(s2) >= 0 && isspace(line[s2]); s2--);
    //const std::string value = line.substr(s1, line.size() - s1);
    //const std::string key   = line.substr(0, s2 + 1);
    map[line.substr(0, s2 + 1)] = line.substr(s1, line.size() - s1);
  }

  return true;
}

bool JMA_Knowledge::loadDictConfig()
{
    string configFile = createFilePath(systemDictPath_.c_str(), DICT_CONFIG_FILES[0]);
    MeCab::Param param;

    if(! param.load(configFile.c_str()))
    {
        return false;
    }

    baseFormOffset_ = param.get<size_t>("base-form-feature-offset");
    featureTokenSize_ = param.get<size_t>("feature-token-size");
    defaultPOS_ = param.get<string>("default-pos");

#if JMA_DEBUG_PRINT
    cout << "base form feature offset is " << baseFormOffset_ << " in " << configFile << endl;
#endif

    return true;
}

bool JMA_Knowledge::loadPOSFeatureMapping()
{
	string configFile = createFilePath(systemDictPath_.c_str(), DICT_CONFIG_FILES[4]);
	posFeatureMap_.clear();
	return loadConfig0( configFile.c_str(), posFeatureMap_ );
}

int JMA_Knowledge::loadDict()
{
    if(! loadPOSDef())
    {
        // if no "pos-id.def" exists,
        // the default value of POS category number would be used.
        posCatNum_ = POS_CAT_NUM_DEFAULT;
    }

    if(! loadDictConfig())
    {
        // if no "dicrc" exists,
        // the default value of base form feature offset would be used.
        baseFormOffset_ = BASE_FORM_OFFSET_DEFAULT;
        // also set other default value
        featureTokenSize_ = FEATURE_TOKEN_SIZE_DEFAULT;
        defaultPOS_ = DEFAULT_POS_DEFAULT;
    }

    if(! loadPOSFeatureMapping() )
    	cerr<< "Fail to load POS - Feature Mapping, User dictionary can't work"<<endl;
    else
    	defaultFeature_ = getMapValue(posFeatureMap_, defaultPOS_);

#if JMA_DEBUG_PRINT
    cout << "JMA_Knowledge::loadDict()" << endl;
    cout << "system dictionary path: " << systemDictPath_ << endl;
#endif

    if(hasUserDict())
    {
        if(! compileUserDict())
        {
            return 0;
        }
    }

    // load into temporary instance to check the result
    MeCab::Tagger* tagger = createTagger();
    if(! tagger)
    {
        return 0;
    }

    // destroy the temporary instance
    delete tagger;

    return 1;
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

    // if pos-feature.def exists, copy it to the destination directory
    src = createFilePath(txtDirPath, POS_FEATURE_DEF_FILE);
    dest = createFilePath(binDirPath, POS_FEATURE_DEF_FILE);
    if(copyFile(src.c_str(), dest.c_str()) == false)
    {
        cout << POS_FEATURE_DEF_FILE << " is not found in " << txtDirPath << ", no default feature is defined for those words in user dictionary." << endl;
    }

    return 1;
}

bool JMA_Knowledge::isStopWord(const string& word) const
{
	return stopWords_.find(word) != stopWords_.end() || ctype_->isSpace(word.c_str());
}

bool JMA_Knowledge::isOutputFullPOS() const
{
    return isOutputFullPOS_;
}

int JMA_Knowledge::getPOSCatNum() const
{
    return posCatNum_;
}

int JMA_Knowledge::getBaseFormOffset() const
{
    return baseFormOffset_;
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

bool JMA_Knowledge::removeFile(const std::string& fileName)
{
    if(unlink(fileName.c_str()) == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
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

bool JMA_Knowledge::isDictFeature( const char* str, bool includeWord )
{
	size_t tokenSize = includeWord ? 0 : 1;
	while(*str)
	{
		if(*str == ',')
			++tokenSize;
		++str;
	}
	return tokenSize == featureTokenSize_;
}

void JMA_Knowledge::convertTxtToCSV(const char* userDicFile, ofstream& ostream)
{
	ifstream in(userDicFile);
	if( !in )
	{
		cerr << "Can't Find User Dictionary " << userDicFile << ", and IGNORE this file." << endl;
		return;
	}

	cout<<"Converting User Dictionary " << userDicFile << " ..." << endl;

	string line;
	while( !in.eof() )
	{
		getline(in, line);
		trimSelf(line);
		if(line.empty())
			continue;

		vector<string> vec;
		stringToVector( line, vec );
		if(vec.empty())
			continue;

		size_t vecSize = vec.size();
		string& word = vec[0];
		if( vecSize == 1 )
		{
			if( isDictFeature( word.c_str(), true) )
				ostream << word << endl;
			else if( defaultFeature_ )
				ostream << vec[0] << "," << *defaultFeature_ << endl;
			continue;
		}

		set<string> usedFeatures;
		for( size_t i = 1; i < vecSize; ++i )
		{
			string& posOrFec = vec[i];
			// posOrFec is a feature itself
			if( isDictFeature( posOrFec.c_str() ) )
			{
				//don't insert duplicate entry
				if( usedFeatures.find(posOrFec) == usedFeatures.end() )
				{
					ostream << word << "," << posOrFec << endl;
					usedFeatures.insert(posOrFec);
				}
			}
			else
			{
				string* fec = getMapValue( posFeatureMap_, posOrFec );
				//don't insert duplicate entry
				if( fec )
				{
					if( usedFeatures.find( *fec ) == usedFeatures.end() )
					{
						ostream << word << "," << *fec << endl;
						usedFeatures.insert( *fec );
					}
				}
				else
					cerr << "Can't find POS for " << posOrFec << endl;
			}
		}

		if( usedFeatures.empty() && defaultFeature_ )
		{
			ostream << vec[0] << "," << *defaultFeature_ << endl;
		}
	}

}

} // namespace jma
