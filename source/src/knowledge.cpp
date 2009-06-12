/** \file knowledge.cpp
 * Implementation of class Knowledge.
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Jun 12, 2009
 */

#include "knowledge.h"

#include <cassert>

namespace jma
{

Knowledge::Knowledge()
    : encodeType_(ENCODE_TYPE_EUCJP)
{
}

Knowledge::~Knowledge()
{
}

void Knowledge::setEncodeType(EncodeType type)
{
    encodeType_ = type;
}

Knowledge::EncodeType Knowledge::getEncodeType() const
{
    return encodeType_;
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

} // namespace jma
