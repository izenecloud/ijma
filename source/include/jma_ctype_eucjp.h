/**
 * \file jma_ctype_eucjp.h
 * \brief encoding for the EUC-JP
 *
 * \date Jun 25, 2009
 * \author vernkin
 */

#ifndef JMA_CTYPE_EUCJP_H_
#define JMA_CTYPE_EUCJP_H_

#include "jma_ctype.h"

namespace jma {

class JMA_CType_EUCJP: public JMA_CType {

public:

	/**
	 * Create an instance of JMA_CType_EUCJP
	 * \return the pointer to instance (invoke new here)
	 */
	static JMA_CType_EUCJP* instance();

	virtual ~JMA_CType_EUCJP();

    /**
     * Get the byte count of the first character pointed by \e p, which
     * character is in a specific encoding.
     * \param p pointer to the character string
     * \return true for punctuation, false for non punctuation.
     */
    virtual unsigned int getByteCount(const char* p) const;

private:
	JMA_CType_EUCJP();
};

}

#endif /* JMA_CTYPE_EUCJP_H_ */
