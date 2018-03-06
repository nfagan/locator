//
//  locator.cpp
//  locator
//
//  Created by Nick Fagan on 2/24/18.
//

#include "locator.hpp"
#include "utilities.hpp"
#include <config.hpp>
#include <algorithm>
#include <iostream>
#include <chrono>
#include <cassert>

util::locator::locator() : m_random_engine(std::random_device()())
{
    m_n_labels = 0;
}

util::locator::locator(uint32_t n_labels_hint) : m_random_engine(std::random_device()())
{
    m_n_labels = 0;
    
    m_labels.resize(n_labels_hint);
    m_in_category.resize(n_labels_hint);
    m_indices.resize(n_labels_hint);
    
    m_labels.seek_tail_to_start();
    m_in_category.seek_tail_to_start();
    m_indices.seek_tail_to_start();
}

util::locator::~locator() noexcept
{
    //
}

//  copy-construct
util::locator::locator(const util::locator& other) :
    m_random_engine(other.m_random_engine),
    m_labels(other.m_labels),
    m_in_category(other.m_in_category),
    m_by_category(other.m_by_category),
    m_indices(other.m_indices),
    m_tmp_index(other.m_tmp_index)
{
    m_n_labels = other.m_n_labels;
}

//  copy-assign
util::locator& util::locator::operator=(const util::locator& other)
{
    util::locator tmp(other);
    *this = std::move(tmp);
    return *this;
}

//  move-construct
util::locator::locator(util::locator&& rhs) noexcept :
    m_random_engine(std::move(rhs.m_random_engine)),
    m_labels(std::move(rhs.m_labels)),
    m_in_category(std::move(rhs.m_in_category)),
    m_by_category(std::move(rhs.m_by_category)),
    m_indices(std::move(rhs.m_indices)),
    m_tmp_index(std::move(rhs.m_tmp_index))
{
    m_n_labels = rhs.m_n_labels;
    rhs.m_n_labels = 0;
}

//  move-assign
util::locator& util::locator::operator=(util::locator&& rhs) noexcept
{
    m_random_engine = std::move(rhs.m_random_engine);
    m_labels = std::move(rhs.m_labels);
    m_in_category = std::move(rhs.m_in_category);
    m_by_category = std::move(rhs.m_by_category);
    m_indices = std::move(rhs.m_indices);
    m_tmp_index = std::move(rhs.m_tmp_index);
    m_n_labels = rhs.m_n_labels;
    
    rhs.m_n_labels = 0;
    
    return *this;
}

const util::types::entries_t& util::locator::get_labels() const
{
    return m_labels;
}

const util::types::entries_t& util::locator::get_categories() const
{
    return m_categories;
}

uint32_t util::locator::add_category(uint32_t category)
{
    if (has_category(category))
    {
        return util::locator_status::CATEGORY_EXISTS;
    }
    
    unchecked_add_category(category);
    
    return util::locator_status::OK;
}

uint32_t util::locator::require_category(uint32_t category)
{
    if (!has_category(category))
    {
        unchecked_add_category(category);
    }
    
    return util::locator_status::OK;
}

void util::locator::unchecked_add_category(uint32_t category)
{
    m_categories.push(category);
    m_by_category[category] = util::types::entries_t();
    
    m_categories.unchecked_sort(m_categories.tail());
}

uint32_t util::locator::rm_category(uint32_t category)
{
    if (!has_category(category))
    {
        return util::locator_status::CATEGORY_DOES_NOT_EXIST;
    }
    
    util::types::entries_t& by_category = m_by_category[category];
    
    uint32_t n_in_cat = by_category.tail();
    uint32_t* by_category_ptr = by_category.unsafe_get_pointer();
    
    for (uint32_t i = 0; i < n_in_cat; i++)
    {
        uint32_t lab = by_category_ptr[i];
        uint32_t lab_idx = find_label(lab);
        
        util::bit_array& ind = m_indices.ref_at(lab_idx);
        
        ind.fill(false);
    }
    
    prune();
    
    m_by_category.erase(category);
    
    uint32_t in_cat_idx = find_category(category);
    
    m_categories.erase(in_cat_idx);
    
    return util::locator_status::OK;
}

