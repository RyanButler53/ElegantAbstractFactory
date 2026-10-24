#include <print>
#include <meta>
#include <string>
#include <format>
#include <string_view>
#include "classes.hpp"
#include "abstractFactory.hpp"
#include <gtest/gtest.h>



namespace N {
    struct [[=AbstractFactory]] Derived4 : public Base {
        std::string_view structName() override {
            return std::meta::display_string_of(^^Derived4);
        }
    };

    namespace Nested {
        struct [[=AbstractFactory]] Derived5 : public Base {
            std::string_view structName() override {
                return std::meta::display_string_of(^^Derived5);
            }
        };
    }
}

class NamespacedClassesTest : public ::testing::Test {
    protected:
    eaf::TaggedAbstractFactory<Factory, Base, ^^N> factory;
};

TEST_F(NamespacedClassesTest, ValidClasses){
    std::shared_ptr<Base> derived = factory.make_shared("Derived4");
    EXPECT_NE(derived, nullptr);
    // Class can be built without the namespaced name
    EXPECT_EQ(derived->structName(), "N::Derived4");

    derived = factory.make_shared("N::Derived4");
    EXPECT_NE(derived, nullptr);
    EXPECT_EQ(derived->structName(), "N::Derived4");
}

TEST_F(NamespacedClassesTest, GlobalNamespace){
    std::shared_ptr<Base> derived = factory.make_shared("Derived1");
    EXPECT_EQ(derived, nullptr);
}

// Nested namespace classes are not supported. 
TEST_F(NamespacedClassesTest, NestedNamespaces){
    std::unique_ptr<Base> derived = factory.make_unique("Nested::Derived5");
    EXPECT_EQ(derived, nullptr);
}