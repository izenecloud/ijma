/** \file jma_knowledge.cpp
 * Implementation of class JMA_Knowledge.
 *
 * \author Jun Jiang
 * \version 0.1
 * \date Jun 17, 2009
 */

#include "jma_knowledge.h"
#include "sentence.h"
#include "jma_dictionary.h"
#include "tokenizer.h"

#include "mecab.h" // MeCab::Tagger
#include "param.h" // MeCab::Param
#include "iconv_utils.h" // MeCab::Iconv

#include <iostream>
#include <fstream> // ifstream, ofstream
#include <sstream>
#include <cstdlib> // mkstemp, atoi
#include <strstream> // istrstream
#include <cassert>

#if defined(_WIN32) && !defined(__CYGWIN__)
#include <windows.h> // GetTempPath, GetTempFileName, FindFirstFile, FindClose
#else
#include <dirent.h> // opendir, closedir
#include <unistd.h> // unlink
#endif

using namespace std;

namespace
{
/** Default value of base form feature offset */
const int BASE_FORM_OFFSET_DEFAULT = 6;

/** Default value of read form feature offset */
const int READ_FORM_OFFSET_DEFAULT = 7;

/** Default value of norm form feature offset */
const int NORM_FORM_OFFSET_DEFAULT = 9;

/** default pos of user defined noun if not set in "dicrc" */
const char* USER_NOUN_POS_DEFAULT = "N-USER";

/** Dictionary definition files used to compile dictionary */
const char* DICT_DEF_FILES[] = {"rewrite.def", "left-id.def", "right-id.def"};

/** Dictionary binary files */
const char* DICT_BINARY_FILES[] = {"unk.dic", "char.bin", "sys.dic", "matrix.bin"};

/** Dictionary configure file, used in both source and binary dictionaries */
const char* DICT_CONFIG_FILE = "dicrc";

/** POS index definition file name */
const char* POS_ID_DEF_FILE = "pos-id.def";

/** POS combination rule file name */
const char* POS_COMBINE_DEF_FILE = "compound.def";

/** Map file name to convert between Hiragana and Katakana characters */
const char* KANA_MAP_DEF_FILE = "map-kana.def";

/** Map file name to convert between half and full width characters */
const char* WIDTH_MAP_DEF_FILE = "map-width.def";

/** Map file name to convert between lower and upper case characters */
const char* CASE_MAP_DEF_FILE = "map-case.def";

/** System dictionary archive */
const char* DICT_ARCHIVE_FILE = "sys.bin";

/** Binary user dictionary in memory */
const char* BIN_USER_DICT_MEMORY_FILE = "user.bin";

/** Text user dictionary in memory */
const char* TXT_USER_DICT_MEMORY_FILE = "user.csv";

/** default character encode type of dictionary config files */
jma::Knowledge::EncodeType DEFAULT_CONFIG_ENCODE_TYPE = jma::Knowledge::ENCODE_TYPE_EUCJP;

/** the cost value of user defined noun, the smaller the value, the more likely user defined nouns are recognized */
const int USER_NOUN_COST = 0;

/**
 * Convert string to the value of type \e Target.
 * \param str source string
 * \return target value of type \e Target
 * \attention if the convertion failed, the default value of type \e Target would be returned instead.
 */
template<class Target> Target convertFromStr(const string& str)
{
    stringstream convertor;
    Target result;

    if(!(convertor << str) || !(convertor >> result) || !(convertor >> ws).eof())
    {
        cerr << "error in converting value from " << str << ", use default value instead." << endl;
        return Target();
    }

    return result;
}

/**
 * Tokenize CSV string to each components.
 * \param str the string in CSV format, such as "1,2,3"
 * \param compVec the vector to store each tokenized components, the original contents are removed, such as "1", "2", "3"
 * \return the number of components
 */
unsigned int tokenizeCSV(const string& str, vector<string>& compVec)
{
    compVec.clear();

    const char* delimit = ",";
    string::size_type i, j;
    i = 0;
    while(i < str.size())
    {
        j = str.find_first_of(delimit, i);
        if(j != string::npos)
        {
            compVec.push_back(str.substr(i, j-i));
            i = j + 1;
        }
        else
        {
            compVec.push_back(str.substr(i));
            i = j;
        }
    }

#if JMA_DEBUG_PRINT
    cout << "tokenize CSV from string (" << str << ") to ";
    for(unsigned int i=0; i<compVec.size(); ++i)
    {
        cout << compVec[i] << ", ";
    }
    cout << endl;
#endif

    return compVec.size();
}

/**
 * Check whether the string represents a decimal number.
 * \param str the string to check
 * \return true for number, false for not number
 */
bool isNumber(const char* str)
{
    const char* p = str;
    while(*p)
    {
        if(*p < '0' || *p > '9')
            return false;

        ++p;
    }

    // in case of empty string
    if(p == str)
        return false;

    return true;
}
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
    : isOutputFullPOS_(false), baseFormOffset_(0), readFormOffset_(0), normFormOffset_(0),
    ctype_(0), configEncodeType_(DEFAULT_CONFIG_ENCODE_TYPE),
    dictionary_(JMA_Dictionary::instance())
{
}

JMA_Knowledge::~JMA_Knowledge()
{
    dictionary_->close();
    delete ctype_;
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
            cout << userDictNames_[i].first << " ";
        }
        cout << endl;
    }
