#pragma once

#include <unordered_map>
#include <string>

namespace util {
    namespace ops 
    {
        constexpr uint32_t CREATE = 0u;
        constexpr uint32_t DESTROY = 1u;
        constexpr uint32_t ADD_CATEGORY = 2u;
        constexpr uint32_t SET_CATEGORY = 3u;
        constexpr uint32_t GET_CATEGORIES = 4u;
        constexpr uint32_t GET_LABELS = 5u;
        constexpr uint32_t FIND = 6u;
        constexpr uint32_t HAS_CATEGORY = 7u;
        constexpr uint32_t IS_LOCATOR = 8u;
        constexpr uint32_t REQUIRE_CATEGORY = 9u;
        constexpr uint32_t SIZE = 10u;
        constexpr uint32_t KEEP = 11u;
        constexpr uint32_t COPY = 12u;
        constexpr uint32_t INSTANCES = 13u;
        constexpr uint32_t RM_CATEGORY = 14u;
        constexpr uint32_t WHICH_CATEGORY = 15u;
        constexpr uint32_t HAS_LABEL = 16u;
        constexpr uint32_t EQUALS = 17u;
        constexpr uint32_t APPEND = 18u;
    };
    
    typedef std::unordered_map<std::string, uint32_t> op_map_t;
}