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
#include <unordered_map>

namespace util {
    class locator;
    
    namespace types {
        typedef util::dynamic_array<uint32_t> entries_t;
        typedef util::dynamic_array<uint32_t> numeric_indices_t;
    }
    
    struct locator_status {
        static constexpr uint32_t OK = 0u;
        static constexpr uint32_t CATEGORY_EXISTS = 1u;
        static constexpr uint32_t CATEGORY_DOES_NOT_EXIST = 2u;
        static constexpr uint32_t LABEL_EXISTS = 3u;
        static constexpr uint32_t WRONG_INDEX_SIZE = 4u;
    };
}

class util::locator
{
    
public:
    locator();
    ~locator() noexcept;
    
    uint32_t add_category(uint32_t category);
    uint32_t add_label(uint32_t label, uint32_t in_category, const util::bit_array& index);
    
    bool has_category(uint32_t category) const;
    bool has_label(uint32_t label) const;
    
    types::entries_t get_labels() const;
    types::entries_t get_categories() const;
    
    bool is_empty() const;
    uint32_t size() const;
    uint32_t n_labels() const;
    uint32_t n_categories() const;
    
    types::numeric_indices_t locate(const types::entries_t& labels);
    
private:
    types::entries_t m_labels;
    types::entries_t m_categories;
    types::entries_t m_in_category;
    std::unordered_map<uint32_t, types::entries_t> m_by_category;
    std::vector<util::bit_array> m_indices;
    util::bit_array m_tmp_index;
    
    uint32_t m_n_categories;
    uint32_t m_n_labels;
    
    uint32_t find_category(uint32_t category, bool* was_found) const;
    uint32_t find_label(uint32_t label, bool* was_found) const;
};