#endif

    if(userDictNum == 0)
    {
        return false;
    }

    // remove existing decompostion map
    decompMap_.clear();

    // just create an empty instance, it would be generated in mecab compiling
    binUserDic_ = BIN_USER_DICT_MEMORY_FILE;
    dictionary_->createEmptyBinaryUserDict(binUserDic_.c_str());

    // create text user dictionary
    const char* textUserDicName = TXT_USER_DICT_MEMORY_FILE;
    dictionary_->createEmptyTextUserDict(textUserDicName);

    ostringstream osst;
    // append source files of user dictionary
    unsigned int entryCount = 0;
    for(size_t i=0; i<userDictNum; ++i)
        entryCount += convertTxtToCSV(userDictNames_[i], osst);

#if JMA_DEBUG_PRINT
    cout << entryCount << " entries in user dictionaries altogether." << endl;
#endif
    if(entryCount == 0)
    {
        cerr << "fail to compile the empty user dictionary." << endl;
        return false;
    }

#if JMA_DEBUG_PRINT
    cout << "user defined nouns are converted as:" << endl;
    cout << osst.str() << endl;
#endif
    if(! dictionary_->copyStrToDict(osst.str(), textUserDicName))
    {
        cerr << "fail to copy text user dictionary from stream to memory." << endl;
        return false;
    }

    // construct parameter to compile user dictionary
    vector<char*> compileParam;
    compileParam.push_back((char*)"JMA_Knowledge");
    compileParam.push_back((char*)"-d");
    compileParam.push_back(const_cast<char*>(systemDictPath_.c_str()));
    compileParam.push_back((char*)"-u");
    compileParam.push_back(const_cast<char*>(binUserDic_.c_str()));

    // the encoding type of text user dictionary,
    // as they have been converted to destination encoding in convertTxtToCSV(),
    // use destination encoding here
    compileParam.push_back((char*)"-f");
    compileParam.push_back(const_cast<char*>(Knowledge::encodeStr(getEncodeType())));

    // the encoding type of binary user dictionary
    compileParam.push_back((char*)"-t");
    compileParam.push_back(const_cast<char*>(Knowledge::encodeStr(getEncodeType())));

    compileParam.push_back(const_cast<char*>(textUserDicName));

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

    return true;
}

MeCab::Tagger* JMA_Knowledge::createTagger() const
{
    // construct parameter to create tagger
    string taggerParam("-d ");
    taggerParam += systemDictPath_;

    if(hasUserDict())
    {
        assert(! binUserDic_.empty() && "the binary user dictionary in memory should have been created by JMA_Knowledge::compileUserDict()");

        // append the name of user dictionary binary file to the parameter of tagger creation
        taggerParam += " -u ";
        taggerParam += binUserDic_;
    }

#if JMA_DEBUG_PRINT
    cout << "parameter of MeCab::createTagger() to create MeCab::Tagger: " << taggerParam << endl << endl;
#endif

    // create tagger by loading dictioanry files
    return MeCab::createTagger(taggerParam.c_str());
}

