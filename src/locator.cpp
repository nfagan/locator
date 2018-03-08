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
    m_labels.seek_tail_to_start();
}

util::locator::~locator() noexcept
{
    //
}

//  copy-construct
util::locator::locator(const util::locator& other) :
    m_random_engine(other.m_random_engine),
    m_labels(other.m_labels),
    m_categories(other.m_categories),
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
    m_categories(std::move(rhs.m_categories)),
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
    m_categories = std::move(rhs.m_categories);
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

bool util::locator::categories_match(const util::locator &other) const
{
    return m_categories.eq_contents(other.m_categories, m_categories.tail());
}

bool util::locator::labels_match(const util::locator &other) const
{
    return m_labels.eq_contents(other.m_labels, m_n_labels);
}

bool util::locator::operator !=(const util::locator &other) const
{
    return !(util::locator::operator ==(other));
}

bool util::locator::operator ==(const util::locator &other) const
{
    if (this == &other)
    {
        return true;
    }
    
    const uint32_t sz = size();
    
    if (sz != other.size())
    {
        return false;
    }
    
    if (!categories_match(other))
    {
        return false;
    }
    
    if (!labels_match(other))
    {
        return false;
    }
    
    //  otherwise, we have to loop through all the indices to compare
    uint32_t* lab_ptr = m_labels.unsafe_get_pointer();
    
    util::bit_array self_copy(sz, false);
    
    for (uint32_t i = 0; i < m_n_labels; i++)
    {
        uint32_t label = lab_ptr[i];
        
        const util::bit_array& self_ref = m_indices.at(label);
        const util::bit_array& other_ref = other.m_indices.at(label);
        
        util::bit_array::unchecked_dot_eq(self_copy, self_ref, other_ref, 0, sz);
        
        if (!self_copy.all())
        {
            return false;
        }
        
        self_copy.fill(false);
    }
    
    return true;
}

uint32_t util::locator::which_category(uint32_t label, bool *exists) const
{
    uint32_t category = 0u;
    
    auto it = m_in_category.find(label);
    
    if (it == m_in_category.end())
    {
        *exists = false;
        return category;
    }
    
    *exists = true;
    
    return it->second;
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
        m_indices[lab].fill(false);
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
    
    bool is_present = has_label(label);
    
    //  make sure we're not trying to add a label
    //  that already exists in another category
    if (is_present)
    {
        uint32_t c_in_category = m_in_category[label];
        
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
        
        m_indices[label] = index;
        
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
            m_indices[lab] = index;
            continue;
        }
        
        util::bit_array& lab_index = m_indices[lab];
        
        util::bit_array::unchecked_dot_and_not(lab_index, lab_index, index, 0, lab_index.size());
    }
    
    if (!is_present)
    {
        m_labels.push(label);
        m_in_category[label] = category;
        m_indices[label] = index;
        by_category.push(label);
        
        m_n_labels++;
        
        by_category.sort();
        m_labels.sort();
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
    auto it = m_indices.begin();
    
    while (it != m_indices.end())
    {
        if (it->second.any())
        {
            it++;
            continue;
        }
        
        uint32_t lab = it->first;
        uint32_t cat = m_in_category[lab];
        
        util::types::entries_t& by_category = m_by_category[cat];
        uint32_t* by_category_ptr = by_category.unsafe_get_pointer();
        
        uint32_t idx_in_by_category;
        
        util::unchecked_binary_search(by_category_ptr, by_category.tail(), lab, &idx_in_by_category);
        
        by_category.erase(idx_in_by_category);
        m_labels.erase(find_label(lab));
        m_in_category.erase(lab);
        it = m_indices.erase(it);
        
        m_n_labels--;
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

void util::locator::unchecked_keep(const util::types::entries_t& at_indices, int32_t index_offset)
{
    for (auto& it : m_indices)
    {
        it.second.unchecked_keep(at_indices, index_offset);
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

util::types::numeric_indices_t util::locator::find(const uint32_t label, uint32_t index_offset)
{
    util::types::numeric_indices_t empty_result;
    
    if (!has_label(label))
    {
        return empty_result;
    }
    
    const util::bit_array& index = m_indices[label];
    
    return util::bit_array::find(index, index_offset);
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
    
    for (uint32_t i = 0; i < search_size; i++)
    {
        uint32_t label = search_label_ptr[i];
        
        if (!has_label(label))
        {
            return empty_result;
        }
        
        const uint32_t category = m_in_category[label];
        const util::bit_array& label_index = m_indices[label];
        
        if (index_map.find(category) == index_map.end())
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
    return is_empty() ? 0 : m_indices.begin()->second.size();
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
    return m_in_category.find(label) != m_in_category.end();
}

bool util::locator::has_category(uint32_t category) const
{
    return m_by_category.find(category) != m_by_category.end();
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
