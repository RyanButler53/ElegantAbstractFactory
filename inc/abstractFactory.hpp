/**
 * @file abstractFactory.hpp
 * @author Ryan Butler (rmbutler@outlook.com)
 * @brief Template definition of abstract factory class
 * @version 0.1
 * @date 2026-10-21
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#pragma once

#include <meta>
#include <type_traits>
#include <memory>
#include <string_view>

namespace eaf {

    /**
     * @brief Abstract Factory 
     * 
     * @tparam Tag 
     * @tparam Base 
     */
    template <class Tag, class Base>
    class TaggedAbstractFactory {

        /**
         * @brief Internal function to create the raw pointer to the derived class. Gets wrapped 
         * by a smart pointer by the corresponding factory function
         * 
         * @param className Class name to make
         * @return Base* 
         */
        Base* make(std::string_view className);

        public:

        /**
         * @brief Construct a new Tagged Abstract Factory. The default constructor will construct a factory
         * that can construct objects of any type in the global namespace that both derive from the base class Base
         * and are annotated with class Tag. 
         * 
         */
        TaggedAbstractFactory() = default;

        /**
         * @brief Construct a shared/unique pointer to the provided class name
         * @todo Make this work with variadic arguments
         * 
         * @param classname Class name of the type to construct
         */
        std::shared_ptr<Base> make_shared(std::string_view classname);
        std::unique_ptr<Base> make_unique(std::string_view classname);

    };
    
    
    // template <class Base>
    // class AbstractFactory {

    //     /**
    //      * @brief Construct a new generic Abstract Factory. The default constructor will construct a factory
    //      * that can construct objects of any type in the global namespace that both derive from the base class Base
    //      * Annotations are ignored
    //      */
    //     AbstractFactory();

    //     /**
    //      * @brief Construct a new  Abstract Factory object that constructs objects that
    //      * are derived classes of Base that are in a specific namespace. Annotations are not used
    //      * 
    //      * @param namespaceRef Reflection of a namespace to look for derived classes in
    //      */
    //     AbstractFactory(std::meta::info namespaceRef);

    //     /**
    //      * @brief Construct a shared/unique pointer to the provided class name
    //      * @todo Make this work with variadic arguments
    //      * 
    //      * @param classname Class name of the type to construct
    //      */
    //     std::shared_ptr<Base> make_shared(std::string_view classname);
    //     std::unique_ptr<Base> make_unique(std::string_view classname);

    // };
}

template <typename Tag, typename Base>
Base* eaf::TaggedAbstractFactory<Tag, Base>::make(std::string_view className){

    constexpr auto ctx = std::meta::access_context::unprivileged();

    template for (constexpr auto cls : std::define_static_array(std::meta::members_of(^^::, ctx))){
        if constexpr (std::meta::is_type(cls) && std::meta::is_complete_type(cls)){
            static constexpr auto annotations = std::define_static_array(std::meta::annotations_of_with_type(cls, ^^Tag));
            using Class_t = typename [:cls:];
            if constexpr (annotations.size() == 1 && std::is_base_of_v<Base, Class_t> ){ // , std::meta::display_string_of(^^Base)
                if (std::meta::display_string_of(cls) == className){
                    return new Class_t();
                }
            }
        }
    }
    return nullptr;
}


template <typename Tag, typename Base>
std::shared_ptr<Base> eaf::TaggedAbstractFactory<Tag, Base>::make_shared(std::string_view className){
    return std::shared_ptr<Base>(make(className));
}

template <typename Tag, typename Base>
std::unique_ptr<Base> eaf::TaggedAbstractFactory<Tag, Base>::make_unique(std::string_view className){
    return std::unique_ptr<Base>(make(className));
}