const POSTable& JMA_Knowledge::getPOSTable() const
{
    return posTable_;
}

const CharTable& JMA_Knowledge::getKanaTable() const
{
    return kanaTable_;
}

const CharTable& JMA_Knowledge::getWidthTable() const
{
    return widthTable_;
}

const CharTable& JMA_Knowledge::getCaseTable() const
{
    return caseTable_;
}

const JMA_Knowledge::DecompMap& JMA_Knowledge::getDecompMap() const
{
    return decompMap_;
}

void JMA_Knowledge::loadDictConfig()
{
    const char* configFile = DICT_CONFIG_FILE;
    map<string, string> configMap;

    bool isLoadConfig = false;
    const DictUnit* dict = dictionary_->getDict(configFile);
    if(dict)
    {
        istrstream ist(dict->text_, dict->length_);
        if(ist)
        {
            isLoadConfig = true;
            string line, left, middle, right;
            istringstream iss;
            while(getline(ist, line))
            {
                // remove carriage return character
                line = line.substr(0, line.find('\r'));

                if (line.empty() || line[0] == ';' || line[0] == '#')
                    continue;

                iss.clear();
                iss.str(line);
                if(iss >> left >> middle >> right && middle == "=")
                    configMap[left] = right;
                else
                    cerr << "format error in configuration file: " << configFile << ", ingoring line: " << line << endl;
            }
        }
    }

    if(! isLoadConfig)
    {
        cerr << "cannot load configuration file: " << configFile << endl;
        cerr << "default configuration value is used." << endl;
    }

    string* value = getMapValue(configMap, "base-form-feature-offset");
    baseFormOffset_ = value ? convertFromStr<int>(*value) : BASE_FORM_OFFSET_DEFAULT;

    value = getMapValue(configMap, "read-form-feature-offset");
    readFormOffset_ = value ? convertFromStr<int>(*value) : READ_FORM_OFFSET_DEFAULT;

    value = getMapValue(configMap, "norm-form-feature-offset");
    normFormOffset_ = value ? convertFromStr<int>(*value) : NORM_FORM_OFFSET_DEFAULT;

    value = getMapValue(configMap, "user-noun-pos");
    userNounPOS_ = value ? *value : USER_NOUN_POS_DEFAULT;

    value = getMapValue(configMap, "config-charset");
    configEncodeType_ = Knowledge::ENCODE_TYPE_NUM; // reset
    if(value)
        configEncodeType_ = Knowledge::decodeEncodeType(value->c_str());

    if(configEncodeType_ == Knowledge::ENCODE_TYPE_NUM)
    {
        configEncodeType_ = DEFAULT_CONFIG_ENCODE_TYPE;
        cerr << "unknown dictionary config charset, use default charset " << DEFAULT_CONFIG_ENCODE_TYPE << endl;
    }

    value = getMapValue(configMap, "binary-charset");
    EncodeType type = Knowledge::ENCODE_TYPE_NUM; // reset
    if(value)
        type = Knowledge::decodeEncodeType(value->c_str());

    if(type == Knowledge::ENCODE_TYPE_NUM)
    {
        type = DEFAULT_CONFIG_ENCODE_TYPE;
        cerr << "unknown dictionary config charset, use default charset " << DEFAULT_CONFIG_ENCODE_TYPE << endl;
    }
    // set binary encode type
    if(encodeType_ != type)
    {
        encodeType_ = type;
        delete ctype_;
        ctype_ = JMA_CType::instance(encodeType_);
    }

#if JMA_DEBUG_PRINT
    cout << "JMA_Knowledge::loadDictConfig() loads:" << endl;
    cout << "configFile: " << configFile << endl;
    cout << "base form feature offset: " << baseFormOffset_ << endl;
    cout << "read form feature offset: " << readFormOffset_ << endl;
    cout << "norm form feature offset: " << normFormOffset_ << endl;
    cout << "POS of user defined noun: " << userNounPOS_ << endl;
    cout << "config charset: " << Knowledge::encodeStr(configEncodeType_) << endl;
    cout << "binary charset: " << Knowledge::encodeStr(encodeType_) << endl;
#endif
}

