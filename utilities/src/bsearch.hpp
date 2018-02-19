//
//  bsearch.hpp
//  utilities-test
//
//  Created by Nick Fagan on 2/18/18.
//

#pragma once
#include <cstdint>

namespace util {
    template<typename T>
    void unchecked_bsearch(T* in_elements, uint32_t n_elements, T for_value, bool* was_found, uint32_t* at_index);
}


template<typename T>
void util::unchecked_bsearch(T *in_elements, uint32_t n_elements, T for_value, bool *was_found, uint32_t *at_index)
{
    uint32_t start = 0;
    uint32_t stop = n_elements - 1;
    
    *was_found = false;
    
    if (n_elements == 0 || for_value < in_elements[start])
    {
        *at_index = start;
        return;
    }
    
    if (for_value > in_elements[stop])
    {
        *at_index = n_elements;
        return;
    }
    
    while (stop >= start)
    {
        uint32_t mid = (stop - start) / 2 + start;
        
        T value = in_elements[mid];
        
        if (for_value == value)
        {
            *was_found = true;
            *at_index = mid;
            return;
        }
        
        if (for_value < value)
        {
            stop = mid - 1;
        }
        else
        {
            start = mid + 1;
        }
    }
    
    *at_index = start;
    return;
}
