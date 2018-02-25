//
//  locator.hpp
//  locator
//
//  Created by Nick Fagan on 2/24/18.
//

#pragma once

#include <dynamic_array.hpp>
#include <bit_array.hpp>
#include <utilities.hpp>
#include <cstdint>
#include <vector>

namespace util {
    class locator;
}

class util::locator
{
public:
    locator();
    ~locator() noexcept;
    
private:
    util::dynamic_array<uint32_t> m_labels;
    util::dynamic_array<uint32_t> m_categories;
    util::dynamic_array<uint32_t> m_in_category;
    util::dynamic_array<uint32_t> m_by_category;
    std::vector<util::bit_array> m_indices;
};
