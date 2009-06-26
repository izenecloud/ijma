/** \file cma_ctype.cpp
 * \brief CMA_CType gives the character type information.
 * \author Jun Jiang
 * \version 0.1
 * \date Mar 10, 2009
 */

#include "jma_ctype.h"
#include "jma_ctype_eucjp.h"
#include "jma_ctype_sjis.h"

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
	    return JMA_CType_SJIS::instance();

	default:
	    assert(false && "Unknown character encode type");
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

    assert(false && "Unknown character encode type");
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
	unsigned int bytes = getByteCount(p);

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

bool JMA_CType::addSentenceSeparator(unsigned int val)
{
	return seps_[getOccupiedBytes(val)].insert(val);
}

unsigned int JMA_CType::getOccupiedBytes(unsigned int val)
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

} // namespace jma
