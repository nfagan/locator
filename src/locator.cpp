//
//  locator.cpp
//  locator
//
//  Created by Nick Fagan on 2/24/18.
//

#include "locator.hpp"
#include "utilities.hpp"
#include <algorithm>
#include <iostream>
#include <chrono>
#include <cassert>
#include <string>

#define LOC_COMB_FULL_CAT
#define LOC_FIND_ALL_ONE_CAT_ARRAY

uint32_t util::get_random_id(std::function<bool(uint32_t)> exists_func)
{
    static std::mt19937 random_engine = std::mt19937(std::random_device()());
    
    uint32_t int_max = ~(uint32_t(0));
    std::uniform_int_distribution<uint32_t> uniform_dist(0, int_max);
    
    uint32_t id = uniform_dist(random_engine);
    
    while (exists_func(id) || id == util::locator::UNDEFINED_LABEL)
    {
        id = uniform_dist(random_engine);
    }
    
    return id;
}

util::locator::locator()
{
    m_n_labels = 0;
}

util::locator::locator(uint32_t n_labels_hint)
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
    return m_categories.eq_contents(other.m_categories);
}

bool util::locator::labels_match(const util::locator &other) const
{
    return m_labels.eq_contents(other.m_labels);
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

util::types::entries_t util::locator::all_in_category(uint32_t category, bool *exists) const
{
    auto it = m_by_category.find(category);
    
    if (it == m_by_category.end())
    {
        *exists = false;
        return util::types::entries_t();
    }
    
    *exists = true;
    
    return it->second;
}

util::types::entries_t util::locator::full_category(uint32_t category, bool *exists) const
{
    return full_category(category, get_random_label_id(), exists);
}

util::types::entries_t util::locator::full_category(uint32_t category, uint32_t set_empty_labels, bool *exists) const
{
    auto it = m_by_category.find(category);
    
    if (it == m_by_category.end())
    {
        *exists = false;
        return util::types::entries_t();
    }
    
    *exists = true;
    
    const util::types::entries_t& labs = it->second;
    
    uint32_t n_in_cat = labs.tail();
    
    if (n_in_cat == 0)
    {
        return util::types::entries_t();
    }
    
    uint32_t* labs_ptr = labs.unsafe_get_pointer();
    
    uint32_t sz = size();
    
    util::types::entries_t result(sz);
    uint32_t* result_ptr = result.unsafe_get_pointer();
    
    std::memset(result_ptr, set_empty_labels, sz * sizeof(uint32_t));
    
    for (uint32_t i = 0; i < n_in_cat; i++)
    {
        uint32_t lab = labs_ptr[i];
        auto inds = util::bit_array::find(m_indices.at(lab));
        uint32_t* inds_ptr = inds.unsafe_get_pointer();
        
        uint32_t n_inds = inds.tail();
        
        for (uint32_t j = 0; j < n_inds; j++)
        {
            result_ptr[inds_ptr[j]] = lab;
        }
    }
    
    return result;
}

util::types::find_all_return_t util::locator::find_all(const types::entries_t& categories,
                                                       bool* exist, uint32_t index_offset) const
{
    using namespace util;
    
    types::find_all_return_t result;
    
    uint32_t n_cats_in = categories.tail();
    
    *exist = true;
    
    if (n_cats_in == 0)
    {
        return result;
    }
    
    uint32_t* cat_ptr = categories.unsafe_get_pointer();
    
    types::arr_entries_t full_categories(n_cats_in);
    types::entries_t* full_categories_ptr = full_categories.unsafe_get_pointer();
    
    //  fill empty spaces in categories with this label
    uint32_t undf_lab = util::locator::UNDEFINED_LABEL;
    
    for (uint32_t i = 0; i < n_cats_in; i++)
    {
        full_categories_ptr[i] = full_category(cat_ptr[i], undf_lab, exist);
        
        if (!(*exist))
        {
            return result;
        }
        
        //  if there are no labels in the category, no
        //  combinations can possibly exist
        
        if (full_categories_ptr[i].tail() == 0)
        {
            return result;
        }
    }
    
    uint32_t sz = size();
    
    //  hash based on the sequence of bits formed by
    //  labels in a given category
    size_t size_int = sizeof(uint32_t);
    std::string hash_code(n_cats_in * size_int, 'a');
    char* hash_code_ptr = &hash_code[0];
    
    std::unordered_map<std::string, uint32_t> combination_exists;
    uint32_t next_id = 0;
    
    //  start with room for 2048 combinations (just a guess)
    result.combinations.resize(2048);
    result.combinations.seek_tail_to_start();
    result.indices.resize(2048);
    result.indices.seek_tail_to_start();
    
    for (uint32_t i = 0; i < sz; i++)
    {
        for (uint32_t j = 0; j < n_cats_in; j++)
        {
            uint32_t* full_cat = full_categories_ptr[j].unsafe_get_pointer();
            //  copy bits to string
            std::memcpy(hash_code_ptr + j * size_int, &full_cat[i], size_int);
        }
        
        auto c_it = combination_exists.find(hash_code);
        bool c_exists = c_it != combination_exists.end();
        uint32_t comb_idx;
        
        if (!c_exists)
        {
            for (uint32_t j = 0; j < n_cats_in; j++)
            {
                uint32_t* full_cat = full_categories_ptr[j].unsafe_get_pointer();
                result.combinations.push(full_cat[i]);
            }
            
            combination_exists[hash_code] = next_id;
            
            comb_idx = next_id;
            
            next_id++;
            
            result.indices.push(util::types::entries_t());
        }
        else
        {
            comb_idx = c_it->second;
        }
        
        types::entries_t* inds_ptr = result.indices.unsafe_get_pointer();
        inds_ptr[comb_idx].push(i + index_offset);
    }
    
    return result;
}

//  keep_each: Keep one row for each subset of label combinations.

util::types::find_all_return_t util::locator::keep_each(const types::entries_t& categories, bool *exist, uint32_t index_offset)
{
    util::types::find_all_return_t res = find_all(categories, exist, index_offset);
    
    if (!exist)
    {
        return res;
    }
    
    uint32_t n_indices = res.indices.tail();
    uint32_t total_sz = n_indices;
    uint32_t n_cats_in = categories.tail();
    uint32_t* in_cat_ptr = categories.unsafe_get_pointer();
    
    types::entries_t remaining_categories = m_categories;
    
    //  determine which additional categories we'll have to draw labels from
    for (uint32_t i = 0; i < n_cats_in; i++)
    {
        uint32_t* remaining_cats_ptr = remaining_categories.unsafe_get_pointer();
        
        uint32_t idx_in_remaining_cats;
        util::unchecked_binary_search(remaining_cats_ptr, remaining_categories.tail(), in_cat_ptr[i], &idx_in_remaining_cats);
        
        remaining_categories.erase(idx_in_remaining_cats);
    }
    
    uint32_t n_remaining_cats = remaining_categories.tail();
    uint32_t* remaining_cats_ptr = remaining_categories.unsafe_get_pointer();
    
    types::entries_t* raw_inds = res.indices.unsafe_get_pointer();
    uint32_t* raw_combs = res.combinations.unsafe_get_pointer();
    
    util::locator copy = *this;
    
    //  resize indices to total size
    for (auto& it : copy.m_indices)
    {
        it.second.resize(total_sz);
        it.second.fill(false);
    }
    
    copy.m_tmp_index.resize(total_sz);
    
    //  mapping category to collapsed id.
    std::unordered_map<uint32_t, uint32_t> collapsed_labels;
    
    for (uint32_t i = 0; i < n_indices; i++)
    {
        //  for the inputted categories, we can just copy the
        //  combinations
        uint32_t* c_indices = raw_inds[i].unsafe_get_pointer();
        uint32_t c_n_indices = raw_inds[i].tail();
        
        for (uint32_t j = 0; j < n_cats_in; j++)
        {
            uint32_t lab = raw_combs[i * n_cats_in + j];
            
            copy.m_indices[lab].place(true, i);
        }
        
        //  for the other categories, we have to see which label
        //  to use for each index
        for (uint32_t j = 0; j < n_remaining_cats; j++)
        {
            
            uint32_t c_cat = remaining_cats_ptr[j];
            
            //  other labels in the current category
            const types::entries_t& other_labs = copy.m_by_category[c_cat];
            
            uint32_t n_other_labs = other_labs.tail();
            
            if (n_other_labs == 0)
            {
                continue;
            }
            
            if (n_other_labs == 1)
            {
                util::bit_array& other_idx = copy.m_indices[other_labs.at(0)];
                
                other_idx.place(true, i);
                
                continue;
            }
            
            //  otherwise, we have to determine whether to collapse the
            //  labels at these indices
            
            uint32_t* other_labs_ptr = other_labs.unsafe_get_pointer();
            
            // find the label at the first index
            
            uint32_t first_lab = util::locator::UNDEFINED_LABEL;
            
            for (uint32_t k = 0; k < n_other_labs; k++)
            {
                uint32_t c_lab = other_labs_ptr[k];
                
                const util::bit_array& lab_idx = m_indices.at(c_lab);
                
                if (lab_idx.at(c_indices[0] - index_offset))
                {
                    first_lab = c_lab;
                    break;
                }
            }
            
            bool proceed = true;
            bool need_collapse = false;
            uint32_t k = 1;
            const util::bit_array& lab_idx = m_indices.at(first_lab);
            
            while (proceed && k < c_n_indices)
            {
                if (!lab_idx.at(c_indices[k] - index_offset))
                {
                    need_collapse = true;
                    proceed = false;
                }
                
                k++;
            }
            
            uint32_t set_lab;
            
            if (need_collapse)
            {
                auto collapsed_it = collapsed_labels.find(c_cat);
                
                uint32_t collapsed_lab;
                
                //  we need to insert a new collapsed label
                if (collapsed_it == collapsed_labels.end())
                {
                    collapsed_lab = copy.get_random_label_id();
                    collapsed_labels[c_cat] = collapsed_lab;
                    
                    copy.m_labels.push(collapsed_lab);
                    copy.m_in_category[collapsed_lab] = c_cat;
                    copy.m_indices[collapsed_lab] = util::bit_array(total_sz, false);
                    
                    util::types::entries_t& by_cat = copy.m_by_category[c_cat];
                    
                    by_cat.push(collapsed_lab);
                    
                    copy.m_n_labels++;
                    
                    by_cat.sort();
                    copy.m_labels.sort();
                }
                else
                {
                    collapsed_lab = collapsed_it->second;
                }
                
                set_lab = collapsed_lab;
            }
            else
            {
                set_lab = first_lab;
            }
        
            util::bit_array& c_idx = copy.m_indices.at(set_lab);
            
            c_idx.place(true, i);
        }
    }
    
    copy.prune();
    
    *this = std::move(copy);
    
    return res;
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

uint32_t util::locator::collapse_category(uint32_t category)
{
    if (!has_category(category))
    {
        return util::locator_status::CATEGORY_DOES_NOT_EXIST;
    }
    
    const types::entries_t& by_category = m_by_category.at(category);
    const uint32_t n_in_cat = by_category.tail();
    
    //  category is already collapsed
    if (n_in_cat == 0 || n_in_cat == 1)
    {
        return util::locator_status::OK;
    }
    
    uint32_t next_lab_id = get_random_label_id();
    
    bool is_present = false;
    bool create_tmp = false;
    util::bit_array index(size(), true);
    
    unchecked_set_category(category, next_lab_id, is_present, create_tmp, index);
    
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
    
    if (label == locator::UNDEFINED_LABEL)
    {
        return util::locator_status::IS_UNDEFINED_LABEL;
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
    
    //  if no elements are true, we can just return early
    if (!index.any())
    {
        return util::locator_status::OK;
    }
    
    unchecked_set_category(category, label, is_present, c_is_empty, index);
    
    return util::locator_status::OK;
}

uint32_t util::locator::set_category(uint32_t category, const util::types::entries_t& labels, const util::bit_array& index)
{
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
    
    //  sum of elements in the index must match the number of labels
    //  to assign
    if (labels.tail() != index.sum())
    {
        return util::locator_status::WRONG_NUMBER_OF_INDICES;
    }
    
    if (!index.any())
    {
        return util::locator_status::OK;
    }
    
    uint32_t n_labels_in = labels.tail();
    uint32_t* in_labels_ptr = labels.unsafe_get_pointer();
    
    types::entries_t in_labels_copy = labels;
    
    uint32_t* in_labels_copy_ptr = in_labels_copy.unsafe_get_pointer();
    uint32_t* unique_tail = std::unique(in_labels_copy_ptr, in_labels_copy_ptr + n_labels_in);
    uint32_t n_unique = unique_tail - in_labels_copy_ptr;
    
    util::dynamic_array<bool> lab_exists(n_unique);
    bool* lab_exists_ptr = lab_exists.unsafe_get_pointer();
    
    for (uint32_t i = 0; i < n_unique; i++)
    {
        auto it = m_in_category.find(in_labels_copy_ptr[i]);
        
        if (it == m_in_category.end())
        {
            lab_exists_ptr[i] = false;
            continue;
        }

        if (it->second != category)
        {
            return util::locator_status::LABEL_EXISTS_IN_OTHER_CATEGORY;
        }
        
        lab_exists_ptr[i] = true;
    }
    
    uint32_t index_sz;
    
    if (c_is_empty)
    {
        index_sz = n_labels_in;
    }
    else
    {
        index_sz = c_size;
    }
    
    util::bit_array tmp_index(index_sz, false);
    types::entries_t offsets = bit_array::find(index);
    uint32_t* offset_ptr = offsets.unsafe_get_pointer();
    
    for (uint32_t i = 0; i < n_unique; i++)
    {
        uint32_t c_label = in_labels_copy_ptr[i];
        
        for (uint32_t j = 0; j < n_labels_in; j++)
        {
            if (in_labels_ptr[j] == c_label)
            {
                uint32_t offset = offset_ptr[j];
                tmp_index.unchecked_place(true, offset);
            }
        }
        
        unchecked_set_category(category, c_label, lab_exists_ptr[i], is_empty(), tmp_index);
        
        tmp_index.fill(false);
    }
    
    return util::locator_status::OK;
}

void util::locator::unchecked_set_category(uint32_t category, uint32_t label, bool is_present, bool create_tmp, const util::bit_array& index)
{
    util::types::entries_t& by_category = m_by_category[category];
    
    uint32_t* by_category_ptr = by_category.unsafe_get_pointer();
    uint32_t n_by_category = by_category.tail();
    uint32_t c_sz = size();
    
    //  set false at rows of other indices
    for (uint32_t i = 0; i < n_by_category; i++)
    {
        uint32_t lab = by_category_ptr[i];
        
        //  if updating a pre-existing label, set true to elements
        //  that are currently false
        if (lab == label)
        {
            util::bit_array& c_index = m_indices[lab];
            util::bit_array::unchecked_dot_or(c_index, c_index, index, 0, c_sz);
            continue;
        }
        
        util::bit_array& lab_index = m_indices[lab];
        
        util::bit_array::unchecked_dot_and_not(lab_index, lab_index, index, 0, c_sz);
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
    
    if (create_tmp)
    {
        m_tmp_index = util::bit_array(index.size(), false);
    }
    
    if (m_n_labels > 1)
    {
        prune();
    }
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
    
    m_tmp_index.unchecked_keep(at_indices, index_offset);
    
    prune();
}

uint32_t util::locator::append(const util::locator &other)
{
    if (!categories_match(other))
    {
        return util::locator_status::CATEGORIES_DO_NOT_MATCH;
    }
    
    if (other.is_empty())
    {
        return util::locator_status::OK;
    }
    
    if (is_empty())
    {
        *this = other;
        return util::locator_status::OK;
    }
    
    util::types::entries_t other_labels = other.m_labels;
    uint32_t* own_label_ptr = m_labels.unsafe_get_pointer();
    
    uint32_t original_sz = size();
    uint32_t other_sz = other.size();
    
    uint32_t int_max = ~(uint32_t(0));
    
    if (int_max - original_sz < other_sz)
    {
        return util::locator_status::LOC_OVERFLOW;
    }
    
    for (uint32_t i = 0; i < m_n_labels; i++)
    {
        uint32_t own_label = own_label_ptr[i];
        
        if (other.has_label(own_label))
        {
            m_indices[own_label].append(other.m_indices.at(own_label));
            
            uint32_t index_in_other_labels;
            uint32_t* other_label_ptr = other_labels.unsafe_get_pointer();
            util::unchecked_binary_search(other_label_ptr, other_labels.tail(), own_label, &index_in_other_labels);
            
            other_labels.erase(index_in_other_labels);
            
            continue;
        }
        
        m_indices[own_label].resize(original_sz + other_sz);
    }
    
    uint32_t remaining_labels = other_labels.tail();
    uint32_t* other_label_ptr = other_labels.unsafe_get_pointer();
    
    for (uint32_t i = 0; i < remaining_labels; i++)
    {
        uint32_t other_lab = other_label_ptr[i];
        uint32_t in_cat = other.m_in_category.at(other_lab);
        
        util::bit_array own_index(original_sz, false);
        
        own_index.append(other.m_indices.at(other_lab));
        
        m_indices[other_lab] = std::move(own_index);
        m_labels.push(other_lab);
        m_in_category[other_lab] = in_cat;
        
        util::types::entries_t& by_category = m_by_category.at(in_cat);
        
        by_category.push(other_lab);
        by_category.unchecked_sort(by_category.tail());
        
        m_n_labels++;
    }
    
    m_labels.unchecked_sort(m_n_labels);
    
    m_tmp_index.append(other.m_tmp_index);
    
    return util::locator_status::OK;
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

void util::locator::resize(uint32_t to_size)
{
    uint32_t orig_size = size();
    
    for (auto& it : m_indices)
    {
        it.second.resize(to_size);
    }
    
    if (to_size < orig_size)
    {
        prune();
    }
    
    if (orig_size > 0)
    {
        m_tmp_index.resize(to_size);
    }
}

util::types::numeric_indices_t util::locator::find(const uint32_t label, uint32_t index_offset) const
{
    util::types::numeric_indices_t empty_result;
    
    if (!has_label(label))
    {
        return empty_result;
    }
    
    const util::bit_array& index = m_indices.at(label);
    
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

uint32_t util::locator::count(uint32_t label) const
{
    auto it = m_indices.find(label);
    
    if (it == m_indices.end())
    {
        return 0u;
    }
    
    return it->second.sum();
}

bool util::locator::has_label(uint32_t label) const
{
    return m_in_category.find(label) != m_in_category.end();
}

bool util::locator::has_category(uint32_t category) const
{
    return m_by_category.find(category) != m_by_category.end();
}

bool util::locator::is_full_category(uint32_t category, bool *exists) const
{
    auto it = m_by_category.find(category);
    
    if (it == m_by_category.end())
    {
        *exists = false;
        return false;
    }
    
    *exists = true;
    
    const util::types::entries_t& labs = it->second;
    
    uint32_t n_labs = labs.tail();
    uint32_t* labs_ptr = labs.unsafe_get_pointer();
    uint32_t sum = 0;
    uint32_t c_size = size();
    
    for (uint32_t i = 0; i < n_labs; i++)
    {
        sum += m_indices.at(labs_ptr[i]).sum();
    }
    
    return sum == c_size;
}

uint32_t util::locator::swap_label(uint32_t from, uint32_t to)
{
    if (!has_label(from))
    {
        return locator_status::LABEL_DOES_NOT_EXIST;
    }
    
    if (has_label(to))
    {
        return locator_status::LABEL_EXISTS;
    }
    
    //  update in category
    uint32_t category = m_in_category.at(from);
    m_in_category[to] = category;
    m_in_category.erase(from);
    
    //  update by category
    types::entries_t& by_cat = m_by_category.at(category);
    
    uint32_t idx_in_by_cat;
    util::unchecked_binary_search(by_cat.unsafe_get_pointer(), by_cat.tail(), from, &idx_in_by_cat);
    
    by_cat.erase(idx_in_by_cat);
    by_cat.push(to);
    by_cat.unchecked_sort(by_cat.tail());
    
    //  update indices
    m_indices[to] = std::move(m_indices.at(from));
    m_indices.erase(from);
    
    //  update labels
    uint32_t idx_in_labs;
    util::unchecked_binary_search(m_labels.unsafe_get_pointer(), m_n_labels, from, &idx_in_labs);
    m_labels.erase(idx_in_labs);
    m_labels.push(to);
    m_labels.unchecked_sort(m_n_labels);
    
    return locator_status::OK;
}

uint32_t util::locator::swap_category(uint32_t from, uint32_t to)
{
    if (!has_category(from))
    {
        return locator_status::CATEGORY_DOES_NOT_EXIST;
    }
    
    if (has_category(to))
    {
        return locator_status::CATEGORY_EXISTS;
    }
    
    const types::entries_t& by_cat = m_by_category.at(from);
    
    uint32_t n_in_cat = by_cat.tail();
    uint32_t* by_cat_ptr = by_cat.unsafe_get_pointer();
    
    for (uint32_t i = 0; i < n_in_cat; i++)
    {
        m_in_category[by_cat_ptr[i]] = to;
    }
    
    m_by_category[to] = std::move(by_cat);
    m_by_category.erase(from);
    
    uint32_t idx_in_categories;
    util::unchecked_binary_search(m_categories.unsafe_get_pointer(), m_categories.tail(), from, &idx_in_categories);
    
    m_categories.erase(idx_in_categories);
    m_categories.push(to);
    m_categories.unchecked_sort(m_categories.tail());
    
    return locator_status::OK;
}

//  rm_label: Remove label.
//
//      No modifications occur if the label is not present.

void util::locator::rm_label(uint32_t lab)
{
    size_t n_erased = m_indices.erase(lab);
    
    if (n_erased == 0)
    {
        return;
    }
    
    uint32_t cat = m_in_category[lab];
    
    util::types::entries_t& by_category = m_by_category[cat];
    uint32_t* by_category_ptr = by_category.unsafe_get_pointer();
    
    uint32_t idx_in_by_category;
    
    util::unchecked_binary_search(by_category_ptr, by_category.tail(), lab, &idx_in_by_category);
    
    by_category.erase(idx_in_by_category);
    m_labels.erase(find_label(lab));
    m_in_category.erase(lab);
    
    m_n_labels--;
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

uint32_t util::locator::get_random_label_id() const
{
    //  shouldn't happen
    if (m_n_labels == ~(uint32_t(0)))
    {
        return 0u;
    }
    
    return util::get_random_id(std::bind(&util::locator::has_label, this, std::placeholders::_1));
}

uint32_t util::locator::get_random_label_id(const util::locator &a, const util::locator &b)
{
    uint32_t int_max = ~(uint32_t(0));
    
    //  shouldn't happen
    if (a.m_n_labels == int_max || b.m_n_labels == int_max)
    {
        return 0u;
    }
    
    auto func = [&a, &b] (uint32_t id) -> bool {
        return a.has_label(id) || b.has_label(id);
    };
    
    return util::get_random_id(func);
}
