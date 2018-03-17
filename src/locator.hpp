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
        using entries_t = util::dynamic_array<uint32_t>;
        using numeric_indices_t = util::dynamic_array<uint32_t>;
        using arr_entries_t = util::dynamic_array<entries_t, util::dynamic_allocator<entries_t>>;
        
        struct find_all_return_t {
            entries_t combinations;
            arr_entries_t indices;
        };
    }
    
    struct locator_status {
        static constexpr uint32_t OK = 0u;
        static constexpr uint32_t CATEGORY_EXISTS = 1u;
        static constexpr uint32_t CATEGORY_DOES_NOT_EXIST = 2u;
        static constexpr uint32_t LABEL_EXISTS = 3u;
        static constexpr uint32_t WRONG_INDEX_SIZE = 4u;
        static constexpr uint32_t INDEX_OUT_OF_BOUNDS = 5u;
        static constexpr uint32_t LABEL_EXISTS_IN_OTHER_CATEGORY = 6u;
        static constexpr uint32_t CATEGORIES_DO_NOT_MATCH = 7u;
        static constexpr uint32_t LOC_OVERFLOW = 8u;
        static constexpr uint32_t WRONG_NUMBER_OF_INDICES = 9u;
    };
    
    uint32_t get_random_id(std::function<bool(const util::locator*, uint32_t)> exists_func, const util::locator* loc);
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
    
    bool operator ==(const util::locator& other) const;
    bool operator !=(const util::locator& other) const;
    
    void unchecked_keep(const util::types::entries_t& at_indices, int32_t index_offset = 0);
    uint32_t keep(const util::types::entries_t& at_indices);
    
    void clear();
    void empty();
    void resize(uint32_t to_size);
    
    uint32_t add_category(uint32_t category);
    uint32_t set_category(uint32_t category, uint32_t label, const util::bit_array& index);
    uint32_t set_category(uint32_t category, const types::entries_t& labels, const util::bit_array& index);
    uint32_t rm_category(uint32_t category);
    uint32_t require_category(uint32_t category);
    uint32_t collapse_category(uint32_t category);
    uint32_t which_category(uint32_t label, bool* exists) const;
    types::entries_t all_in_category(uint32_t category, bool* exists) const;
    types::entries_t full_category(uint32_t category, bool* exists) const;
    
    bool has_label(uint32_t label) const;
    bool has_category(uint32_t category) const;
    bool is_full_category(uint32_t category, bool* exists) const;
    
    const types::entries_t& get_labels() const;
    const types::entries_t& get_categories() const;
    
    bool is_empty() const;
    
    uint32_t size() const;
    uint32_t n_labels() const;
    uint32_t n_categories() const;
    
    uint32_t count(uint32_t label) const;
    
    bool categories_match(const util::locator& other) const;
    bool labels_match(const util::locator& other) const;
    
    uint32_t append(const util::locator& other);
    
    types::entries_t combinations(const types::entries_t& categories, bool* exist) const;
    
    types::numeric_indices_t find(const types::entries_t& labels, uint32_t index_offset = 0u);
    types::numeric_indices_t find(const uint32_t label, uint32_t index_offset = 0u) const;
    types::find_all_return_t find_all(const types::entries_t& categories,
                                      bool* exist, uint32_t index_offset = 0u) const;
    
    uint32_t get_random_label_id() const;
private:
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
    
    uint32_t get_random_category_id() const;
    
    void unchecked_add_category(uint32_t category);
    void unchecked_set_category(uint32_t category, uint32_t label, bool is_present, bool create_tmp, const util::bit_array& index);
    void unchecked_full_category(uint32_t* labs, uint32_t n_labs, uint32_t* out, uint32_t offset) const;
};
