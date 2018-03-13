#include "multimap.hpp"
#include "utilities.hpp"
#include <iostream>
#include <assert.h>
#include <cstdint>
#include <string>

void test_instantiation();

int main(int argc, char* argv[])
{
    test_instantiation();
}

void test_instantiation()
{
    using namespace util;
    
    typedef multimap<std::string, uint32_t> map_t;
    
    map_t map0;
    
    std::string sval = "hi";
    uint32_t val = 10u;
    
    map0.insert(sval, val);
    
    uint32_t res = map0.at(sval);
    
    assert(res == 10u);
    
    std::string res2 = map0.at(val);
    
    assert(res2 == sval);
    
    assert(map0.contains(sval) && map0.contains(val));
    
    assert(!map0.contains("hello"));
    assert(!map0.contains(0u));
    
    map_t map1 = map0;
    
    
    
}

