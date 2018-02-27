//
//  locator.cpp
//  locator
//
//  Created by Nick Fagan on 2/24/18.
//

#include "locator.hpp"
#include <utilities.hpp>

util::locator::locator()
{
    //
}

util::locator::~locator() noexcept
{
    
}

uint32_t util::locator::add_category(uint32_t category)
{
    if (has_category(category))
    {
        return util::locator_status::CATEGORY_EXISTS;
    }
    
    m_categories.push(category);
    m_n_categories++;
    
    sort_categories();
    
    return util::locator_status::OK;
}

uint32_t util::locator::add_label(uint32_t label, uint32_t in_category, const util::bit_array &index)
{
    bool label_exists;
    
    uint32_t insert_at = find_label(label, &label_exists);
    
    if (label_exists)
    {
        return util::locator_status::LABEL_EXISTS;
    }
    
    if (!has_category(in_category))
    {
        return util::locator_status::CATEGORY_DOES_NOT_EXIST;
    }
    
    uint32_t c_size_m = size_m();
    uint32_t index_size_m = index.size();
    
    //  size of incoming index must match current size, unless
    //  locator is empty.
    if (!is_empty() && index_size_m != c_size_m)
    {
        return util::locator_status::WRONG_INDEX_SIZE;
    }
    
    
    
    return util::locator_status::OK;
}

bool util::locator::is_empty() const
{
    return m_n_labels == 0;
}

uint32_t util::locator::size_m() const
{
    return is_empty() ? 0 : m_indices[0].size();
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

void util::locator::sort_categories()
{
    uint32_t* category_ptr = m_categories.unsafe_get_pointer();
    util::quick_sort(category_ptr, m_n_categories);
}
