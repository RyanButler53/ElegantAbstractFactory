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
#include <memory>

namespace eaf {

    /**
     * @brief Abstract Factory 
     * 
     * @tparam Tag 
     * @tparam Base 
     */
    template <class Tag, class Base>
    class TaggedAbstractFactory {

        template <std::meta::info R>
        void init();

        public:

        /**
         * @brief Construct a new Tagged Abstract Factory. The default constructor will construct a factory
         * that can construct objects of any type in the global namespace that both derive from the base class Base
         * and are annotated with class Tag. 
         * 
         */
        TaggedAbstractFactory();

        /**
         * @brief Construct a new Tagged Abstract Factory object that constructs objects that
         * are derived classes of Base that are in a specific namespace. 
         * 
         * @param namespaceRef Reflection of a namespace to look for derived classes in
         */
        // template<std::meta::info namespaceRef>
        // TaggedAbstractFactory();

        /**
         * @brief Construct a shared/unique pointer to the provided class name
         * @todo Make this work with variadic arguments
         * 
         * @param classname Class name of the type to construct
         */
        std::shared_ptr<Base> make_shared(std::string_view classname){return nullptr;}
        std::unique_ptr<Base> make_unique(std::string_view classname){return nullptr;}

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
eaf::TaggedAbstractFactory<Tag, Base>::TaggedAbstractFactory(){
    init<^^::>();
}


// template <typename Tag, typename Base>
// template <std::meta::info R>
// TaggedAbstractFactory<Tag, Base>::TaggedAbstractFactory(){
//     init<R>();
// }

template <typename Tag, typename Base>
template <std::meta::info R>
void eaf::TaggedAbstractFactory<Tag, Base>::init(){
    if constexpr (!std::meta::is_namespace(R)){
        return;
    }
    constexpr auto ctx = std::meta::access_context::unprivileged();

    template for (constexpr auto cls : std::define_static_array(std::meta::members_of(R, ctx))){
        if constexpr (std::meta::is_type(cls)){
            static constexpr auto annotations = std::define_static_array(std::meta::annotations_of(cls));
            if constexpr (annotations.size() == 1){
                std::println("{} Has ONE annotation!", std::meta::display_string_of(annotations[0]));
            } else {
                std::println("{} has no annotations :(", std::meta::display_string_of(cls));
            }
        }
    }
}
