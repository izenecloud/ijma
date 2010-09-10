/** \file unittest_analyzer.cpp
 * Unit test of class Analyzer.
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Sep 10, 2010
 */

#include <gtest/gtest.h>
#include <analyzer.h>
#include <jma_analyzer.h>

using namespace jma;

TEST(AnalyzerTest, getOptionDefault) {
    Analyzer* analyzer = new JMA_Analyzer;
    ASSERT_TRUE(analyzer != NULL);

    EXPECT_EQ(1, analyzer->getOption(Analyzer::OPTION_TYPE_POS_TAGGING));
    EXPECT_EQ(1, analyzer->getOption(Analyzer::OPTION_TYPE_NBEST));
    EXPECT_EQ(0, analyzer->getOption(Analyzer::OPTION_TYPE_POS_FORMAT_ALPHABET));
    EXPECT_EQ(0, analyzer->getOption(Analyzer::OPTION_TYPE_POS_FULL_CATEGORY));
    EXPECT_EQ(1, analyzer->getOption(Analyzer::OPTION_TYPE_COMPOUND_MORPHOLOGY));
    EXPECT_EQ(0, analyzer->getOption(Analyzer::OPTION_TYPE_DECOMPOSE_USER_NOUN));
    EXPECT_EQ(0, analyzer->getOption(Analyzer::OPTION_TYPE_CONVERT_TO_HIRAGANA));
    EXPECT_EQ(0, analyzer->getOption(Analyzer::OPTION_TYPE_CONVERT_TO_KATAKANA));
    EXPECT_EQ(0, analyzer->getOption(Analyzer::OPTION_TYPE_CONVERT_TO_HALF_WIDTH));
    EXPECT_EQ(0, analyzer->getOption(Analyzer::OPTION_TYPE_CONVERT_TO_FULL_WIDTH));
    EXPECT_EQ(0, analyzer->getOption(Analyzer::OPTION_TYPE_CONVERT_TO_LOWER_CASE));
    EXPECT_EQ(0, analyzer->getOption(Analyzer::OPTION_TYPE_CONVERT_TO_UPPER_CASE));

    delete analyzer;
}