int JMA_Knowledge::loadDict()
{
    // file "sys.bin"
    string sysDicName = createFilePath(systemDictPath_.c_str(), DICT_ARCHIVE_FILE);
    if(! dictionary_->open(sysDicName.c_str()))
    {
        cerr << "fail to open system dictionary: " << sysDicName << endl;
        return 0;
    }

    // file "dicrc"
    loadDictConfig();

    // file "pos-id.def"
    const char* posFileName = POS_ID_DEF_FILE;
    // load POS table
    if(! posTable_.loadConfig(posFileName, configEncodeType_, getEncodeType()))
    {
        cerr << "fail in POSTable::loadConfig() to load " << posFileName << endl;
        return 0;
    }

    // file "compound.def"
    string posCombineName = createFilePath(systemDictPath_.c_str(), POS_COMBINE_DEF_FILE);
    // load POS combine rules
    if(! posTable_.loadCombineRule(posCombineName.c_str()))
    {
        cerr << "warning: as " << posCombineName << " not exists, no rules is defined to combine tokens with specific POS tags" << endl;
    }

    // file "map-kana.def"
    const char* kanaFileName = KANA_MAP_DEF_FILE;
    // load kana conversion map
    if(! kanaTable_.loadConfig(kanaFileName, configEncodeType_, getEncodeType()))
    {
        cerr << "warning: as fails to load " << kanaFileName << ", no mapping is defined to convert between Hiragana and Katakana characters." << endl;
    }

    // file "map-width.def"
    const char* widthFileName = WIDTH_MAP_DEF_FILE;
    // load width conversion map
    if(! widthTable_.loadConfig(widthFileName, configEncodeType_, getEncodeType()))
    {
        cerr << "warning: as fails to load " << widthFileName << ", no mapping is defined to convert between half and full width characters." << endl;
    }

    // file "map-case.def"
    const char* caseFileName = CASE_MAP_DEF_FILE;
    // load case conversion map
    if(! caseTable_.loadConfig(caseFileName, configEncodeType_, getEncodeType()))
    {
        cerr << "warning: as fails to load " << caseFileName << ", no mapping is defined to convert between lower and upper case characters." << endl;
    }

#if JMA_DEBUG_PRINT
    cout << "JMA_Knowledge::loadDict()" << endl;
    cout << "system dictionary path: " << systemDictPath_ << endl << endl;
#endif

    if(hasUserDict())
    {
        if(! compileUserDict())
        {
            cerr << "fail to compile user dictionary in JMA_Knowledge::loadDict()" << endl;
            return 0;
        }
    }

    // load into temporary instance to check the result
    MeCab::Tagger* tagger = createTagger();
    if(! tagger)
    {
        cerr << "fail to create MeCab::Tagger in JMA_Knowledge::loadDict()" << endl;
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

    // remove existing stop words
    stopWords_.clear();

    string line;
    while(getline(in, line))
    {
        line = line.substr(0, line.find('\r'));
        if(line.empty())
            continue;

        stopWords_.insert(line);
    }

    return 1;
}

int JMA_Knowledge::encodeSystemDict(const char* txtDirPath, const char* binDirPath, EncodeType binEncodeType)
{
    assert(txtDirPath && binDirPath);

#if JMA_DEBUG_PRINT
    cout << "JMA_Knowledge::encodeSystemDict()" << endl;
    cout << "path of source system dictionary: " << txtDirPath << endl;
    cout << "path of binary system directory: " << binDirPath << endl;
    cout << "encoding type of binary system dictioanry: " << Knowledge::encodeStr(binEncodeType) << endl;
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
    // below is to set the destination encoding type
    compileParam.push_back((char*)"-t");
    compileParam.push_back(const_cast<char*>(Knowledge::encodeStr(binEncodeType)));

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

    string src, dest;
    // if compound.def exists, copy it to the destination directory
    src = createFilePath(txtDirPath, POS_COMBINE_DEF_FILE);
    dest = createFilePath(binDirPath, POS_COMBINE_DEF_FILE);
    if(copyFile(src.c_str(), dest.c_str()) == false)
    {
        cout << POS_COMBINE_DEF_FILE << " is not found in " << txtDirPath << ", no rule is defined to combine tokens with specific POS." << endl;
    }

    // source files to compile into archive
    vector<string> srcFiles;

    // dicrc
    src = createFilePath(txtDirPath, DICT_CONFIG_FILE);
    dest = createFilePath(binDirPath, DICT_CONFIG_FILE);
    // fill "binary-charset" in dicrc
    if(! fillBinaryEncodeType(src.c_str(), dest.c_str(), binEncodeType))
    {
        cerr << "fail to fill binary-charset from " << src << " to " << dest << endl;
    }
    srcFiles.push_back(dest);

    // definition files
    size_t configNum = sizeof(DICT_DEF_FILES) / sizeof(DICT_DEF_FILES[0]);
    for(size_t i=0; i<configNum; ++i)
    {
        srcFiles.push_back(createFilePath(txtDirPath, DICT_DEF_FILES[i]));
    }

    // pos-id.def
    srcFiles.push_back(createFilePath(txtDirPath, POS_ID_DEF_FILE));

    // map-kana.def
    srcFiles.push_back(createFilePath(txtDirPath, KANA_MAP_DEF_FILE));

    // map-width.def
    srcFiles.push_back(createFilePath(txtDirPath, WIDTH_MAP_DEF_FILE));

    // map-case.def
    srcFiles.push_back(createFilePath(txtDirPath, CASE_MAP_DEF_FILE));

    // get binary file names
    configNum = sizeof(DICT_BINARY_FILES) / sizeof(DICT_BINARY_FILES[0]);
    for(size_t i=0; i<configNum; ++i)
    {
        srcFiles.push_back(createFilePath(binDirPath, DICT_BINARY_FILES[i]));
    }

    // compile into archive file
    dest = createFilePath(binDirPath, DICT_ARCHIVE_FILE);
    cout << "compressing into archive file " << dest << endl;
    if(JMA_Dictionary::compile(srcFiles, dest.c_str()) == false)
        return 0;

    // remove dicrc under binary path
    dest = createFilePath(binDirPath, DICT_CONFIG_FILE);
    if(! removeFile(dest))
        cerr << "fail to delete temporary configuration file: " << dest << endl;

    // remove temporary binary files
    configNum = sizeof(DICT_BINARY_FILES) / sizeof(DICT_BINARY_FILES[0]);
    for(size_t i=0; i<configNum; ++i)
    {
        dest = createFilePath(binDirPath, DICT_BINARY_FILES[i]);
        if(! removeFile(dest))
            cerr << "fail to delete temporary binary file: " << dest << endl;
    }

    return 1;
}

bool JMA_Knowledge::isStopWord(const std::string& word) const
{
    return stopWords_.find(word) != stopWords_.end() || ctype_->isSpace(word.c_str());
}

bool JMA_Knowledge::isSentenceSeparator(const char* p) const
{
    return sentSeps_.find(p) != sentSeps_.end();
}

bool JMA_Knowledge::isKeywordPOS(int pos) const
{
    if(keywordPOSSet_.empty())
        return true;

    return keywordPOSSet_.find(pos) != keywordPOSSet_.end();
}

int JMA_Knowledge::getBaseFormOffset() const
{
    return baseFormOffset_;
}

int JMA_Knowledge::getReadFormOffset() const
{
    return readFormOffset_;
}

int JMA_Knowledge::getNormFormOffset() const
{
    return normFormOffset_;
}

int JMA_Knowledge::getUserNounPOSIndex() const
{
    return posTable_.getIndexFromAlphaPOS(userNounPOS_);
}

//bool JMA_Knowledge::createTempFile(std::string& tempName)
//{
//#if defined(_WIN32) && !defined(__CYGWIN__)
    //// directory name for temporary file
    //char dirName[MAX_PATH];

    //// retrieve the directory path designated for temporary files
    //DWORD dirResult = GetTempPath(MAX_PATH, dirName);
    //if(dirResult == 0)
    //{
        //cerr << "fail in GetTempPath() to get the directory path for temporary file" << endl;
        //return false;
    //}

    //// file name for temporary file
    //char fileName[MAX_PATH];

    //// create a unique temporary file
    //UINT nameResult = GetTempFileName(dirName, "jma", 0, fileName);
    //if(nameResult == 0)
    //{
        //cerr << "fail in GetTempFileName() to create a temporary file" << endl;
        //return false;
    //}

    //tempName = fileName;
//#else
    //// file name for temporary file
    //char fileName[] = "/tmp/jmaXXXXXX";

    //// create a unique temporary file
    //int tempFd = mkstemp(fileName);
    //if(tempFd == -1)
    //{
        //cerr << "fail in mkstemp() to create a temporary file" << endl;
        //return false;
    //}

    //tempName = fileName;
//#endif

    //return true;
//}

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
    ifstream from(src, ios::binary);
    if(! from)
    {
        cerr << "cannot open source file: " << src << endl;
        return false;
    }

    ofstream to(dest, ios::binary);
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

int JMA_Knowledge::loadSentenceSeparatorConfig(const char* fileName)
{
    ifstream in(fileName);
    if(!in)
    {
        cerr << "cannot open file: " << fileName << endl;
        return 0;
    }

    // remove existing separators
    sentSeps_.clear();

    string line;
    while(getline(in, line))
    {
        line = line.substr(0, line.find('\r'));
        if(line.empty() || line[0] == '#')
            continue;

        sentSeps_.insert(line);
    }

    return 1;
}

unsigned int JMA_Knowledge::convertTxtToCSV(const UserDictFileType& userDicFile, std::ostream& ost)
{
    unsigned int count = 0;

    const int userNounIndex = getUserNounPOSIndex();
    if(userNounIndex == -1)
    {
        cerr << "fail to get POS index of user noun." << endl;
        return count;
    }

    const char* userNounPOS = posTable_.getPOS(userNounIndex, POSTable::POS_FORMAT_FULL_CATEGORY);
    if(! userNounPOS)
    {
        cerr << "fail to get POS string of user noun." << endl;
        return count;
    }

    vector<string> t;
    const int posSize = tokenizeCSV(userNounPOS, t);
    assert(posSize > 0 && "the user noun POS size must be positive");

    // tokenize word into each characters
    CTypeTokenizer tokenizer(getCType());

    const char* srcEnc = Knowledge::encodeStr(userDicFile.second);
    const char* destEnc = Knowledge::encodeStr(getEncodeType());
    MeCab::Iconv iconv;
    if(! iconv.open(srcEnc, destEnc))
    {
        cerr << "error to open encoding conversion from " << srcEnc << " to " << destEnc << endl;
        return false;
    }

    const char* fileName = userDicFile.first.c_str();
    ifstream in(fileName);
    if( !in )
    {
        cerr << "fail to open user dictionary " << fileName << ", ignoring this file." << endl;
        return count;
    }

#if JMA_DEBUG_PRINT
    cout << "Converting user dictionary " << fileName << " (" << srcEnc << " => " << destEnc << ") ..." << endl;
#endif

    string line, word, pattern;
    istringstream iss;
    ostringstream ostrs;
    while(getline(in, line))
    {
        // remove carriage return character
        line = line.substr(0, line.find('\r'));

        if (line.empty() || line[0] == ';' || line[0] == '#')
            continue;

        if(! iconv.convert(&line))
        {
            cerr << "error to convert encoding from " << srcEnc << " to " << destEnc << " for line: " << line << endl;
            continue;
        }
#if JMA_DEBUG_PRINT
        cout << "line: " << line << endl;
#endif

        iss.clear();
        iss.str(line);
        ostrs.str(""); // reset output string stream to empty

        if(iss >> word)
            ostrs << word << ",-1,-1," << USER_NOUN_COST;
        else
        {
            cerr << "no word is defined in line: " << line << endl;
            continue;
        }

        ostrs << "," << userNounPOS;
        for(int i=posSize; i<readFormOffset_; ++i)
        {
            ostrs << ",*";
        }

        bool hasRead = false;
        vector<string> compVec;
        if(iss >> pattern)
        {
            if(tokenizeCSV(pattern, compVec) == 0)
            {
                cerr << "fail to tokenize from pattern: " << pattern << endl;
                continue;
            }

            if(isNumber(compVec[0].c_str()))
            {
                MorphemeList decompList;
                bool isAllNumber = true;
                bool isValidCharCount = true;
                tokenizer.assign(word.c_str());
                for(unsigned int i=0; i<compVec.size(); ++i)
                {
                    if(! isNumber(compVec[i].c_str()))
                    {
                        isAllNumber = false;
                        break;
                    }

                    Morpheme morp;
                    int charCount = convertFromStr<int>(compVec[i]);
                    for(int j=0; j<charCount; ++j)
                    {
                        if(const char* p = tokenizer.next())
                            morp.lexicon_ += p;
                        else
                        {
                            isValidCharCount = false;
                            break;
                        }
                    }

                    if(! isValidCharCount)
                        break;

                    decompList.push_back(morp);
                }

                if(! isAllNumber)
                {
                    cerr << "invalid format, only digit is allowed in decomposition pattern: " << pattern << endl;
                    continue;
                }

                // should reach the word end
                if(! isValidCharCount || tokenizer.next())
                {
                    cerr << "unmatched character numbers in line: " << line << endl;
                    continue;
                }

                if(iss >> pattern)
                {
                    if(tokenizeCSV(pattern, compVec) == 0)
                    {
                        cerr << "fail to tokenize from pattern: " << pattern << endl;
                        continue;
                    }

                    if(compVec.size() != decompList.size())
                    {
                        cerr << "invalid format, the pronunciation pattern size is not equal to decomposition pattern size in line: " << line << endl;
                        continue;
                    }

                    for(unsigned int i=0; i<compVec.size(); ++i)
                    {
                        decompList[i].readForm_ = compVec[i];
                    }

                    hasRead = true;
                }

                decompMap_[word] = decompList;
#if JMA_DEBUG_PRINT
                cout << "word " << word << " is decomposed into: ";
                for(unsigned int i=0; i<decompList.size(); ++i)
                {
                    cout << decompList[i].lexicon_;
                    if(! decompList[i].readForm_.empty())
                        cout << "/" << decompList[i].readForm_;
                    cout << ", ";
                }
                cout << endl;
#endif
            }
            else
                hasRead = true;
        }

        if(hasRead)
        {
            assert(compVec.size() && "the read form should not be empty.");

            // combine compVec into whole read form
            string wholeRead;
            for(unsigned int i=0; i<compVec.size(); ++i)
            {
                wholeRead += compVec[i];
            }
            ostrs << "," << wholeRead;
        }
        else
            ostrs << ",*";

        ost << ostrs.str() << endl;
        ++count;
    }

    return count;
}

bool JMA_Knowledge::fillBinaryEncodeType(const char* src, const char* dest, EncodeType binEncodeType) const
{
    ifstream ist(src);
    if(! ist)
    {
        cerr << "cannot read configuration file: " << src << endl;
        return false;
    }

    ofstream ost(dest);
    if(! ost)
    {
        cerr << "cannot write configuration file: " << dest << endl;
        return false;
    }

    string line, left, middle;
    istringstream iss;
    bool fill = false;
    while(getline(ist, line))
    {
        // remove carriage return character
        line = line.substr(0, line.find('\r'));

        if(line.empty() || line[0] == ';' || line[0] == '#')
            ost << line << endl;
        else
        {
            iss.clear();
            iss.str(line);
            if(iss >> left >> middle && left == "binary-charset" && middle == "=")
            {
                ost << left << " " << middle << " " << Knowledge::encodeStr(binEncodeType) << endl;
                fill = true;
#if JMA_DEBUG_PRINT
                cout << "JMA_Knowledge::fillBinaryEncodeType(), src: " << src << ", dest: " << dest << endl;
                cout << left << " " << middle << " " << Knowledge::encodeStr(binEncodeType) << endl;
                cout << endl;
#endif
            }
            else
                ost << line << endl;
        }
    }

    if(fill)
        return true;
    else
    {
        cerr << "the field binary-charset is not found in configuration file: " << src << endl;
        return false;
    }
}

} // namespace jma
