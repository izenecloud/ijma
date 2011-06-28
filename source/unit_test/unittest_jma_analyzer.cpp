/** \file unittest_jma_analyzer.cpp
 * Unit test of class JMA_Analyzer.
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Sep 14, 2010
 */

#include <gtest/gtest.h>
#include <jma_analyzer.h>
#include <jma_knowledge.h>
#include "../../test_src/test_jma_common.h"
#include "../../include/file_utils.h"

#include <vector>
#include <string>
#include <iostream>
#include <fstream>

using namespace jma;
using namespace std;

class JMA_AnalyzerTest : public ::testing::Test
{
protected:
    virtual void SetUp() {
        analyzer_ = new JMA_Analyzer;
        ASSERT_TRUE(analyzer_ != NULL);

        knowledge_ = new JMA_Knowledge;
        ASSERT_TRUE(knowledge_);

        knowledge_->setSystemDict(TEST_JMA_DEFAULT_SYSTEM_DICT);
        knowledge_->addUserDict(TEST_JMA_DEFAULT_USER_DICT);
        ASSERT_EQ(1, knowledge_->loadDict());
        EXPECT_EQ(Knowledge::ENCODE_TYPE_UTF8, knowledge_->getEncodeType());

        ASSERT_EQ(1, analyzer_->setKnowledge(knowledge_));
    }

    virtual void TearDown() {
        delete analyzer_;
        delete knowledge_;
    }

    virtual void runWithSentenceTest(Analyzer* pAnalyzer) {
        Sentence sent;
        ASSERT_EQ(1, pAnalyzer->runWithSentence(sent));
        EXPECT_EQ(0, sent.getListSize());
        EXPECT_STREQ("", sent.getString());

        sent.setString("abc");
        ASSERT_EQ(1, pAnalyzer->runWithSentence(sent));
        ASSERT_EQ(1, sent.getListSize());
        EXPECT_EQ(1, sent.getCount(0));
        EXPECT_STREQ("abc", sent.getLexicon(0, 0));
        EXPECT_EQ(45, sent.getPOS(0, 0));
        EXPECT_STREQ("名詞,固有名詞,組織", sent.getStrPOS(0, 0));
        EXPECT_STREQ("abc", sent.getBaseForm(0, 0));
        EXPECT_STREQ("", sent.getReadForm(0, 0));
        EXPECT_STREQ("abc", sent.getNormForm(0, 0));
        EXPECT_EQ(1, sent.getScore(0));
        EXPECT_EQ(0, sent.getOneBestIndex());

        sent.setString("ソニー");
        ASSERT_EQ(1, pAnalyzer->runWithSentence(sent));
        ASSERT_EQ(1, sent.getListSize());
        EXPECT_EQ(1, sent.getCount(0));
        EXPECT_STREQ("ソニー", sent.getLexicon(0, 0));
        EXPECT_EQ(69, sent.getPOS(0, 0));
        EXPECT_STREQ("名詞,ユーザ", sent.getStrPOS(0, 0));
        EXPECT_STREQ("ソニー", sent.getBaseForm(0, 0));
        EXPECT_STREQ("", sent.getReadForm(0, 0));
        EXPECT_STREQ("ソニー", sent.getNormForm(0, 0));
        EXPECT_EQ(1, sent.getScore(0));
        EXPECT_EQ(0, sent.getOneBestIndex());

        sent.setString("来た");
        ASSERT_EQ(1, pAnalyzer->runWithSentence(sent));
        ASSERT_EQ(1, sent.getListSize());
        EXPECT_EQ(2, sent.getCount(0));
        EXPECT_STREQ("来", sent.getLexicon(0, 0));
        EXPECT_STREQ("た", sent.getLexicon(0, 1));
        EXPECT_EQ(31, sent.getPOS(0, 0));
        EXPECT_EQ(25, sent.getPOS(0, 1));
        EXPECT_STREQ("動詞,自立", sent.getStrPOS(0, 0));
        EXPECT_STREQ("助動詞", sent.getStrPOS(0, 1));
        EXPECT_STREQ("来る", sent.getBaseForm(0, 0));
        EXPECT_STREQ("た", sent.getBaseForm(0, 1));
        EXPECT_STREQ("き", sent.getReadForm(0, 0));
        EXPECT_STREQ("た", sent.getReadForm(0, 1));
        EXPECT_STREQ("来", sent.getNormForm(0, 0));
        EXPECT_STREQ("た", sent.getNormForm(0, 1));
        EXPECT_EQ(1, sent.getScore(0));
        EXPECT_EQ(0, sent.getOneBestIndex());
    }

