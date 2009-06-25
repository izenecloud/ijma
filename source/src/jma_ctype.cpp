/** \file cma_ctype.cpp
 * \brief CMA_CType gives the character type information.
 * \author Jun Jiang
 * \version 0.1
 * \date Mar 10, 2009
 */

#include "jma_ctype.h"
#include "jma_ctype_eucjp.h"

#include <cassert>

using namespace std;

namespace jma
{

JMA_CType* JMA_CType::instance(Knowledge::EncodeType type)
{
    switch(type)
    {
	case Knowledge::ENCODE_TYPE_EUCJP:
	    return JMA_CType_EUCJP::instance();

	case Knowledge::ENCODE_TYPE_SJIS:
	    return 0;

	default:
	    assert(false && "unkown character encode type");
	    return 0;
    }
}

Knowledge::EncodeType JMA_CType::getEncType(string encType)
{
    if(encType == "EUC-JP" || encType == "enc-jp")
    {
        return Knowledge::ENCODE_TYPE_EUCJP;
    }
    else if(encType == "SHIFT-JIS" || encType == "shift-jis")
    {
        return Knowledge::ENCODE_TYPE_SJIS;
    }

    assert(false && "unkown character encode type");
    return Knowledge::ENCODE_TYPE_NUM;
}

JMA_CType::~JMA_CType()
{
}

size_t JMA_CType::length(const char* p) const
{
    size_t ret = 0;
    while(p)
    {
        unsigned int len = getByteCount(p);
        //len can be 0
        if(!len)
            return ret;
        p += len;
        ++ret;
    }
    return ret;
}

bool JMA_CType::isSentenceSeparator(const char* p)
{
	return false;
}

} // namespace cma
