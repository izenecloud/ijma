/**
 * \file jma_ctype_sjis.cpp
 * \brief encoding for the SHIFT-JIS
 *
 * \date Jun 25, 2009
 * \author vernkin
 */

#include "jma_ctype_sjis.h"

#include <cassert>

namespace jma
{

JMA_CType_SJIS* JMA_CType_SJIS::instance()
{
	return new JMA_CType_SJIS;
}

JMA_CType_SJIS::JMA_CType_SJIS()
{
	// TODO Auto-generated constructor stub

}

JMA_CType_SJIS::~JMA_CType_SJIS()
{
	// TODO Auto-generated destructor stub
}

unsigned int JMA_CType_SJIS::getByteCount(const char* p) const
{
	assert(p);
	const unsigned char* uc = (const unsigned char*)p;

	if(uc[0] == 0)
		return 0;

	if(uc[0] < 0x80)
		return 1; // encoding in ASCII

	assert(uc[1]);
	return 2; // encoding in SHIFT-JIS
}

bool JMA_CType_SJIS::isSpace(const char* p) const
{
    assert(p);

    const unsigned char* uc = (const unsigned char*)p;

    if(uc[0] < 0x80)
        return isspace(uc[0]); // check by std library

    //full-width space in SHIFT-JIS
    if(uc[0] == 0x81 && uc[1] == 0x40)
        return true;

    return false;
}

string JMA_CType_SJIS::replaceSpaces(const char* str, char replacement)
{
	unsigned char replace = (unsigned char)replacement;
	const unsigned char* uc = (const unsigned char*)str;
	string ret(str);
	unsigned char* c2 = (unsigned char*)ret.data();

    const unsigned int length = ret.size();
    for(unsigned int i=0; i<length;)
	{
		if(uc[i] < 0x80)
		{
			if(isspace(uc[i]))
				c2[i] = replace;
			++i;
		}
		else
		{
			//full-width space in SHIFT-JIS
			if(uc[i] == 0x81 && uc[i+1] == 0x40)
			{
				c2[i] = c2[i+1] = replace;
			}
			i += 2;
		}
	}

	return ret;
}

}