uint32_t util::locator::set_category(uint32_t category, uint32_t label, const util::bit_array& index)
{
    if (!has_category(category))
    {
        return util::locator_status::CATEGORY_DOES_NOT_EXIST;
    }
    
    bool is_present;
    
    uint32_t insert_at = find_label(label, &is_present);
    
    util::bit_array* indices_ptr = m_indices.unsafe_get_pointer();
    
    uint32_t* m_in_category_ptr = m_in_category.unsafe_get_pointer();
    
    //  make sure we're not trying to add a label
    //  that already exists in another category
    if (is_present)
    {
        uint32_t c_in_category = m_in_category_ptr[insert_at];
        
        if (c_in_category != category)
        {
            return util::locator_status::LABEL_EXISTS_IN_OTHER_CATEGORY;
        }
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
    
    //  if no elements are true, we can just return early, unless
    //  we're assigning false to a present label
    if (!index.any())
    {
        if (!is_present)
        {
            return util::locator_status::OK;
        }
        
        indices_ptr[insert_at] = index;
        
        prune();
        
        return util::locator_status::OK;
    }
    
    util::types::entries_t& by_category = m_by_category[category];
    
    uint32_t* by_category_ptr = by_category.unsafe_get_pointer();
    uint32_t n_by_category = by_category.tail();
    
    //  set false at rows of other indices
    for (uint32_t i = 0; i < n_by_category; i++)
    {
        uint32_t lab = by_category_ptr[i];
        
        if (lab == label)
        {
            indices_ptr[insert_at] = index;
            continue;
        }
        
        uint32_t lab_loc = find_label(lab);
        
        util::bit_array& lab_index = m_indices.ref_at(lab_loc);
        
        util::bit_array::unchecked_dot_and_not(lab_index, lab_index, index, 0, lab_index.size());
    }
    
    if (!is_present)
    {
        m_labels.insert(label, insert_at);
        m_in_category.insert(category, insert_at);
        m_indices.insert(index, insert_at);
        by_category.push(label);
        
        m_n_labels++;
        
        by_category.sort();
    }
    
    if (c_is_empty)
    {
        m_tmp_index = util::bit_array(index.size(), false);
    }
    
    if (m_n_labels > 1)
    {
        prune();
    }
    
    return util::locator_status::OK;
}

void util::locator::prune()
{
    uint32_t* category_ptr = m_in_category.unsafe_get_pointer();
    uint32_t* label_ptr = m_labels.unsafe_get_pointer();
    
    int32_t idx_offset = 0;
    int32_t i = 0;
    
    while (i < m_n_labels)
    {
        int32_t full_idx = i + idx_offset;
        
        if (m_indices.ref_at(full_idx).any())
        {
            i++;
            continue;
        }
        
        uint32_t category = category_ptr[full_idx];
        uint32_t label = label_ptr[full_idx];
        
        types::entries_t& by_category = m_by_category[category];
        uint32_t* by_category_ptr = by_category.unsafe_get_pointer();
        
        uint32_t index_in_by_category;
        
        util::unchecked_binary_search(by_category_ptr, by_category.tail(), label, &index_in_by_category);
        
        by_category.erase(index_in_by_category);
        m_indices.erase(full_idx);
        m_labels.erase(full_idx);
        m_in_category.erase(full_idx);
        
        m_n_labels--;
        
        idx_offset -= 1;
        
        i++;
    }
}

uint32_t util::locator::keep(const util::types::entries_t& at_indices)
{
    if (is_empty())
    {
        return util::locator_status::OK;
    }
    
    uint32_t n_indices = at_indices.tail();
    
    if (n_indices == 0)
    {
        empty();
        return util::locator_status::OK;
    }
    
    uint32_t sz = size();
    uint32_t* at_indices_ptr = at_indices.unsafe_get_pointer();
    
    for (uint32_t i = 0; i < n_indices; i++)
    {
        if (at_indices_ptr[i] >= sz)
        {
            return util::locator_status::INDEX_OUT_OF_BOUNDS;
        }
    }
    
    unchecked_keep(at_indices);
    
    return util::locator_status::OK;
}

void util::locator::unchecked_keep(const util::types::entries_t &at_indices)
{
    for (uint32_t i = 0; i < m_n_labels; i++)
    {
        m_indices.ref_at(i).unchecked_keep(at_indices);
    }
    
    m_tmp_index.unchecked_keep(at_indices);
    
    prune();
}

void util::locator::clear()
{
    m_labels.clear();
    m_in_category.clear();
    m_categories.clear();
    m_indices.clear();
    m_by_category.clear();
    m_tmp_index.empty();
    
    m_n_labels = 0;
}

void util::locator::empty()
{
    m_labels.clear();
    m_in_category.clear();
    m_indices.clear();
    m_tmp_index.empty();
    
    for (auto& it : m_by_category)
    {
        it.second.clear();
    }
    
    m_n_labels = 0;
}

util::types::numeric_indices_t util::locator::find(uint32_t label, uint32_t index_offset) const
{
    util::types::numeric_indices_t empty_result;
    
    bool was_found;
    uint32_t label_idx = find_label(label, &was_found);
    
    if (!was_found)
    {
        return empty_result;
    }
    
    return util::bit_array::find(m_indices.ref_at(label_idx), index_offset);
}

util::types::numeric_indices_t util::locator::find(const util::types::entries_t& labels, uint32_t index_offset)
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
        const util::bit_array& label_index = m_indices.ref_at(in_labels_idx);
        
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
    
    return bit_array::find(m_tmp_index, index_offset);
}

