//
//  locator.hpp
//  locator
//
//  Created by Nick Fagan on 2/24/18.
//

#pragma once

#include "dynamic_array.hpp"
#include "bit_array.hpp"
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <functional>
#include <random>

namespace util {
    class locator;
    
    namespace types {
        typedef util::dynamic_array<uint32_t> entries_t;
        typedef util::dynamic_array<uint32_t> numeric_indices_t;
        typedef util::dynamic_array<util::bit_array, util::dynamic_allocator<util::bit_array>> indices_t;
    }
    
    struct locator_status {
        static constexpr uint32_t OK = 0u;
        static constexpr uint32_t CATEGORY_EXISTS = 1u;
        static constexpr uint32_t CATEGORY_DOES_NOT_EXIST = 2u;
        static constexpr uint32_t LABEL_EXISTS = 3u;
        static constexpr uint32_t WRONG_INDEX_SIZE = 4u;
        static constexpr uint32_t INDEX_OUT_OF_BOUNDS = 5u;
        static constexpr uint32_t LABEL_EXISTS_IN_OTHER_CATEGORY = 6u;
    };
}

class util::locator
{
    
public:
    locator();
    locator(uint32_t n_labels_hint);
    
    locator(const locator& other);
    locator& operator=(const locator& other);
    locator(locator&& rhs) noexcept;
    locator& operator=(locator&& other) noexcept;
    
    ~locator() noexcept;
    
    void unchecked_keep(const util::types::entries_t& at_indices);
    uint32_t keep(const util::types::entries_t& at_indices);
    
    void clear();
    void empty();
    
    uint32_t add_category(uint32_t category);
    uint32_t set_category(uint32_t category, uint32_t label, const util::bit_array& index);
    uint32_t rm_category(uint32_t category);
    
    uint32_t require_category(uint32_t category);
    
    bool has_label(uint32_t label) const;
    bool has_category(uint32_t category) const;
    
    const types::entries_t& get_labels() const;
    const types::entries_t& get_categories() const;
    
    bool is_empty() const;
    
    uint32_t size() const;
    uint32_t n_labels() const;
    uint32_t n_categories() const;
    
    types::entries_t combinations(const types::entries_t& categories) const;
    
    types::numeric_indices_t find(const types::entries_t& labels, uint32_t index_offset = 0u);
    types::numeric_indices_t find(const uint32_t label, uint32_t index_offset = 0u);
    
    types::numeric_indices_t find_all(const types::entries_t &categories);
    
private:
    std::mt19937 m_random_engine;
    
    types::entries_t m_labels;
    types::entries_t m_categories;
    std::unordered_map<uint32_t, uint32_t> m_in_category;
    std::unordered_map<uint32_t, types::entries_t> m_by_category;
    std::unordered_map<uint32_t, util::bit_array> m_indices;
    util::bit_array m_tmp_index;
    uint32_t m_n_labels;
    
    void prune();
    
    uint32_t find_category(uint32_t category, bool* was_found) const;
    uint32_t find_category(uint32_t category) const;
    uint32_t find_label(uint32_t label, bool* was_found) const;
    uint32_t find_label(uint32_t label) const;
    
    uint32_t get_random_category_id();
    uint32_t get_random_label_id();
    uint32_t get_random_id(std::function<bool(const util::locator*, uint32_t)> exists_func);
    
    void unchecked_add_category(uint32_t category);
};
