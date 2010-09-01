/** \file test_jma_common.h
 * The header file containing those macro definitions commonly used in test source files.
 *
 * \author Jun Jiang
 * \version 0.1
 * \date Sep 29, 2009
 */

#ifndef TEST_JMA_COMMON_H
#define TEST_JMA_COMMON_H

#if defined(_WIN32) && !defined(__CYGWIN__)
#define TEST_JMA_DEFAULT_SYSTEM_DICT "../../db/ipadic/bin_eucjp"

#define TEST_JMA_DEFAULT_USER_DICT "../../db/userdic/user_noun.eucjp"

#define TEST_JMA_DEFAULT_STOPWORD_DICT "../../db/stopworddic/test-eucjp.txt"
#else
/** default path of system dictionary directory */
#define TEST_JMA_DEFAULT_SYSTEM_DICT "../db/ipadic/bin_eucjp"

/** default path of user dictionary file */
#define TEST_JMA_DEFAULT_USER_DICT "../db/userdic/user_noun.eucjp"

/** default path of stop word dictionary file */
#define TEST_JMA_DEFAULT_STOPWORD_DICT "../db/stopworddic/test-eucjp.txt"
#endif

#endif // TEST_JMA_COMMON_H