    virtual void runWithStreamTest(const char* strInputFile, const char* strOutputFile)  {
        ofstream ofs(strInputFile);
        ASSERT_TRUE(ofs);
        ofs << "田中さんは三菱東京UFJ銀行に行った。" << endl;
        ofs << "どういう意味でしょうか？" << endl;
        ofs.close();

        ASSERT_EQ(1, analyzer_->runWithStream(strInputFile, strOutputFile));
    }

    JMA_Analyzer* analyzer_;
    JMA_Knowledge* knowledge_;
};

TEST_F(JMA_AnalyzerTest, setKnowledgeFail) {
    JMA_Knowledge* knowledge = new JMA_Knowledge;
    ASSERT_EQ(0, analyzer_->setKnowledge(knowledge));
    delete knowledge;
}

TEST_F(JMA_AnalyzerTest, runWithString) {
    EXPECT_STREQ("123/名詞,数  ", analyzer_->runWithString("123"));
    EXPECT_STREQ("本田総一郎/名詞,ユーザ  ", analyzer_->runWithString("本田総一郎"));
    EXPECT_STREQ("", analyzer_->runWithString(""));
}

TEST_F(JMA_AnalyzerTest, runWithSentence) {
    runWithSentenceTest(analyzer_);
}

TEST_F(JMA_AnalyzerTest, loadMultipleKnowledge) {
    runWithSentenceTest(analyzer_);

    string strSysDict(TEST_JMA_DEFAULT_SYSTEM_DICT);
    strSysDict += "/../bin_eucjp";
    knowledge_->setSystemDict(strSysDict.c_str());
    EXPECT_EQ(1, knowledge_->loadDict());
    EXPECT_EQ(Knowledge::ENCODE_TYPE_EUCJP, knowledge_->getEncodeType());
    EXPECT_EQ(1, analyzer_->setKnowledge(knowledge_));

    knowledge_->setSystemDict(TEST_JMA_DEFAULT_SYSTEM_DICT);
    EXPECT_EQ(1, knowledge_->loadDict());
    EXPECT_EQ(Knowledge::ENCODE_TYPE_UTF8, knowledge_->getEncodeType());
    EXPECT_EQ(1, analyzer_->setKnowledge(knowledge_));

    runWithSentenceTest(analyzer_);
}

TEST_F(JMA_AnalyzerTest, multipleAnalyzer) {
    runWithSentenceTest(analyzer_);

    JMA_Analyzer* analyzer1 = new JMA_Analyzer;
    ASSERT_TRUE(analyzer1 != NULL);
    EXPECT_EQ(1, analyzer1->setKnowledge(knowledge_));
    runWithSentenceTest(analyzer1);

    JMA_Analyzer* analyzer2 = new JMA_Analyzer;
    ASSERT_TRUE(analyzer2 != NULL);
    EXPECT_EQ(1, analyzer2->setKnowledge(knowledge_));
    runWithSentenceTest(analyzer2);

    delete analyzer1;
    delete analyzer2;
}

TEST_F(JMA_AnalyzerTest, runWithStreamFail) {
    EXPECT_EQ(0, analyzer_->runWithStream("aaa", "bbb"));
    EXPECT_EQ(0, analyzer_->runWithStream("", "ccc"));
}

TEST_F(JMA_AnalyzerTest, runWithStream) {
    const char* strInputFile = "aaa.txt";
    const char* strOutputFile = "bbb.txt";

    runWithStreamTest(strInputFile, strOutputFile);

    ifstream ifs(strOutputFile);
    ASSERT_TRUE(ifs);
    string line;
    ASSERT_TRUE(getline(ifs, line));
    EXPECT_EQ("田中/名詞,固有名詞,人名,姓  さん/名詞,接尾,人名  は/助詞,係助詞  三菱東京UFJ銀行/名詞,ユーザ  に/助詞,格助詞,一般  行っ/動詞,自立  た/助動詞  。/記号,句点  ", line);
    ASSERT_TRUE(getline(ifs, line));
    EXPECT_EQ("どういう/連体詞  意味/名詞,サ変接続  でしょ/助動詞  う/助動詞  か/助詞,副助詞／並立助詞／終助詞  ？/記号,一般  ", line);

    EXPECT_TRUE(removeFile(strInputFile));
    EXPECT_TRUE(removeFile(strOutputFile));
}

