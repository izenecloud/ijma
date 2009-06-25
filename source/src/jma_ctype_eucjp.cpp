/**
 * \file jma_ctype_eucjp.h
 * \brief encoding for the EUC-JP
 *
 * \date Jun 25, 2009
 * \author vernkin
 */

#include "jma_ctype_eucjp.h"

#include <cassert>

namespace jma
{

JMA_CType_EUCJP* JMA_CType_EUCJP::instance()
{
	return new JMA_CType_EUCJP;
}

JMA_CType_EUCJP::JMA_CType_EUCJP()
{
	// TODO Auto-generated constructor stub

}

JMA_CType_EUCJP::~JMA_CType_EUCJP()
{
	// TODO Auto-generated destructor stub
}

unsigned int JMA_CType_EUCJP::getByteCount(const char* p) const
{
	assert(p);
	const unsigned char* uc = (const unsigned char*)p;

	if(uc[0] == 0)
		return 0;

	if(uc[0] < 0x80)
		return 1; // encoding in ASCII

	assert(uc[1] && uc[1] >= 0x80);
	return 2; // encoding in EUC-JP
}

}
