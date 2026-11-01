#pragma once
#include <meta>
#include <expected>
#include <print>
#include <utility>
#include <ranges>
#include <functional>

/**
 * @brief 
 * 
 * f(tag) -> call a function based on the annotation of the tag
 * 
 * can this work with std::expected at all? 
 * 
 */
namespace eaf {

    /**
     * @brief Abstract function dispatcher class
     * 
     * @tparam R: Return type of the function. All function signatures must be the same
     * @tparam Registration Type of annotation required to register to this specific dispatcher instance
     * @tparam Mapping Type of the mapping class. All functions annotated must be called by their mapping, not by function name
     * @tparam Namespace Namespace to look for annotated functions
     */
    template <typename Registration, class R, class Mapping, std::meta::info Namespace>
    class FunctionDispatcher {

        public:
        FunctionDispatcher() = default;

        template <typename... Args>
        R dispatch(std::string_view functionName, Args&&... args);

        /**
         * @brief Dispatches the function specified by the mapping to the generalized dispatcher
         * If the no function matches the expected 
         * 
         * @tparam Args 
         * @param functionIdentifier 
         * @param args 
         * @return R 
         */
        template <typename... Args>
        R dispatch(Mapping functionIdentifier, Args&&... args);

        /**
         * @brief Dispatches the function specified by the mapping to the generalized dispatcher
         * The function will be wrapped in a try catch and if an exception is thrown it
         * will be cause and e.what() will be returned as the unexpected output
         * 
         * @tparam Args variadic args to function 
         * @param fn Function specified by an annotaton
         * @param args Arguments 
         * @return std::expected<R, std::string>  Expected containing the result or error if an error was thrown. 
         */
        template <typename... Args>
        std::expected<R, std::string> try_dispatch(Mapping fn, Args&&... args);

        template <typename... Args>
        std::function<R(Args...)> curry(Mapping fn);

        private:
        consteval std::span<const std::meta::info> registeredFunctions();
    };

}

template <typename Registration, typename R,  class Mapping,  std::meta::info Namespace>
consteval std::span<const std::meta::info> eaf::FunctionDispatcher<Registration, R, Mapping, Namespace>::registeredFunctions() {
    constexpr auto ctx = std::meta::access_context::unprivileged();
    return std::define_static_array(std::meta::members_of(Namespace, ctx)
                                   | std::views::filter(std::meta::is_function)
                                   | std::views::filter([](std::meta::info func) consteval -> bool {
                                        return std::meta::return_type_of(func) == ^^R; })
                                   | std::views::filter([](std::meta::info func) consteval -> bool {
                                        return !std::meta::annotations_of_with_type(func, ^^Registration).empty(); })
    );
}

template <typename Registration, typename R, class Mapping, std::meta::info Namespace>
template <typename... Args>
R eaf::FunctionDispatcher<Registration,R,  Mapping, Namespace>::dispatch(std::string_view functionName, Args&&... args){
    // Iterate all the functions in the namespace that have the correct annotations
    template for (constexpr auto func : registeredFunctions()){
        if (functionName == std::meta::identifier_of(func)){
            return [:func:](std::forward<Args>(args)...);
        }
    }
    throw std::invalid_argument(std::format("No registered function named {}", functionName));
}

template <typename Registration, typename R, class Mapping, std::meta::info Namespace>
template <typename... Args>
R eaf::FunctionDispatcher<Registration, R,  Mapping, Namespace>::dispatch(Mapping functionIdentifier, Args&&... args){
    // Iterate all the functions in the namespace that have the correct annotations
    template for (constexpr auto func : registeredFunctions()){
        template for (constexpr auto mapping : std::define_static_array(std::meta::annotations_of_with_type(func, ^^Mapping))){
            if ([:std::meta::constant_of(mapping):] == functionIdentifier){
                return [:func:](std::forward<Args>(args)...);
            }
        }
    }
    throw std::invalid_argument(std::format("No registered function annotated with the specified mapping {}", std::meta::display_string_of(^^functionIdentifier)));
}

template <typename Registration, typename R, class Mapping, std::meta::info Namespace>
template <typename... Args>
std::expected<R, std::string> eaf::FunctionDispatcher<Registration, R, Mapping, Namespace>::try_dispatch(Mapping functionIdentifier, Args&&... args){
    try {
        return std::expected<R, std::string>(dispatch(functionIdentifier, std::forward<Args>(args)...));
    } catch(const std::exception& e) {
        return std::unexpected(e.what());
    }
}

// Curry? How to turn into a functor? 
template <typename Registration, typename R, class Mapping, std::meta::info Namespace>
template <typename... Args>
std::function<R(Args...)> eaf::FunctionDispatcher<Registration, R, Mapping, Namespace>::curry(Mapping fn){
    return [this, fn](Args... args){return dispatch(fn, std::forward<Args>(args)...);};
}
