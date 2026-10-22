#include <print>
#include <meta>
#include <string>
#include <vector>
#include <ranges>
#include <iostream>
#include <format>
#include <fstream>
#include <cstdlib>
#include "abstractFactory.hpp"

struct Base{
    virtual std::string_view structName() = 0;
};

// Set up constexpr annotation tags to register to the factory
struct Factory{};
constexpr Factory AbstractFactory;

struct [[=AbstractFactory]] Derived1 : public Base {
    std::string_view  structName() override {
        return std::meta::display_string_of(^^Derived1);
    }
};

struct [[=AbstractFactory]] Derived2 : public Base {
    std::string_view structName() override {
        return std::meta::display_string_of(^^Derived2);

    }
};

struct  Derived3 : public Base {
    std::string_view structName() override {
        return std::meta::display_string_of(^^Derived3);

    }
};

struct NotDerived {
    std::string_view structName(){
        constexpr std::meta::info r = ^^NotDerived;
        return std::meta::display_string_of(r);
    }
};

// class TaggedFactoryTest : public ::testing::Test {
//     protected:
//     eaf::TaggedAbstractFactory<Factory, Base> factory;
// };
// TEST_F(TaggedFactoryTest, ValidClasses){
//     std::shared_ptr<Base> d1 = factory.make_shared("Derived1");
//     EXPECT_NEQ(d1, nullptr);
//     EXPECT_EQ(d1->structName(), "Derived1");

//     d1 = factory.make_shared("Derived2");
//     EXPECT_NEQ(d1, nullptr);
//     EXPECT_EQ(d1->structName(), "Derived2");
// }

// TEST_F(TaggedFactoryTest, UnmarkedClass){
//     std::shared_ptr<Base> derived = factory.make_shared("Derived3");
//     EXPECT_EQ(derived, nullptr);
// }

// TEST_F(TaggedFactoryTest, NonBaseClass){
//     std::shared_ptr<Base> derived = factory.make_shared("NotDerived");
//     EXPECT_EQ(derived, nullptr);
// }

// TEST_F(TaggedFactoryTest, NonexistentClass){
//     std::shared_ptr<Base> derived = factory.make_shared("NotAClass");
//     EXPECT_EQ(derived, nullptr);
// }


int main(){

    eaf::TaggedAbstractFactory<Factory, Base> factory;
    std::shared_ptr<Base> d1 = factory.make_shared("Derived1");
    std::println("{}", d1->structName());
    std::shared_ptr<Base> d2 = factory.make_shared("Derived2");
    std::println("{}", d2->structName());

    std::shared_ptr<Base> nonDerived = factory.make_shared("NotDerived");
    if (!nonDerived){
        std::println("Not valid type");
    }

    std::shared_ptr<Base> notMarked = factory.make_shared("Derived3");
    if (!notMarked){
        std::println("Derived3 is not marked to the abstract factory");
    }
}
