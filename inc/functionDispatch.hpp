#pragma once
#include <meta>
#include <expected>
#include <print>
#include <utility>
#include <ranges>
#include <functional>

namespace eaf {

    /**
     * @brief Abstract function dispatcher class. 
     * @details Gets all the functions in the provided namespace \ref Namespace that have an annotation of type  \ref Registration. Also
     * registers all annotations with type \ref Mapping to map from the dispatch to the underlying function. 
     * \warning All functions must have the same function signature specified in the \ref Signature template parameter. 
     * 
     * @tparam Registration  Type of annotation required to register to this specific dispatcher instance
     * @tparam Mapping Type of the mapping class. All functions annotated must be called by their mapping, not by function name
     * @tparam Namespace Namespace to look for annotated functions. 
     * @tparam Sig Function signature of all registered functions. 
     */
    template <typename Registration, class Mapping, std::meta::info Namespace,  class Signature>
    class FunctionDispatcher;

    template <typename Registration, class Mapping, std::meta::info Namespace, typename R, typename... Args>
    class FunctionDispatcher<Registration, Mapping, Namespace, R(Args...)> {

        public:
        FunctionDispatcher() = default;

        /**
         * @brief Calls the function specified by functionName with supplied args. 
         * 
         * @param functionName Function to call. The function name is the raw function name with no :: or namespace qualifications
         * @param args Arguments to pass into the function
         * @return R Return value of the function
         */
        R dispatch(std::string_view functionName, Args&&... args);

        /**
         * @brief Dispatches the function specified by the mapping to the generalized dispatcher with supplied arguments
         * 
         * @param functionIdentifier Identifier of the function. The function called with this parameter must
         * have an annotation with the identifier in order for the dispatcher to register it
         * @param args Arguments to pass into the function
         * @return R Return value of the function
         */
        R dispatch(Mapping functionIdentifier, Args&&... args);

        /**
         * @brief Dispatches the function specified by the mapping to the generalized dispatcher
         * The function will be wrapped in a try catch and if an exception is thrown it
         * will be cause and e.what() will be returned as the unexpected output
         * 
         * @param fn Function specified by an annotaton
         * @param args Arguments to pass into the function
         * @return std::expected<R, std::string>  Expected containing the result or error if an error was thrown. 
         * This dispatch wraps the function call in a try-catch and handles exception based error handling rather than
         * error codes or other functions that call std::expected
         */
        std::expected<R, std::string> try_dispatch(Mapping fn, Args&&... args);

        /**
         * @brief Returns the underlying function that is specified by the mapping. This function does partial application 
         * of the dispatch member function, aka Currying
         * 
         * @param fn Function mapping to return a function to
         * @return std::function<R(Args...)> Function object that will call the underlying function. 
         */
        std::function<R(Args...)> operator()(Mapping fn);

        private:
        consteval std::span<const std::meta::info> registeredFunctions();
    };

}

template <typename Registration, class Mapping,  std::meta::info Namespace, typename R, typename... Args>
consteval std::span<const std::meta::info> eaf::FunctionDispatcher<Registration, Mapping, Namespace, R(Args...)>::registeredFunctions() {
    constexpr auto ctx = std::meta::access_context::unprivileged();
    return std::define_static_array(std::meta::members_of(Namespace, ctx)
                                   | std::views::filter(std::meta::is_function)
                                   | std::views::filter([](std::meta::info func) consteval -> bool {
                                        return std::meta::return_type_of(func) == ^^R; })
                                   | std::views::filter([](std::meta::info func) consteval -> bool {
                                        return !std::meta::annotations_of_with_type(func, ^^Registration).empty(); })
    );
}

template <typename Registration, class Mapping,  std::meta::info Namespace, typename R, typename... Args>
R eaf::FunctionDispatcher<Registration, Mapping, Namespace, R(Args...)>::dispatch(std::string_view functionName, Args&&... args){
    // Iterate all the functions in the namespace that have the correct annotations
    template for (constexpr auto func : registeredFunctions()){
        if (functionName == std::meta::identifier_of(func)){
            return [:func:](std::forward<Args>(args)...);
        }
    }
    throw std::invalid_argument(std::format("No registered function named {}", functionName));
}

template <typename Registration, class Mapping,  std::meta::info Namespace, typename R, typename... Args>
R eaf::FunctionDispatcher<Registration, Mapping, Namespace, R(Args...)>::dispatch(Mapping functionIdentifier, Args&&... args){
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

template <typename Registration, class Mapping,  std::meta::info Namespace, typename R, typename... Args>
std::expected<R, std::string> eaf::FunctionDispatcher<Registration, Mapping, Namespace, R(Args...)>::try_dispatch(Mapping functionIdentifier, Args&&... args){
    try {
        return std::expected<R, std::string>(dispatch(functionIdentifier, std::forward<Args>(args)...));
    } catch(const std::exception& e) {
        return std::unexpected(e.what());
    }
}

template <typename Registration, class Mapping,  std::meta::info Namespace, typename R, typename... Args>
std::function<R(Args...)> eaf::FunctionDispatcher<Registration, Mapping, Namespace, R(Args...)>::operator()(Mapping fn){
    return [this, fn](Args... args){return dispatch(fn, std::forward<Args>(args)...);};
}
