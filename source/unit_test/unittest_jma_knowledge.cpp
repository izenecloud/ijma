/** \file unittest_jma_knowledge.cpp
 * Unit test of class JMA_Knowledge.
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Sep 13, 2010
 */

#include <gtest/gtest.h>
#include <jma_knowledge.h>
#include <mecab.h> // MeCab::Tagger
#include "../../test_src/test_jma_common.h"
#include <jma_dictionary.h>
#include <mempool.h>
#include <mmap.h>

#include <vector>
#include <string>
#include <iostream>

using namespace jma;
using namespace std;
using namespace MeCab;

class JMA_Knowledge_Test : public ::testing::Test
{
protected:
    virtual void SetUp() {
        knowledge_ = new JMA_Knowledge;
        ASSERT_TRUE(knowledge_);
    }

    virtual void TearDown() {
        delete knowledge_;
    }

    virtual void loadDictTest() {
        knowledge_->setSystemDict(TEST_JMA_DEFAULT_SYSTEM_DICT);
        knowledge_->addUserDict(TEST_JMA_DEFAULT_USER_DICT);
        EXPECT_EQ(1, knowledge_->loadDict());

        EXPECT_EQ(Knowledge::ENCODE_TYPE_UTF8, knowledge_->getEncodeType());
        MeCab::Tagger* tagger = knowledge_->createTagger();
        EXPECT_TRUE(tagger != NULL);
        delete tagger;
        EXPECT_TRUE(knowledge_->isSentenceSeparator("!"));
        EXPECT_TRUE(knowledge_->getCType());
        EXPECT_EQ(6, knowledge_->getBaseFormOffset());
        EXPECT_EQ(7, knowledge_->getReadFormOffset());
        EXPECT_EQ(9, knowledge_->getNormFormOffset());
        EXPECT_EQ(69, knowledge_->getUserNounPOSIndex());

        const POSTable& posTable = knowledge_->getPOSTable();
        EXPECT_STREQ("O", posTable.getPOS(0, POSTable::POS_FORMAT_ALPHABET));

        const CharTable& kanaTable = knowledge_->getKanaTable();
        EXPECT_STREQ("ア", kanaTable.toRight("あ"));

        const CharTable& widthTable = knowledge_->getWidthTable();
        EXPECT_STREQ("6", widthTable.toLeft("６"));

        const CharTable& caseTable = knowledge_->getCaseTable();
        EXPECT_STREQ("A", caseTable.toRight("a"));

        const JMA_Knowledge::DecompMap& decompMap = knowledge_->getDecompMap();
        EXPECT_EQ(3u, decompMap.size());
    }

    virtual void printDict() {
        JMA_Dictionary::instance()->debugPrint();
        JMA_UserDictionary::instance()->debugPrint();
        getMemoryPool<std::string, Mmap<char> >().debugPrint();
        getMemoryPool<std::string, Mmap<short> >().debugPrint();
    }

    JMA_Knowledge* knowledge_;
};

TEST_F(JMA_Knowledge_Test, getValueDefault) {
    EXPECT_FALSE(knowledge_->isStopWord("aaa"));
    EXPECT_TRUE(knowledge_->isKeywordPOS(0));
    EXPECT_TRUE(knowledge_->createTagger() == NULL);
    EXPECT_FALSE(knowledge_->isSentenceSeparator("!"));
    EXPECT_TRUE(knowledge_->getCType() == NULL);
    EXPECT_EQ(0, knowledge_->getBaseFormOffset());
    EXPECT_EQ(0, knowledge_->getReadFormOffset());
    EXPECT_EQ(0, knowledge_->getNormFormOffset());
    EXPECT_EQ(-1, knowledge_->getUserNounPOSIndex());

    const POSTable& posTable = knowledge_->getPOSTable();
    EXPECT_STREQ("", posTable.getPOS(0));

    const CharTable& kanaTable = knowledge_->getKanaTable();
    EXPECT_STREQ(NULL, kanaTable.toRight("bbb"));

    const CharTable& widthTable = knowledge_->getWidthTable();
    EXPECT_STREQ(NULL, widthTable.toLeft("ccc"));

    const CharTable& caseTable = knowledge_->getCaseTable();
    EXPECT_STREQ(NULL, caseTable.toRight("ddd"));

    const JMA_Knowledge::DecompMap& decompMap = knowledge_->getDecompMap();
    EXPECT_EQ(0u, decompMap.size());
}

TEST_F(JMA_Knowledge_Test, loadFail) {
    EXPECT_EQ(0, knowledge_->loadDict());
    EXPECT_EQ(0, knowledge_->loadStopWordDict("aaa"));

    vector<string> posVec;
    posVec.push_back("bbb");
    EXPECT_EQ(0, knowledge_->setKeywordPOS(posVec));

    EXPECT_EQ(0, knowledge_->encodeSystemDict("ccc", "ddd", Knowledge::ENCODE_TYPE_UTF8));
    EXPECT_EQ(0, knowledge_->encodeSystemDict(TEST_JMA_DEFAULT_SYSTEM_DICT_SOURCE, "eee", Knowledge::ENCODE_TYPE_UTF8));
}

TEST_F(JMA_Knowledge_Test, loadDict) {
    loadDictTest();
}

TEST_F(JMA_Knowledge_Test, loadDictMultiTimes) {
    string strSysDict(TEST_JMA_DEFAULT_SYSTEM_DICT);
    strSysDict += "/../bin_eucjp";
    knowledge_->setSystemDict(strSysDict.c_str());
    knowledge_->addUserDict(TEST_JMA_DEFAULT_USER_DICT);
    EXPECT_EQ(1, knowledge_->loadDict());
    EXPECT_EQ(Knowledge::ENCODE_TYPE_EUCJP, knowledge_->getEncodeType());
    MeCab::Tagger* tagger = knowledge_->createTagger();
    EXPECT_TRUE(tagger != NULL);
    delete tagger;
    EXPECT_TRUE(knowledge_->getCType());

    loadDictTest();
}

TEST_F(JMA_Knowledge_Test, loadStopWordDict) {
    EXPECT_EQ(1, knowledge_->loadStopWordDict(TEST_JMA_DEFAULT_STOPWORD_DICT));
    EXPECT_TRUE(knowledge_->isStopWord("が"));
    EXPECT_FALSE(knowledge_->isStopWord("A"));
}

TEST_F(JMA_Knowledge_Test, setKeywordPOS) {
    knowledge_->setSystemDict(TEST_JMA_DEFAULT_SYSTEM_DICT);
    EXPECT_EQ(1, knowledge_->loadDict());

    vector<string> posVec;
    posVec.push_back("AUV");
    EXPECT_EQ(1, knowledge_->setKeywordPOS(posVec));
    EXPECT_TRUE(knowledge_->isKeywordPOS(25));
    EXPECT_FALSE(knowledge_->isKeywordPOS(33));
}
