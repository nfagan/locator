#pragma once

#include <unordered_map>
#include <string>
#include <cstdint>

namespace util {
    
    namespace ops 
    {
        constexpr uint32_t CREATE =               0u;
        constexpr uint32_t DESTROY =              1u;
        constexpr uint32_t INSERT =               2u;
        constexpr uint32_t AT =                   3u;
        constexpr uint32_t INSTANCES =            4u;
        constexpr uint32_t KEYS =                 5u;
        constexpr uint32_t VALUES =               6u;
        constexpr uint32_t CONTAINS =             7u;
        constexpr uint32_t COPY =                 8u;
        //  how many ops
        constexpr uint32_t N_OPS =                9u;
    };
    
    typedef std::unordered_map<std::string, uint32_t> op_map_t;
}