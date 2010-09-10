/** \file unittest_knowledge.cpp
 * Unit test of class Knowledge.
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Sep 10, 2010
 */

#include <gtest/gtest.h>
#include <knowledge.h>
#include <jma_knowledge.h>

using namespace jma;

TEST(Knowledge, getEncodeTypeDefault) {
    Knowledge* knowledge = new JMA_Knowledge;

    ASSERT_TRUE(knowledge);

    EXPECT_EQ(Knowledge::ENCODE_TYPE_NUM, knowledge->getEncodeType()) << "default encode type should be invalid type";

    delete knowledge;
}

TEST(Knowledge, decodeEncodeType) {
    EXPECT_EQ(Knowledge::ENCODE_TYPE_UTF8, Knowledge::decodeEncodeType("UTF-8"));
    EXPECT_EQ(Knowledge::ENCODE_TYPE_UTF8, Knowledge::decodeEncodeType("UTF8"));
    EXPECT_EQ(Knowledge::ENCODE_TYPE_UTF8, Knowledge::decodeEncodeType("utf-8"));
    EXPECT_EQ(Knowledge::ENCODE_TYPE_UTF8, Knowledge::decodeEncodeType("utf8"));
    EXPECT_EQ(Knowledge::ENCODE_TYPE_EUCJP, Knowledge::decodeEncodeType("EUCJP"));
    EXPECT_EQ(Knowledge::ENCODE_TYPE_EUCJP, Knowledge::decodeEncodeType("EUC-JP"));
    EXPECT_EQ(Knowledge::ENCODE_TYPE_EUCJP, Knowledge::decodeEncodeType("eucjp"));
    EXPECT_EQ(Knowledge::ENCODE_TYPE_EUCJP, Knowledge::decodeEncodeType("euc-jp"));
    EXPECT_EQ(Knowledge::ENCODE_TYPE_SJIS, Knowledge::decodeEncodeType("SJIS"));
    EXPECT_EQ(Knowledge::ENCODE_TYPE_SJIS, Knowledge::decodeEncodeType("SHIFT-JIS"));
    EXPECT_EQ(Knowledge::ENCODE_TYPE_SJIS, Knowledge::decodeEncodeType("sjis"));
    EXPECT_EQ(Knowledge::ENCODE_TYPE_SJIS, Knowledge::decodeEncodeType("shift-jis"));

    EXPECT_EQ(Knowledge::ENCODE_TYPE_NUM, Knowledge::decodeEncodeType(""));
    EXPECT_EQ(Knowledge::ENCODE_TYPE_NUM, Knowledge::decodeEncodeType("GB2312"));
    EXPECT_EQ(Knowledge::ENCODE_TYPE_NUM, Knowledge::decodeEncodeType("EUCKR"));
}

TEST(Knowledge, encodeStr) {
    EXPECT_STREQ("UTF-8", Knowledge::encodeStr(Knowledge::ENCODE_TYPE_UTF8));
    EXPECT_STREQ("EUC-JP", Knowledge::encodeStr(Knowledge::ENCODE_TYPE_EUCJP));
    EXPECT_STREQ("SHIFT-JIS", Knowledge::encodeStr(Knowledge::ENCODE_TYPE_SJIS));

    EXPECT_STREQ(NULL, Knowledge::encodeStr(Knowledge::ENCODE_TYPE_NUM));
}
