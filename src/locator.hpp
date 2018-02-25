//
//  locator.hpp
//  locator
//
//  Created by Nick Fagan on 2/24/18.
//

#pragma once

#include <dynamic_array.hpp>
#include <bit_array.hpp>
#include <cstdint>
#include <vector>

namespace util {
    class locator;
    
    namespace locator_status {
        enum locator_status
        {
            OK,
            CATEGORY_EXISTS,
            CATEGORY_DOES_NOT_EXIST,
            LABEL_EXISTS
        };
    }
}

class util::locator
{
    typedef util::dynamic_array<uint32_t> entries_t;
    typedef util::dynamic_array<uint32_t> numeric_indices_t;
    
public:
    
    locator();
    ~locator() noexcept;
    
    uint32_t add_category(uint32_t category);
    uint32_t add_label(uint32_t label, uint32_t in_category, const util::bit_array& index);
    
    bool has_category(uint32_t category) const;
    bool has_label(uint32_t label) const;
    
    numeric_indices_t locate(uint32_t* labels, uint32_t n_labels);
private:
    entries_t m_labels;
    entries_t m_categories;
    entries_t m_in_category;
    entries_t m_by_category;
    std::vector<util::bit_array> m_indices;
    
    uint32_t find_category(uint32_t category, bool* was_found) const;
    uint32_t find_label(uint32_t label, bool* was_found) const;
};
