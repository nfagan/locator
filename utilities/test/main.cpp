#include <iostream>
#include <assert.h>
#include <chrono>
#include <bsearch.hpp>
#include <vector>

void test_bsearch();

int main(int argc, char* argv[])
{
    test_bsearch();
}

void test_bsearch()
{
    std::vector<uint32_t> values({1, 2, 2, 4, 8, 9});
    
    bool res;
    uint32_t at_index;
    
    util::unchecked_bsearch<uint32_t>(&values[0], values.size(), 2, &res, &at_index);
    
    assert(res);
    assert(at_index == 2);
    
    util::unchecked_bsearch<uint32_t>(&values[0], values.size(), 0, &res, &at_index);
    
    assert(!res);
    assert(at_index == 0);
    
    util::unchecked_bsearch<uint32_t>(&values[0], values.size(), 7, &res, &at_index);
    
    assert(!res);
    assert(at_index == 4);
    
    std::vector<uint32_t> values2;
    
    uint32_t sz = 10000;
    
    for (uint32_t i = 1; i < sz; i += 2)
    {
        values2.push_back(i);
    }
    
    util::unchecked_bsearch<uint32_t>(&values2[0], values2.size(), sz/2, &res, &at_index);
    
    assert(!res);
    assert(at_index == sz/4);
}
