//
//  locator.cpp
//  locator
//
//  Created by Nick Fagan on 2/24/18.
//

#include "locator.hpp"
#include <utilities.hpp>
#include <algorithm>
#include <iostream>

util::locator::locator()
{
    m_n_categories = 0;
    m_n_labels = 0;
}

util::locator::~locator() noexcept
{
    
}

util::types::entries_t util::locator::get_labels() const
{
    return m_labels;
}

util::types::entries_t util::locator::get_categories() const
{
    return m_categories;
}

uint32_t util::locator::add_category(uint32_t category)
{
    if (has_category(category))
    {
        return util::locator_status::CATEGORY_EXISTS;
    }
    
    m_categories.push(category);
    m_by_category[category] = util::types::entries_t();
    m_n_categories++;
    
    m_categories.unchecked_sort(m_n_categories);
    
    return util::locator_status::OK;
}

uint32_t util::locator::add_label(uint32_t label, uint32_t category, const util::bit_array &index)
{
    bool label_exists;
    
    uint32_t insert_at = find_label(label, &label_exists);
    
    if (label_exists)
    {
        return util::locator_status::LABEL_EXISTS;
    }
    
    if (!has_category(category))
    {
        return util::locator_status::CATEGORY_DOES_NOT_EXIST;
    }
    
    uint32_t c_size = size();
    uint32_t index_size_m = index.size();
    
    bool c_is_empty = is_empty();
    
    //  size of incoming index must match current size, unless
    //  locator is empty.
    if (!c_is_empty && index_size_m != c_size)
    {
        return util::locator_status::WRONG_INDEX_SIZE;
    }
    
    util::types::entries_t& by_category = m_by_category[category];

    m_labels.insert(label, insert_at);
    m_in_category.insert(category, insert_at);
    by_category.push(label);
    m_indices.insert(m_indices.begin() + insert_at, std::move(index));
    
    if (c_is_empty)
    {
        m_tmp_index = util::bit_array(index.size());
    }
    
    m_n_labels++;
    
    by_category.sort();
    
    return util::locator_status::OK;
}

util::types::numeric_indices_t util::locator::locate(const util::types::entries_t& labels)
{
    using util::bit_array;
    
    util::types::numeric_indices_t empty_result;
    
    uint32_t c_size = size();
    
    if (m_n_labels == 0 || c_size == 0)
    {
        return empty_result;
    }
    
    std::unordered_map<uint32_t, util::bit_array> index_map;
    
    uint32_t* search_label_ptr = labels.unsafe_get_pointer();
    uint32_t search_size = labels.tail();
    
    uint32_t* category_ptr = m_in_category.unsafe_get_pointer();
    
    bool was_found;
    
    for (uint32_t i = 0; i < search_size; i++)
    {
        uint32_t in_labels_idx = find_label(search_label_ptr[i], &was_found);
        
        if (!was_found)
        {
            return empty_result;
        }
        
        const uint32_t category = category_ptr[in_labels_idx];
        const util::bit_array& label_index = m_indices[in_labels_idx];
        
        if (index_map.count(category) == 0)
        {
            index_map[category] = label_index;
        }
        else
        {
            bit_array& current = index_map[category];
            bit_array::unchecked_dot_or(current, current, label_index, 0, c_size);
        }
    }
    
    m_tmp_index.fill(true);
    
    for (const auto& it : index_map)
    {
        bit_array::unchecked_dot_and(m_tmp_index, m_tmp_index, it.second, 0, c_size);
    }
    
    return bit_array::find(m_tmp_index);
}

bool util::locator::is_empty() const
{
    return m_n_labels == 0;
}

uint32_t util::locator::size() const
{
    return is_empty() ? 0 : m_indices[0].size();
}

uint32_t util::locator::n_categories() const
{
    return m_n_categories;
}

uint32_t util::locator::n_labels() const
{
    return m_n_labels;
}

bool util::locator::has_label(uint32_t label) const
{
    bool was_found;
    find_label(label, &was_found);
    return was_found;
}

bool util::locator::has_category(uint32_t category) const
{
    bool was_found;
    find_category(category, &was_found);
    return was_found;
}

uint32_t util::locator::find_label(uint32_t label, bool *was_found) const
{
    uint32_t idx;
    uint32_t* labels = m_labels.unsafe_get_pointer();
    
    util::unchecked_binary_search(labels, m_n_labels, label, was_found, &idx);
    
    return idx;
}

uint32_t util::locator::find_category(uint32_t category, bool *was_found) const
{
    uint32_t idx;
    uint32_t* categories = m_categories.unsafe_get_pointer();
    
    util::unchecked_binary_search(categories, m_n_categories, category, was_found, &idx);
    
    return idx;
}
