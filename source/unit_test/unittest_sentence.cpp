/** \file unittest_sentence.cpp
 * Unit test of class Sentence.
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Sep 10, 2010
 */

#include <gtest/gtest.h>
#include <ijma/sentence.h>
#include <algorithm>

using namespace jma;
using namespace std;

TEST(SentenceTest, morphemeDefault) {
    Morpheme morpheme;

    EXPECT_EQ("", morpheme.lexicon_);
    EXPECT_EQ(-1, morpheme.posCode_);
    EXPECT_EQ("", morpheme.posStr_);
    EXPECT_EQ("", morpheme.baseForm_);
    EXPECT_EQ("", morpheme.readForm_);
    EXPECT_EQ("", morpheme.normForm_);
}

TEST(SentenceTest, getDefault) {
    Sentence sent;

    EXPECT_STREQ("", sent.getString());
    EXPECT_EQ(0, sent.getListSize());
    EXPECT_EQ(-1, sent.getOneBestIndex());
}

TEST(SentenceTest, getValue) {
    Sentence sent;

    MorphemeList morphList1;
    morphList1.push_back(Morpheme("lexicon11", 1, "noun", "base11", "read11", "norm11"));
    morphList1.push_back(Morpheme("lexicon12", 2, "verb", "base12", "read12", "norm12"));
    sent.addList(morphList1, 0.2);

    MorphemeList morphList2;
    morphList2.push_back(Morpheme("lexicon21", 3, "adj", "base21", "read21", "norm21"));
    morphList2.push_back(Morpheme("lexicon22", 4, "adv", "base22", "read22", "norm22"));
    morphList2.push_back(Morpheme("lexicon23", 5, "punct", "base23", "read23", "norm23"));
    sent.addList(morphList2, 0.8);

    MorphemeList morphList3;
    sent.addList(morphList3);

    EXPECT_EQ(3, sent.getListSize());
    EXPECT_EQ(1, sent.getOneBestIndex());

    int i = sent.getOneBestIndex();
    EXPECT_EQ(3, sent.getCount(i));
    EXPECT_STREQ("lexicon21", sent.getLexicon(i, 0));
    EXPECT_STREQ("lexicon22", sent.getLexicon(i, 1));
    EXPECT_STREQ("lexicon23", sent.getLexicon(i, 2));
    EXPECT_EQ(3, sent.getPOS(i, 0));
    EXPECT_EQ(4, sent.getPOS(i, 1));
    EXPECT_EQ(5, sent.getPOS(i, 2));
    EXPECT_STREQ("adj", sent.getStrPOS(i, 0));
    EXPECT_STREQ("adv", sent.getStrPOS(i, 1));
    EXPECT_STREQ("punct", sent.getStrPOS(i, 2));
    EXPECT_STREQ("base21", sent.getBaseForm(i, 0));
    EXPECT_STREQ("base22", sent.getBaseForm(i, 1));
    EXPECT_STREQ("base23", sent.getBaseForm(i, 2));
    EXPECT_STREQ("read21", sent.getReadForm(i, 0));
    EXPECT_STREQ("read22", sent.getReadForm(i, 1));
    EXPECT_STREQ("read23", sent.getReadForm(i, 2));
    EXPECT_STREQ("norm21", sent.getNormForm(i, 0));
    EXPECT_STREQ("norm22", sent.getNormForm(i, 1));
    EXPECT_STREQ("norm23", sent.getNormForm(i, 2));
    EXPECT_EQ(0.8, sent.getScore(i));
    sent.setScore(i, 0.64);
    EXPECT_EQ(0.64, sent.getScore(i));

    i = 0;
    EXPECT_EQ(2, sent.getCount(i));
    EXPECT_STREQ("lexicon11", sent.getLexicon(i, 0));
    EXPECT_STREQ("lexicon12", sent.getLexicon(i, 1));
    EXPECT_EQ(1, sent.getPOS(i, 0));
    EXPECT_EQ(2, sent.getPOS(i, 1));
    EXPECT_STREQ("noun", sent.getStrPOS(i, 0));
    EXPECT_STREQ("verb", sent.getStrPOS(i, 1));
    EXPECT_STREQ("base11", sent.getBaseForm(i, 0));
    EXPECT_STREQ("base12", sent.getBaseForm(i, 1));
    EXPECT_STREQ("read11", sent.getReadForm(i, 0));
    EXPECT_STREQ("read12", sent.getReadForm(i, 1));
    EXPECT_STREQ("norm11", sent.getNormForm(i, 0));
    EXPECT_STREQ("norm12", sent.getNormForm(i, 1));
    EXPECT_EQ(0.2, sent.getScore(i));
    sent.setScore(i, 0.32);
    EXPECT_EQ(0.32, sent.getScore(i));

    i = 2;
    EXPECT_EQ(0, sent.getCount(i));
}
