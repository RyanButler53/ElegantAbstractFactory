#include <meta>
#include <string>
#include <string_view>
#include "classes.hpp"
#include "abstractFactory.hpp"
#include <gtest/gtest.h>

class TaggedFactoryTest : public ::testing::Test {
    protected:
    eaf::TaggedAbstractFactory<Factory, Base> factory;

};

TEST_F(TaggedFactoryTest, ValidClasses){
    std::shared_ptr<Base> d1 = factory.make_shared("Derived1");
    ASSERT_TRUE(d1);
    EXPECT_EQ(d1->structName(), "Derived1");

    d1 = factory.make_shared("Derived2");
    ASSERT_TRUE(d1);
    EXPECT_EQ(d1->structName(), "Derived2");
}

TEST_F(TaggedFactoryTest, UnmarkedClass){
    std::shared_ptr<Base> derived = factory.make_shared("Derived3");
    EXPECT_FALSE(derived);
}

TEST_F(TaggedFactoryTest, NonBaseClass){
    std::shared_ptr<Base> derived = factory.make_shared("NotDerived");
    EXPECT_FALSE(derived);
}

TEST_F(TaggedFactoryTest, NonexistentClass){
    std::unique_ptr<Base> derived = factory.make_unique("NotAClass");
    EXPECT_FALSE(derived);
}
