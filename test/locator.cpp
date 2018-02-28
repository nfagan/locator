#include <locator.hpp>
#include <iostream>
#include <assert.h>
#include <chrono>
#include <cstdint>

void test_add_category();
void test_add_label();
void test_locate();
double test_locate_speed(uint32_t sz);
void test_locate_speed_multi();
double ellapsed_time_s(std::chrono::high_resolution_clock::time_point t1, std::chrono::high_resolution_clock::time_point t2);

int main(int argc, char* argv[])
{
    test_add_label();
    test_add_category();
    test_locate();
    test_locate_speed_multi();
    
    return 0;
}

void test_locate_speed_multi()
{
    double mean = 0.0;
    double iters = 0.0;
    double total_time = 0.0;
    uint32_t sz = 1e4;
    
    uint32_t n_iters = 1000;
    
    for (uint32_t i = 0; i < n_iters; i++)
    {
        double res = test_locate_speed(sz);
        mean = (mean * iters + res) / (iters + 1.0);
        iters += 1.0;
        total_time += res;
    }
    
    std::cout << "Mean time: (locate) " << (mean * 1000.0) << " (ms), ";
    std::cout << sz << " (elements)" << std::endl;
    std::cout << "Total time: (locate) " << (total_time * 1000.0) << " (ms)" << std::endl;
    std::cout << "--" << std::endl;
}

double test_locate_speed(uint32_t sz)
{
    using namespace util;
    using namespace std::chrono;
    
    high_resolution_clock::time_point t1;
    high_resolution_clock::time_point t2;
    
    locator loc;
    uint32_t n_true = sz / 1e2;
    uint32_t n_search = 100;
    
    uint32_t n_categories = 7;
    uint32_t n_labels = 10000;
    
    for (uint32_t i = 0; i < n_categories; i++)
    {
        loc.add_category(i);
    }
    
    util::bit_array index(sz);
    
    for (uint32_t i = 0; i < n_labels; i++)
    {
        index.fill(false);
        
        uint32_t cat = rand() % n_categories;
        
        for (uint32_t j = 0; j < n_true; j++)
        {
            uint32_t idx = rand() % sz;
            index.place(true, idx);
        }
        
        loc.add_label(i, cat, index);
    }
    
    types::entries_t search_for;
    
    for (uint32_t i = 0; i < n_search; i++)
    {
        search_for.push(i);
    }
    
    t1 = high_resolution_clock::now();
    
    types::entries_t res = loc.locate(search_for);
    
    t2 = high_resolution_clock::now();
    
    return ellapsed_time_s(t1, t2);
}

double ellapsed_time_s(std::chrono::high_resolution_clock::time_point t1, std::chrono::high_resolution_clock::time_point t2)
{
    return std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1).count();
}

void test_locate()
{
    using namespace util;
    
    locator loc;
    uint32_t sz = 100;
    
    bit_array index0(sz);
    
    index0.fill(false);
    
    bit_array index1 = index0;
    bit_array index2 = index0;
    bit_array index3 = index0;
    
    index0.place(true, 99);
    index1.place(true, 99);
    index2.place(true, 0);
    index3.place(true, 0);
    
    uint32_t cat0 = 0;
    uint32_t cat1 = 1;
    
    loc.add_category(cat0);
    loc.add_category(cat1);
    
    loc.add_label(10, cat0, index0);
    loc.add_label(11, cat0, index1);
    loc.add_label(12, cat0, index2);
    
    loc.add_label(20, cat1, index3);
    
    types::entries_t labels0 = types::entries_t();
    types::entries_t labels1 = types::entries_t();
    types::entries_t labels2 = types::entries_t();
    types::entries_t labels3 = types::entries_t();
    types::entries_t labels4 = types::entries_t();
    
    labels0.push(10);
    
    labels1.push(10);
    labels1.push(11);
    
    labels2.push(10);
    labels2.push(11);
    labels2.push(12);
    
    labels3.push(20);
    labels3.push(12);
    
    labels4.push(20);
    labels4.push(10);
    
    types::entries_t res0 = loc.locate(labels0);
    
    assert(res0.tail() == 1 && res0.at(0) == 99);
    
    types::entries_t res1 = loc.locate(labels1);
    
    assert(res1.tail() == 1 && res1.at(0) == 99);
    
    types::entries_t res2 = loc.locate(labels2);
    
    assert(res2.tail() == 2 && res2.at(0) == 0 && res2.at(1) == 99);
    
    types::entries_t res3 = loc.locate(labels3);
    
    assert(res3.tail() == 1 && res3.at(0) == 0);
    
    types::entries_t res4 = loc.locate(labels4);
    
    assert(res4.tail() == 0);
    
    index0.place(true, 70);
    res0 = loc.locate(labels0);
    assert(res0.tail() == 1 && res0.at(0) == 99);
}

void test_add_label()
{
    using namespace util;
    
    locator loc;
    uint32_t result;
    bit_array index(100);
    
    uint32_t label = 10;
    uint32_t category = 100;
    
    result = loc.add_label(label, category, index);
    
    assert(result == locator_status::CATEGORY_DOES_NOT_EXIST);
    
    result = loc.add_category(category);
    
    assert(result == locator_status::OK);
    
    result = loc.add_label(label, category, index);
    
    assert(result == locator_status::OK);
    
    result = loc.add_label(label, category, index);
    
    assert(result = locator_status::LABEL_EXISTS);
    assert(loc.n_labels() == 1);
    
    result = loc.add_label(9, category, index);
    
    assert(result == locator_status::OK);
    
    assert(loc.n_labels() == 2);
    
    types::entries_t labels = loc.get_labels();
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
