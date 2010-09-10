/** \file unittest_jma_factory.cpp
 * Unit test of class JMA_Factory.
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Sep 10, 2010
 */

#include <gtest/gtest.h>
#include <jma_factory.h>
#include <analyzer.h>
#include <knowledge.h>

using namespace jma;

TEST(JMAFactoryTest, instance) {
    JMA_Factory* factory = JMA_Factory::instance();

    EXPECT_TRUE(factory);

    JMA_Factory* factory1 = JMA_Factory::instance();
    EXPECT_EQ(factory, factory1) << "JMA_Factory::instance() should return singleton value";

    JMA_Factory* factory2 = JMA_Factory::instance();
    EXPECT_EQ(factory, factory2) << "JMA_Factory::instance() should return singleton value";
}

TEST(JMAFactoryTest, create) {
    JMA_Factory* factory = JMA_Factory::instance();

    ASSERT_TRUE(factory);

    Analyzer* analyzer = factory->createAnalyzer();
    EXPECT_TRUE(analyzer);

    Analyzer* analyzer1 = factory->createAnalyzer();
    EXPECT_TRUE(analyzer1);

    Analyzer* analyzer2 = factory->createAnalyzer();
    EXPECT_TRUE(analyzer2);

    Knowledge* knowledge = factory->createKnowledge();
    EXPECT_TRUE(knowledge);

    Knowledge* knowledge1 = factory->createKnowledge();
    EXPECT_TRUE(knowledge1);

    Knowledge* knowledge2 = factory->createKnowledge();
    EXPECT_TRUE(knowledge2);

    delete analyzer;
    delete analyzer1;
    delete analyzer2;
    delete knowledge;
    delete knowledge1;
    delete knowledge2;
}