bool util::locator::is_empty() const
{
    return m_n_labels == 0;
}

uint32_t util::locator::size() const
{
    return is_empty() ? 0 : m_indices.ref_at(0).size();
}

uint32_t util::locator::n_categories() const
{
    return m_categories.tail();
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
    return m_by_category.find(category) != m_by_category.end();
//    bool was_found;
//    find_category(category, &was_found);
//    return was_found;
}

uint32_t util::locator::find_label(uint32_t label, bool *was_found) const
{
    uint32_t idx;
    uint32_t* labels = m_labels.unsafe_get_pointer();
    
    util::unchecked_binary_search(labels, m_n_labels, label, was_found, &idx);
    
    return idx;
}

uint32_t util::locator::find_label(uint32_t label) const
{
    bool dummy;
    return find_label(label, &dummy);
}

uint32_t util::locator::find_category(uint32_t category, bool *was_found) const
{
    uint32_t idx;
    uint32_t* categories = m_categories.unsafe_get_pointer();
    
    util::unchecked_binary_search(categories, m_categories.tail(), category, was_found, &idx);
    
    return idx;
}

uint32_t util::locator::find_category(uint32_t category) const
{
    bool dummy;
    return find_category(category, &dummy);
}

uint32_t util::locator::get_random_label_id()
{
    return get_random_id(&util::locator::has_label);
}

uint32_t util::locator::get_random_category_id()
{
    return get_random_id(&util::locator::has_category);
}

uint32_t util::locator::get_random_id(std::function<bool(const util::locator*, uint32_t)> exists_func)
{
    uint32_t int_max = ~(uint32_t(0));
    std::uniform_int_distribution<uint32_t> uniform_dist(0, int_max);
    
    uint32_t id = uniform_dist(m_random_engine);
    
    //  shouldn't happen (tm)
    if (size() == int_max)
    {
        return 0u;
    }
    
    while (exists_func(this, id))
    {
        id = uniform_dist(m_random_engine);
    }
    
    return id;
}
