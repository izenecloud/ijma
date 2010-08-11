/** \file knowledge.cpp
 * Implementation of class Knowledge.
 *
 * \author Jun Jiang
 * \version 0.1
 * \date Jun 12, 2009
 */

#include "knowledge.h"

#include <string>
#include <cassert>

using namespace std;

namespace jma
{

const char* Knowledge::ENCODE_TYPE_STR_[] = {"EUC-JP", "SHIFT-JIS", "UTF-8"};

namespace
{
/**
 * Get a string in lower alphabets.
 * \param s the original string
 * \return the converted string with lower alphabets. For example, "euc-jp" is returned when "EUC-JP" is input.
 */
string toLower(const char* s) {
    string str(s);
    for(size_t i=0; i<str.size(); ++i)
    {
        char c = str[i];
        if((c >= 'A') && (c <= 'Z'))
        {
            c += 'a' - 'A';
            str[i] = c;
        }
    }
    return str;
}
} // namespace

Knowledge::Knowledge()
    : encodeType_(ENCODE_TYPE_EUCJP)
{
}

Knowledge::~Knowledge()
{
}

void Knowledge::setEncodeType(EncodeType type)
{
    if( encodeType_ != type )
    {
		encodeType_ = type;
        onEncodeTypeChange( type );
    }
}

Knowledge::EncodeType Knowledge::getEncodeType() const
{
    return encodeType_;
}

Knowledge::EncodeType Knowledge::decodeEncodeType(const char* encodeStr)
{
    string lower = toLower(encodeStr);
    if(lower == "euc-jp" || lower == "eucjp")
    {
        return Knowledge::ENCODE_TYPE_EUCJP;
    }
    else if(lower == "shift-jis" || lower == "sjis")
    {
        return Knowledge::ENCODE_TYPE_SJIS;
    }
    else if(lower == "utf-8" || lower == "utf8")
    {
        return Knowledge::ENCODE_TYPE_UTF8;
    }

    // unknown character encoding type
    return Knowledge::ENCODE_TYPE_NUM;
}

const char* Knowledge::encodeStr(EncodeType encodeType)
{
    if(encodeType < ENCODE_TYPE_NUM)
        return ENCODE_TYPE_STR_[encodeType];

    // unknown encoding type
    return 0;
}

void Knowledge::setSystemDict(const char* dirPath)
{
    assert(dirPath);

    systemDictPath_ = dirPath;
}

void Knowledge::addUserDict(const char* fileName)
{
    assert(fileName);

    userDictNames_.push_back(fileName);
}

void Knowledge::setKeywordPOS(const std::vector<int>& posVec)
{
    keywordPOSSet_.clear();
    for(std::vector<int>::const_iterator it=posVec.begin(); it!=posVec.end(); ++it)
    {
        keywordPOSSet_.insert(*it);
    }
}

} // namespace jma
