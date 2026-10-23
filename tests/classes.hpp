/**
 * @file classes.hpp
 * @author Ryan Butler
 * @brief Declares and defines multiple small structs for testing
 * @version 0.1
 * @date 2026-10-22
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include <meta>
#include <string_view>

struct Base {
    virtual std::string_view structName() = 0;
};

// Set up constexpr annotation tags to register to the factory
struct Factory{};
Factory AbstractFactory;

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
