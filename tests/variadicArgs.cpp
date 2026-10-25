#include <print>
#include <meta>
#include <string>
#include <format>
#include <string_view>
#include "classes.hpp"
#include "abstractFactory.hpp"
#include <gtest/gtest.h>


struct TwoArgs {};
TwoArgs FactoryTag;

struct [[=FactoryTag]] Derived6 : public Base {
    int n_;
    std::string s_;

    Derived6(int n, std::string s):n_{n}, s_{s}{}

    std::string_view structName() override {
        return std::meta::display_string_of(^^Derived6);
    }
};



class ConstructorArgsTest : public ::testing::Test {
    protected:
    eaf::TaggedAbstractFactory<TwoArgs, Base> factory;
};

TEST_F(ConstructorArgsTest, ValidClasses){
    std::shared_ptr<Base> derived = factory.make_shared("Derived6", 6, std::string("Variadic"));
    EXPECT_NE(derived, nullptr);
    // Class can be built without the namespaced name
    EXPECT_EQ(derived->structName(), "Derived6");
    std::shared_ptr<Derived6> d5 = std::dynamic_pointer_cast<Derived6>(derived);
    EXPECT_NE(d5, nullptr);
    EXPECT_EQ(d5->n_, 6);
    EXPECT_EQ(d5->s_, "Variadic");

}
