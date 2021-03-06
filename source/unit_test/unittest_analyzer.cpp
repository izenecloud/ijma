/** \file unittest_analyzer.cpp
 * Unit test of class Analyzer.
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Sep 10, 2010
 */

#include <gtest/gtest.h>
#include <ijma/analyzer.h>
#include <jma_analyzer.h>

using namespace jma;

class AnalyzerTest : public ::testing::Test
{
protected:
    virtual void SetUp() {
        analyzer_ = new JMA_Analyzer;
        ASSERT_TRUE(analyzer_ != NULL);
    }

    virtual void TearDown() {
        delete analyzer_;
    }

    Analyzer* analyzer_;
};

TEST_F(AnalyzerTest, getOptionDefault) {
    EXPECT_EQ(1, analyzer_->getOption(Analyzer::OPTION_TYPE_POS_TAGGING));
    EXPECT_EQ(1, analyzer_->getOption(Analyzer::OPTION_TYPE_NBEST));
    EXPECT_EQ(0, analyzer_->getOption(Analyzer::OPTION_TYPE_POS_FORMAT_ALPHABET));
    EXPECT_EQ(0, analyzer_->getOption(Analyzer::OPTION_TYPE_POS_FULL_CATEGORY));
    EXPECT_EQ(1, analyzer_->getOption(Analyzer::OPTION_TYPE_COMPOUND_MORPHOLOGY));
    EXPECT_EQ(0, analyzer_->getOption(Analyzer::OPTION_TYPE_DECOMPOSE_USER_NOUN));
    EXPECT_EQ(0, analyzer_->getOption(Analyzer::OPTION_TYPE_CONVERT_TO_HIRAGANA));
    EXPECT_EQ(0, analyzer_->getOption(Analyzer::OPTION_TYPE_CONVERT_TO_KATAKANA));
    EXPECT_EQ(0, analyzer_->getOption(Analyzer::OPTION_TYPE_CONVERT_TO_HALF_WIDTH));
    EXPECT_EQ(0, analyzer_->getOption(Analyzer::OPTION_TYPE_CONVERT_TO_FULL_WIDTH));
    EXPECT_EQ(0, analyzer_->getOption(Analyzer::OPTION_TYPE_CONVERT_TO_LOWER_CASE));
    EXPECT_EQ(0, analyzer_->getOption(Analyzer::OPTION_TYPE_CONVERT_TO_UPPER_CASE));
}

