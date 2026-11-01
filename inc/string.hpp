/**
 * @brief Defines a string that is compatible with eaf factory and dispatcher
 * 
 */

#pragma once
#include <string_view>
#include <meta>
#include <cstring>

namespace eaf{
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
}
