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
#include <ranges>
#include <utility>
#include <memory>
#include <string_view>
#include <cstring>

namespace eaf {

    /**
     * @brief Abstract Factory 
     * 
     * @tparam Tag: Annotation type of the tag for each class
     * @tparam Base: Class that all tagged types must inherit from
     * @tparam Mapping: Type of alternaive mapping to class name (t)
     * @tparam Namespace: Reflection of the namespace to look in for types
     */
    template <class Tag, class Base, std::meta::info Namespace = ^^::, class Mapping = int >
    class TaggedAbstractFactory {

        /**
         * @brief Internal function to create the raw pointer to the derived class. Gets wrapped 
         * by a smart pointer by the corresponding factory function
         * 
         * @param className Class name to make
         * @tparam Args
         * @return Base* Raw pointer to the base class. Wrapped in a smart pointer immediately
         */
        template <typename... Args> 
        Base* make(std::string_view className, Args&&... args);

        template <typename... Args>
        Base* make(Mapping classMap, Args&&... args);

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
         * 
         * @param classname Class name of the type to construct
         */
        template <typename... Args>
        std::shared_ptr<Base> make_shared(std::string_view classname, Args&&... args);

        template <typename... Args>
        std::unique_ptr<Base> make_unique(std::string_view classname, Args&&... args);


        /**
         * @brief Construct a shared/unique pointer to the type specified by the class identifier. 
         * The class identifier is an annotation of the class of type \ref Mapping
         * 
         * @tparam Args Variadic type for all arguments
         * @param classIdentifier Class identifier from the annotation. 
         * @param args Arguments forwarded to std::make_shared<classIdentifier>
         * @return std::shared_ptr<Base> 
         */
        template <typename... Args>
        std::shared_ptr<Base> make_shared(Mapping classIdentifier, Args&&... args);

        template <typename... Args>
        std::unique_ptr<Base> make_unique(Mapping classIdentifier, Args&&... args);

    };
    
    /**
     * @brief Get the class name from a full class name involving namespaces and :: operators
     * 
     * @tparam R Reflection of type to get name of. 
     * @return consteval std::string_view of the class name. Runs at compile time. 
     */
    template <std::meta::info R>
    consteval std::string_view getClassName(){
        using namespace std::literals;
        static constexpr std::string_view delim = "::"sv;
        auto v = std::views::split(std::meta::display_string_of(R), delim)
        | std::views::transform([](auto&& subrange){
            return std::string_view(subrange);
        })
        | std::ranges::to<std::vector<std::string_view>>();
        return v.back();
    }
    
    /**
     * @brief Special Case for string based annotations since string memory is dynamic
     * 
     */
    struct String {
        const char* name;

        consteval String(std::string_view s): name(std::define_static_string(s)) {}

        constexpr bool operator==(const String& other){
            return std::strcmp(name, other.name) == 0;
        }

    };

template <typename Tag, typename Base, std::meta::info Namespace, class Mapping>
template <typename... Args>
Base* eaf::TaggedAbstractFactory<Tag, Base, Namespace, Mapping>::make(std::string_view className, Args&&... args){

    constexpr auto ctx = std::meta::access_context::unprivileged();

    template for (constexpr auto cls : std::define_static_array(std::meta::members_of(Namespace, ctx))){
        if constexpr (std::meta::is_type(cls) && std::meta::is_complete_type(cls)){
            // Gets all annotations of type "Tag" that are registered to the factory
            static constexpr auto annotations = std::define_static_array(std::meta::annotations_of_with_type(cls, ^^Tag));
            using Class_t = typename [:cls:];
            if constexpr (!annotations.empty() && std::is_base_of_v<Base, Class_t>){
                
                // Strips the :: out of namespaced classes.
                static constexpr std::string_view prunedClassName = eaf::getClassName<cls>();
                static constexpr std::string_view clsName = std::meta::display_string_of(cls);
                if (clsName == className || prunedClassName == className){
                    return new Class_t(std::forward<Args>(args)...);
                }
            }
        }
    }
    return nullptr;
}


template <typename Tag, typename Base, std::meta::info Namespace, class Mapping>
template <typename... Args>
Base* eaf::TaggedAbstractFactory<Tag, Base, Namespace, Mapping>::make(Mapping classIdentifier, Args&&... args){
    constexpr auto ctx = std::meta::access_context::unprivileged();
    template for (constexpr auto cls : std::define_static_array(std::meta::members_of(Namespace, ctx))){
        if constexpr (std::meta::is_type(cls) && std::meta::is_complete_type(cls)){

            // Check if type is annotated to be in the factory
            static constexpr bool inFactory = !std::define_static_array(std::meta::annotations_of_with_type(cls, ^^Tag)).empty();
            using Class_t = typename [:cls:];
            if constexpr (inFactory && std::is_base_of_v<Base, Class_t>){
                // Loop over all annotations matching type Mapping to match with input parameter
                template for (constexpr auto mapping : std::define_static_array(std::meta::annotations_of_with_type(cls, ^^Mapping))){
                    if ([:std::meta::constant_of(mapping):] == classIdentifier){
                        return new Class_t(std::forward<Args>(args)...);
                    }
                }
            }
        }
    }
    return nullptr;
}
}

// Implementations

template <typename Tag, typename Base, std::meta::info Namespace, class Mapping>
template <typename... Args>
std::shared_ptr<Base> eaf::TaggedAbstractFactory<Tag, Base, Namespace, Mapping>::make_shared(std::string_view className, Args&&... args){
    return std::shared_ptr<Base>(make(className, std::forward<Args>(args)...));
}

template <typename Tag, typename Base, std::meta::info Namespace, class Mapping>
template <typename... Args>
std::unique_ptr<Base> eaf::TaggedAbstractFactory<Tag, Base, Namespace, Mapping>::make_unique(std::string_view className, Args&&... args){
    return std::unique_ptr<Base>(make(className, std::forward<Args>(args)...));
}

template <typename Tag, typename Base, std::meta::info Namespace, class Mapping>
template <typename... Args>
std::shared_ptr<Base> eaf::TaggedAbstractFactory<Tag, Base, Namespace, Mapping>::make_shared(Mapping classIdentifier, Args&&... args){
    return std::shared_ptr<Base>(make(classIdentifier, std::forward<Args>(args)...));
}

template <typename Tag, typename Base, std::meta::info Namespace, class Mapping>
template <typename... Args>
std::unique_ptr<Base> eaf::TaggedAbstractFactory<Tag, Base, Namespace, Mapping>::make_unique(Mapping classIdentifier, Args&&... args){
    return std::unique_ptr<Base>(make(classIdentifier, std::forward<Args>(args)...));
}