TEST_F(JMA_AnalyzerTest, splitSentence) {
    vector<Sentence> sentVec;
    analyzer_->splitSentence("", sentVec);
    EXPECT_EQ(0u, sentVec.size());

    const char* paraStr = "aaaaa,bbbbb.ccccc!ddddd?ここ、そこ。こち、そち！これ、それ？";
    analyzer_->splitSentence(paraStr, sentVec);
    ASSERT_EQ(5u, sentVec.size());
    EXPECT_STREQ("aaaaa,bbbbb.ccccc!", sentVec[0].getString());
    EXPECT_STREQ("ddddd?", sentVec[1].getString());
    EXPECT_STREQ("ここ、そこ。", sentVec[2].getString());
    EXPECT_STREQ("こち、そち！", sentVec[3].getString());
    EXPECT_STREQ("これ、それ？", sentVec[4].getString());
}

TEST_F(JMA_AnalyzerTest, convertCharacters) {
    const char* str = "azAZ10ａｚＡＺ１０あんアンｲﾑ阿";
    EXPECT_EQ(str, analyzer_->convertCharacters(str));

    analyzer_->setOption(Analyzer::OPTION_TYPE_CONVERT_TO_HIRAGANA, 1);
    analyzer_->setOption(Analyzer::OPTION_TYPE_CONVERT_TO_HALF_WIDTH, 1);
    analyzer_->setOption(Analyzer::OPTION_TYPE_CONVERT_TO_LOWER_CASE, 1);
    EXPECT_EQ("azaz10azaz10あんあんｲﾑ阿", analyzer_->convertCharacters(str));

    analyzer_->setOption(Analyzer::OPTION_TYPE_CONVERT_TO_KATAKANA, 1);
    analyzer_->setOption(Analyzer::OPTION_TYPE_CONVERT_TO_FULL_WIDTH, 1);
    analyzer_->setOption(Analyzer::OPTION_TYPE_CONVERT_TO_UPPER_CASE, 1);
    EXPECT_EQ("ＡＺＡＺ１０ＡＺＡＺ１０アンアンｲﾑ阿", analyzer_->convertCharacters(str));
}

TEST_F(JMA_AnalyzerTest, nbest) {
    analyzer_->setOption(Analyzer::OPTION_TYPE_NBEST, 5);
    EXPECT_EQ(5, analyzer_->getOption(Analyzer::OPTION_TYPE_NBEST));

    Sentence sent;
    ASSERT_EQ(1, analyzer_->runWithSentence(sent));
    EXPECT_EQ(0, sent.getListSize());
    EXPECT_STREQ("", sent.getString());

    sent.setString("abc");
    ASSERT_EQ(1, analyzer_->runWithSentence(sent));
    ASSERT_EQ(5, sent.getListSize());
}

TEST_F(JMA_AnalyzerTest, posFormatNone) {
    analyzer_->setOption(Analyzer::OPTION_TYPE_POS_TAGGING, 0);
    EXPECT_STREQ("123  ", analyzer_->runWithString("123"));

    runWithSentenceTest(analyzer_);

    const char* strInputFile = "aaa.txt";
    const char* strOutputFile = "bbb.txt";

    runWithStreamTest(strInputFile, strOutputFile);

    ifstream ifs(strOutputFile);
    ASSERT_TRUE(ifs);
    string line;
    ASSERT_TRUE(getline(ifs, line));
    EXPECT_EQ("田中  さん  は  三菱東京UFJ銀行  に  行っ  た  。  ", line);
    ASSERT_TRUE(getline(ifs, line));
    EXPECT_EQ("どういう  意味  でしょ  う  か  ？  ", line);

    EXPECT_TRUE(removeFile(strInputFile));
    EXPECT_TRUE(removeFile(strOutputFile));
}