TEST_F(AnalyzerTest, setOption) {
    analyzer_->setOption(Analyzer::OPTION_TYPE_POS_TAGGING, 0);
    EXPECT_EQ(0, analyzer_->getOption(Analyzer::OPTION_TYPE_POS_TAGGING));

    analyzer_->setOption(Analyzer::OPTION_TYPE_POS_TAGGING, -1);
    EXPECT_EQ(-1, analyzer_->getOption(Analyzer::OPTION_TYPE_POS_TAGGING));

    analyzer_->setOption(Analyzer::OPTION_TYPE_NBEST, 5);
    EXPECT_EQ(5, analyzer_->getOption(Analyzer::OPTION_TYPE_NBEST));

    analyzer_->setOption(Analyzer::OPTION_TYPE_NBEST, 3.3);
    EXPECT_EQ(3.3, analyzer_->getOption(Analyzer::OPTION_TYPE_NBEST));

    analyzer_->setOption(Analyzer::OPTION_TYPE_NBEST, 1000);
    EXPECT_EQ(1000, analyzer_->getOption(Analyzer::OPTION_TYPE_NBEST));

    analyzer_->setOption(Analyzer::OPTION_TYPE_NBEST, 0);
    EXPECT_EQ(1000, analyzer_->getOption(Analyzer::OPTION_TYPE_NBEST)) << "invalid nbest value should take no effect";

    analyzer_->setOption(Analyzer::OPTION_TYPE_NBEST, -5);
    EXPECT_EQ(1000, analyzer_->getOption(Analyzer::OPTION_TYPE_NBEST)) << "invalid nbest value should take no effect";

    analyzer_->setOption(Analyzer::OPTION_TYPE_POS_FORMAT_ALPHABET, 1);
    EXPECT_EQ(1, analyzer_->getOption(Analyzer::OPTION_TYPE_POS_FORMAT_ALPHABET));

    analyzer_->setOption(Analyzer::OPTION_TYPE_POS_FORMAT_ALPHABET, -1);
    EXPECT_EQ(-1, analyzer_->getOption(Analyzer::OPTION_TYPE_POS_FORMAT_ALPHABET));

    analyzer_->setOption(Analyzer::OPTION_TYPE_POS_FULL_CATEGORY, 1);
    EXPECT_EQ(1, analyzer_->getOption(Analyzer::OPTION_TYPE_POS_FULL_CATEGORY));

    analyzer_->setOption(Analyzer::OPTION_TYPE_COMPOUND_MORPHOLOGY, 0);
    EXPECT_EQ(0, analyzer_->getOption(Analyzer::OPTION_TYPE_COMPOUND_MORPHOLOGY));

    analyzer_->setOption(Analyzer::OPTION_TYPE_COMPOUND_MORPHOLOGY, -1);
    EXPECT_EQ(-1, analyzer_->getOption(Analyzer::OPTION_TYPE_COMPOUND_MORPHOLOGY));

    analyzer_->setOption(Analyzer::OPTION_TYPE_DECOMPOSE_USER_NOUN, 1);
    EXPECT_EQ(1, analyzer_->getOption(Analyzer::OPTION_TYPE_DECOMPOSE_USER_NOUN));

    analyzer_->setOption(Analyzer::OPTION_TYPE_CONVERT_TO_HIRAGANA, 1);
    EXPECT_EQ(1, analyzer_->getOption(Analyzer::OPTION_TYPE_CONVERT_TO_HIRAGANA));

    analyzer_->setOption(Analyzer::OPTION_TYPE_CONVERT_TO_KATAKANA, 1);
    EXPECT_EQ(1, analyzer_->getOption(Analyzer::OPTION_TYPE_CONVERT_TO_KATAKANA));

    analyzer_->setOption(Analyzer::OPTION_TYPE_CONVERT_TO_HALF_WIDTH, 1);
    EXPECT_EQ(1, analyzer_->getOption(Analyzer::OPTION_TYPE_CONVERT_TO_HALF_WIDTH));

    analyzer_->setOption(Analyzer::OPTION_TYPE_CONVERT_TO_FULL_WIDTH, 1);
    EXPECT_EQ(1, analyzer_->getOption(Analyzer::OPTION_TYPE_CONVERT_TO_FULL_WIDTH));

    analyzer_->setOption(Analyzer::OPTION_TYPE_CONVERT_TO_LOWER_CASE, 1);
    EXPECT_EQ(1, analyzer_->getOption(Analyzer::OPTION_TYPE_CONVERT_TO_LOWER_CASE));

    analyzer_->setOption(Analyzer::OPTION_TYPE_CONVERT_TO_UPPER_CASE, 1);
    EXPECT_EQ(1, analyzer_->getOption(Analyzer::OPTION_TYPE_CONVERT_TO_UPPER_CASE));
}

TEST_F(AnalyzerTest, getPOSDelimiterDefault) {
    EXPECT_STREQ("/", analyzer_->getPOSDelimiter());
}

TEST_F(AnalyzerTest, setPOSDelimiter) {
    analyzer_->setPOSDelimiter("#");
    EXPECT_STREQ("#", analyzer_->getPOSDelimiter());
}

TEST_F(AnalyzerTest, getWordDelimiterDefault) {
    EXPECT_STREQ("  ", analyzer_->getWordDelimiter());
}

TEST_F(AnalyzerTest, setWordDelimiter) {
    analyzer_->setWordDelimiter("@");
    EXPECT_STREQ("@", analyzer_->getWordDelimiter());
}
