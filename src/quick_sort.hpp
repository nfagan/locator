//
//  quick_sort.hpp
//  locator
//
//  Created by Nick Fagan on 2/26/18.
//

#pragma once

#include <cstdint>

//  https://codereview.stackexchange.com/questions/77782/quick-sort-implementation

namespace util {
    template<typename T>
    void quick_sort(T* in_elements, int32_t left, int32_t right);
    
    template<typename T>
    void quick_sort(T* in_elements, int32_t n_elements);
    
    namespace detail {
        template<typename T>
        int32_t partition(T* in_elements, int32_t left, int32_t right);
        
        template<typename T>
        void swap(T* in_elements, int32_t i, int32_t j);
    }
}

template<typename T>
void util::detail::swap(T* in_elements, int32_t i, int32_t j)
{
    T tmp = in_elements[i];
    in_elements[i] = in_elements[j];
    in_elements[j] = tmp;
}

template<typename T>
int32_t util::detail::partition(T* in_elements, int32_t left, int32_t right)
{
    int32_t mid = left + (right - left) / 2;

    T pivot = in_elements[mid];

    detail::swap(in_elements, mid, left);

    int32_t i = left + 1;
    int32_t j = right;

    while (i <= j)
    {
        while (i <= j && in_elements[i] <= pivot)
        {
            i++;
        }

        while (i <= j && in_elements[j] > pivot)
        {
            j--;
        }

        if (i < j)
        {
            detail::swap(in_elements, i, j);
        }
    }

    detail::swap(in_elements, i - 1, left);

    return (i - 1);
}

template<typename T>
void util::quick_sort(T* in_elements, int32_t left, int32_t right)
{
    if (left >= right)
    {
        return;
    }
    
    uint32_t part = detail::partition(in_elements, left, right);
    
    quick_sort(in_elements, left, part - 1);
    quick_sort(in_elements, part + 1, right);
}

template<typename T>
void util::quick_sort(T* in_elements, int32_t n_elements)
{
    quick_sort(in_elements, 0, n_elements-1);
}
