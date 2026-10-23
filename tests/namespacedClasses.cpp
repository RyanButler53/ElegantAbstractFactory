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

// int main(){

//     eaf::TaggedAbstractFactory<Factory, Base> factory;
//     std::shared_ptr<Base> d1 = factory.make_shared("Derived1");
//     std::println("{}", d1->structName());
//     std::shared_ptr<Base> d2 = factory.make_shared("Derived2");
//     std::println("{}", d2->structName());

//     std::shared_ptr<Base> nonDerived = factory.make_shared("NotDerived");
//     if (!nonDerived){
//         std::println("Not valid type");
//     }

//     std::shared_ptr<Base> notMarked = factory.make_shared("Derived3");
//     if (!notMarked){
//         std::println("Derived3 is not marked to the abstract factory");
//     }


//     std::shared_ptr<Base> notNamespaced = factory.make_shared("Derived4");
//     if (!notNamespaced){
//         std::println("Derived4 is not in the global namespace to the abstract factory");
//     }

//     eaf::TaggedAbstractFactory<Factory, Base, ^^N> namespacedFactory;
//     d1 = namespacedFactory.make_shared("Derived1");
//     if (!d1){
//         std::println("Derived 1 is not in namespace N");
//     }
//     std::shared_ptr<Base> nd4 = namespacedFactory.make_shared("N::Derived4");
//     std::println("{}", nd4->structName());

//     auto d4 = namespacedFactory.make_shared("Derived4");
//     std::println("{}", d4->structName());

// }