TEST(AnalyzerTest, setOption) {
    Analyzer* analyzer = new JMA_Analyzer;
    ASSERT_TRUE(analyzer != NULL);

    analyzer->setOption(Analyzer::OPTION_TYPE_POS_TAGGING, 0);
    EXPECT_EQ(0, analyzer->getOption(Analyzer::OPTION_TYPE_POS_TAGGING));

    analyzer->setOption(Analyzer::OPTION_TYPE_POS_TAGGING, -1);
    EXPECT_EQ(-1, analyzer->getOption(Analyzer::OPTION_TYPE_POS_TAGGING));

    analyzer->setOption(Analyzer::OPTION_TYPE_NBEST, 5);
    EXPECT_EQ(5, analyzer->getOption(Analyzer::OPTION_TYPE_NBEST));

    analyzer->setOption(Analyzer::OPTION_TYPE_NBEST, 3.3);
    EXPECT_EQ(3.3, analyzer->getOption(Analyzer::OPTION_TYPE_NBEST));

    analyzer->setOption(Analyzer::OPTION_TYPE_NBEST, 1000);
    EXPECT_EQ(1000, analyzer->getOption(Analyzer::OPTION_TYPE_NBEST));

    analyzer->setOption(Analyzer::OPTION_TYPE_NBEST, 0);
    EXPECT_EQ(1000, analyzer->getOption(Analyzer::OPTION_TYPE_NBEST)) << "invalid nbest value should take no effect";

    analyzer->setOption(Analyzer::OPTION_TYPE_NBEST, -5);
    EXPECT_EQ(1000, analyzer->getOption(Analyzer::OPTION_TYPE_NBEST)) << "invalid nbest value should take no effect";

    analyzer->setOption(Analyzer::OPTION_TYPE_POS_FORMAT_ALPHABET, 1);
    EXPECT_EQ(1, analyzer->getOption(Analyzer::OPTION_TYPE_POS_FORMAT_ALPHABET));

    analyzer->setOption(Analyzer::OPTION_TYPE_POS_FORMAT_ALPHABET, -1);
    EXPECT_EQ(-1, analyzer->getOption(Analyzer::OPTION_TYPE_POS_FORMAT_ALPHABET));

    analyzer->setOption(Analyzer::OPTION_TYPE_POS_FULL_CATEGORY, 1);
    EXPECT_EQ(1, analyzer->getOption(Analyzer::OPTION_TYPE_POS_FULL_CATEGORY));

    analyzer->setOption(Analyzer::OPTION_TYPE_COMPOUND_MORPHOLOGY, 0);
    EXPECT_EQ(0, analyzer->getOption(Analyzer::OPTION_TYPE_COMPOUND_MORPHOLOGY));

    analyzer->setOption(Analyzer::OPTION_TYPE_COMPOUND_MORPHOLOGY, -1);
    EXPECT_EQ(-1, analyzer->getOption(Analyzer::OPTION_TYPE_COMPOUND_MORPHOLOGY));

    analyzer->setOption(Analyzer::OPTION_TYPE_DECOMPOSE_USER_NOUN, 1);
    EXPECT_EQ(1, analyzer->getOption(Analyzer::OPTION_TYPE_DECOMPOSE_USER_NOUN));

    analyzer->setOption(Analyzer::OPTION_TYPE_CONVERT_TO_HIRAGANA, 1);
    EXPECT_EQ(1, analyzer->getOption(Analyzer::OPTION_TYPE_CONVERT_TO_HIRAGANA));

    analyzer->setOption(Analyzer::OPTION_TYPE_CONVERT_TO_KATAKANA, 1);
    EXPECT_EQ(1, analyzer->getOption(Analyzer::OPTION_TYPE_CONVERT_TO_KATAKANA));

    analyzer->setOption(Analyzer::OPTION_TYPE_CONVERT_TO_HALF_WIDTH, 1);
    EXPECT_EQ(1, analyzer->getOption(Analyzer::OPTION_TYPE_CONVERT_TO_HALF_WIDTH));

    analyzer->setOption(Analyzer::OPTION_TYPE_CONVERT_TO_FULL_WIDTH, 1);
    EXPECT_EQ(1, analyzer->getOption(Analyzer::OPTION_TYPE_CONVERT_TO_FULL_WIDTH));

    analyzer->setOption(Analyzer::OPTION_TYPE_CONVERT_TO_LOWER_CASE, 1);
    EXPECT_EQ(1, analyzer->getOption(Analyzer::OPTION_TYPE_CONVERT_TO_LOWER_CASE));

    analyzer->setOption(Analyzer::OPTION_TYPE_CONVERT_TO_UPPER_CASE, 1);
    EXPECT_EQ(1, analyzer->getOption(Analyzer::OPTION_TYPE_CONVERT_TO_UPPER_CASE));

    delete analyzer;
}

TEST(AnalyzerTest, getPOSDelimiterDefault) {
    Analyzer* analyzer = new JMA_Analyzer;
    ASSERT_TRUE(analyzer != NULL);

    EXPECT_STREQ("/", analyzer->getPOSDelimiter());

    delete analyzer;
}

TEST(AnalyzerTest, setPOSDelimiter) {
    Analyzer* analyzer = new JMA_Analyzer;
    ASSERT_TRUE(analyzer != NULL);

    analyzer->setPOSDelimiter("#");
    EXPECT_STREQ("#", analyzer->getPOSDelimiter());

    delete analyzer;
}

TEST(AnalyzerTest, getWordDelimiterDefault) {
    Analyzer* analyzer = new JMA_Analyzer;
    ASSERT_TRUE(analyzer != NULL);

    EXPECT_STREQ("  ", analyzer->getWordDelimiter());

    delete analyzer;
}

TEST(AnalyzerTest, setWordDelimiter) {
    Analyzer* analyzer = new JMA_Analyzer;
    ASSERT_TRUE(analyzer != NULL);

    analyzer->setWordDelimiter("@");
    EXPECT_STREQ("@", analyzer->getWordDelimiter());

    delete analyzer;
}