TEST_F(JMA_AnalyzerTest, posFormatAlphabet) {
    analyzer_->setOption(Analyzer::OPTION_TYPE_POS_FORMAT_ALPHABET, 1);
    EXPECT_STREQ("123/NN  ", analyzer_->runWithString("123"));

    Sentence sent("abc");
    ASSERT_EQ(1, analyzer_->runWithSentence(sent));
    ASSERT_EQ(1, sent.getListSize());
    EXPECT_EQ(1, sent.getCount(0));
    EXPECT_STREQ("abc", sent.getLexicon(0, 0));
    EXPECT_EQ(45, sent.getPOS(0, 0));
    EXPECT_STREQ("NP-O", sent.getStrPOS(0, 0));

    const char* strInputFile = "aaa.txt";
    const char* strOutputFile = "bbb.txt";

    runWithStreamTest(strInputFile, strOutputFile);

    ifstream ifs(strOutputFile);
    ASSERT_TRUE(ifs);
    string line;
    ASSERT_TRUE(getline(ifs, line));
    EXPECT_EQ("田中/NP-S  さん/NS-H  は/PL-A  三菱東京UFJ銀行/N-USER  に/PL-G  行っ/V-I  た/AUV  。/EOS  ", line);
    ASSERT_TRUE(getline(ifs, line));
    EXPECT_EQ("どういう/AA  意味/N-VS  でしょ/AUV  う/AUV  か/PL-K  ？/S-G  ", line);

    EXPECT_TRUE(removeFile(strInputFile));
    EXPECT_TRUE(removeFile(strOutputFile));
}

TEST_F(JMA_AnalyzerTest, posFormatFull) {
    analyzer_->setOption(Analyzer::OPTION_TYPE_POS_FULL_CATEGORY, 1);
    EXPECT_STREQ("123/名詞,数,*,*  ", analyzer_->runWithString("123"));

    Sentence sent("abc");
    ASSERT_EQ(1, analyzer_->runWithSentence(sent));
    ASSERT_EQ(1, sent.getListSize());
    EXPECT_EQ(1, sent.getCount(0));
    EXPECT_STREQ("abc", sent.getLexicon(0, 0));
    EXPECT_EQ(45, sent.getPOS(0, 0));
    EXPECT_STREQ("名詞,固有名詞,組織,*", sent.getStrPOS(0, 0));

    const char* strInputFile = "aaa.txt";
    const char* strOutputFile = "bbb.txt";

    runWithStreamTest(strInputFile, strOutputFile);

    ifstream ifs(strOutputFile);
    ASSERT_TRUE(ifs);
    string line;
    ASSERT_TRUE(getline(ifs, line));
    EXPECT_EQ("田中/名詞,固有名詞,人名,姓  さん/名詞,接尾,人名,*  は/助詞,係助詞,*,*  三菱東京UFJ銀行/名詞,ユーザ,*,*  に/助詞,格助詞,一般,*  行っ/動詞,自立,*,*  た/助動詞,*,*,*  。/記号,句点,*,*  ", line);
    ASSERT_TRUE(getline(ifs, line));
    EXPECT_EQ("どういう/連体詞,*,*,*  意味/名詞,サ変接続,*,*  でしょ/助動詞,*,*,*  う/助動詞,*,*,*  か/助詞,副助詞／並立助詞／終助詞,*,*  ？/記号,一般,*,*  ", line);

    EXPECT_TRUE(removeFile(strInputFile));
    EXPECT_TRUE(removeFile(strOutputFile));
}

TEST_F(JMA_AnalyzerTest, compoundNotCombine) {
    const char* str = "高さ";
    EXPECT_STREQ("高さ/名詞,一般  ", analyzer_->runWithString(str));
    analyzer_->setOption(Analyzer::OPTION_TYPE_COMPOUND_MORPHOLOGY, 0);
    EXPECT_STREQ("高/形容詞,自立  さ/名詞,接尾,特殊  ", analyzer_->runWithString(str));
}

TEST_F(JMA_AnalyzerTest, userNounDecompose) {
    const char* str = "野球選手権大会";
    EXPECT_STREQ("野球選手権大会/名詞,ユーザ  ", analyzer_->runWithString(str));
    analyzer_->setOption(Analyzer::OPTION_TYPE_DECOMPOSE_USER_NOUN, 1);
    EXPECT_STREQ("野球/名詞,ユーザ  選手権/名詞,ユーザ  大会/名詞,ユーザ  ", analyzer_->runWithString(str));
}
