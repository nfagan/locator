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
    uint32_t n_labels = m_labels.tail();
    
    util::unchecked_bsearch(labels, n_labels, label, was_found, &idx);
    
    return idx;
}

uint32_t util::locator::find_category(uint32_t category, bool *was_found) const
{
    uint32_t idx;
    uint32_t* categories = m_categories.unsafe_get_pointer();
    uint32_t n_categories = m_categories.tail();
    
    util::unchecked_bsearch(categories, n_categories, category, was_found, &idx);
    
    return idx;
}
