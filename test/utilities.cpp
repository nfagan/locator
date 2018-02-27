#include <iostream>
#include <assert.h>
#include <chrono>
#include <utilities.hpp>
#include <vector>
#include <dynamic_array.hpp>
#include <cstdint>
#include <algorithm>

void test_binary_search();
void test_quick_sort();
double ellapsed_time_s(std::chrono::high_resolution_clock::time_point t1, std::chrono::high_resolution_clock::time_point t2);

int main(int argc, char* argv[])
{
    test_binary_search();
    test_quick_sort();
}

double ellapsed_time_s(std::chrono::high_resolution_clock::time_point t1, std::chrono::high_resolution_clock::time_point t2)
{
    return std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1).count();
}

void test_quick_sort()
{
    using namespace util;
    using namespace std::chrono;
    
    high_resolution_clock::time_point t1;
    high_resolution_clock::time_point t2;
    
    uint32_t sz = 1e6;
    
    dynamic_array<uint32_t> darray(sz);
    dynamic_array<uint32_t> darray2(sz);
    
    darray.seek_tail_to_start();
    darray2.seek_tail_to_start();
    
    uint32_t* ptr = darray.unsafe_get_pointer();
    uint32_t* ptr2 = darray2.unsafe_get_pointer();
    
    for (uint32_t i = 0; i < sz; i++)
    {
        ptr[i] = rand() % 1000;
        ptr2[i] = rand() % 1000;
    }
    
    t1 = high_resolution_clock::now();
    
    quick_sort(ptr, sz);
    
    t2 = high_resolution_clock::now();
    
    double ellapsed = ellapsed_time_s(t1, t2);
    
    std::cout << "Time to sort " << sz << " elements: " << ellapsed * 1000 << " (ms)" << std::endl;
    
    t1 = high_resolution_clock::now();
    
    std::sort(ptr2, ptr2 + sz);
    
    t2 = high_resolution_clock::now();
    
    ellapsed = ellapsed_time_s(t1, t2);
    
    std::cout << "Time to sort (built-in)" << sz << " elements: " << ellapsed * 1000 << " (ms)" << std::endl;
    
    for (uint32_t i = 0; i < sz-1; i++)
    {
        uint32_t current = ptr[i];
        uint32_t next = ptr[i+1];

        assert(current <= next);
    }
}

void test_binary_search()
{
    std::vector<uint32_t> values({1, 2, 2, 4, 8, 9});
    
    bool res;
    uint32_t at_index;
    
    util::unchecked_binary_search<uint32_t>(&values[0], values.size(), 2, &res, &at_index);
    
    assert(res);
    assert(at_index == 2);
    
    util::unchecked_binary_search<uint32_t>(&values[0], values.size(), 0, &res, &at_index);
    
    assert(!res);
    assert(at_index == 0);
    
    util::unchecked_binary_search<uint32_t>(&values[0], values.size(), 7, &res, &at_index);
    
    assert(!res);
    assert(at_index == 4);
    
    std::vector<uint32_t> values2;
    
    uint32_t sz = 10000;
    
    for (uint32_t i = 1; i < sz; i += 2)
    {
        values2.push_back(i);
    }
    
    util::unchecked_binary_search<uint32_t>(&values2[0], values2.size(), sz/2, &res, &at_index);
    
    assert(!res);
    assert(at_index == sz/4);
}
