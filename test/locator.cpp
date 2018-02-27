#include <locator.hpp>
#include <iostream>
#include <assert.h>
#include <chrono>
#include <cstdint>

void test_add_category();

int main(int argc, char* argv[])
{    
    return 0;
}

void test_add_category()
{
    using namespace util;
    
    locator loc;
    
    uint32_t result;
    
    result = loc.add_category(10);
    
    assert(result == locator_status::OK);
    assert(loc.has_category(10));
    
    result = loc.add_category(10);
    
    assert(result == locator_status::CATEGORY_EXISTS);
    
    result = loc.add_category(11);
    
    assert(loc.has_category(11));
}
