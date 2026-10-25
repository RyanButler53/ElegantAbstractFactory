#include <string_view>

#include "abstractFactory.hpp"
#include "classes.hpp"

#include <gtest/gtest.h>


enum class DERIVED_CLASS : uint8_t {
    MN = 1,
    EN = 2
};

namespace Mappings {

    struct [[=AbstractFactory, =70]] LongComplicatedName : public Base {
        std::string_view structName() override {
            return std::meta::display_string_of(^^LongComplicatedName);
        }
    };

    struct [[=AbstractFactory, =81, =105, =DERIVED_CLASS::MN]] ManyNames : public Base {
        std::string_view structName() override {
            return std::meta::display_string_of(^^ManyNames);
        }
    };

    struct [[=AbstractFactory, =DERIVED_CLASS::EN]] EnumeratedName : public Base {
        std::string_view structName() override {
            return std::meta::display_string_of(^^EnumeratedName);
        }
    };

    struct [[=AbstractFactory, =eaf::String("fromString")]] StringTagName : public Base{
        std::string_view structName() override {
            return std::meta::display_string_of(^^StringTagName);
        }
    };
}

class CustomMappingsTest : public ::testing::Test{
    protected:
    eaf::AbstractFactory<Factory, Base, ^^Mappings, int> intFactory;
    eaf::AbstractFactory<Factory, Base, ^^Mappings, DERIVED_CLASS> enumFactory;
    eaf::AbstractFactory<Factory, Base, ^^Mappings, eaf::String> strFactory;

};

TEST_F(CustomMappingsTest, FromInteger){
    std::shared_ptr lcn = intFactory.make_shared(70);
    EXPECT_TRUE(lcn);
    EXPECT_TRUE(std::dynamic_pointer_cast<Mappings::LongComplicatedName>(lcn));
}

TEST_F(CustomMappingsTest, MultipleAlternativeNames){
    std::unique_ptr<Base> multipleNames1 = intFactory.make_unique(81);
    std::shared_ptr<Base> multipleNames2 = intFactory.make_shared(105);

    EXPECT_TRUE(multipleNames1);
    EXPECT_TRUE(multipleNames2);
    EXPECT_TRUE(std::dynamic_pointer_cast<Mappings::ManyNames>(multipleNames2));
    EXPECT_EQ(multipleNames1->structName(), "Mappings::ManyNames");
}

TEST_F(CustomMappingsTest, FromClassName){
    std::shared_ptr className = intFactory.make_shared("LongComplicatedName");
    EXPECT_TRUE(className);
    EXPECT_TRUE(std::dynamic_pointer_cast<Mappings::LongComplicatedName>(className));

    std::shared_ptr fullClassName = intFactory.make_shared("Mappings::LongComplicatedName");
    EXPECT_TRUE(fullClassName);
    EXPECT_TRUE(std::dynamic_pointer_cast<Mappings::LongComplicatedName>(fullClassName));
}

TEST_F(CustomMappingsTest, FromEnum){
    std::shared_ptr enumNames = enumFactory.make_shared(DERIVED_CLASS::EN);
    EXPECT_TRUE(enumNames);
    EXPECT_TRUE(std::dynamic_pointer_cast<Mappings::EnumeratedName>(enumNames));
}

TEST_F(CustomMappingsTest, MultipleNamesDifferentFactories){
    std::unique_ptr fromInt = intFactory.make_unique(81);
    std::unique_ptr fromEnum = enumFactory.make_unique(DERIVED_CLASS::MN);

    EXPECT_TRUE(fromInt);
    EXPECT_TRUE(fromEnum);
    EXPECT_EQ(fromInt->structName(), fromEnum->structName());
}

TEST_F(CustomMappingsTest, FromStringView){
    std::shared_ptr stringName = strFactory.make_shared(eaf::String("fromString"));
    EXPECT_TRUE(stringName);
    EXPECT_TRUE(std::dynamic_pointer_cast<Mappings::StringTagName>(stringName));
}

TEST_F(CustomMappingsTest, FromClassNameSV){
    std::shared_ptr stringName = strFactory.make_shared("StringTagName");
    EXPECT_TRUE(stringName);
    EXPECT_TRUE(std::dynamic_pointer_cast<Mappings::StringTagName>(stringName));

    stringName = strFactory.make_shared("Mappings::StringTagName");
    EXPECT_TRUE(stringName);
    EXPECT_TRUE(std::dynamic_pointer_cast<Mappings::StringTagName>(stringName));

}

