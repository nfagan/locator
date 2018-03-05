#include "dynamic_array.hpp"
#include "profile.hpp"
#include <iostream>
#include <assert.h>
#include <chrono>
#include <vector>
#include <string>
#include <cstdint>

void test_simple();
void test_general();
void test_push();
void test_insert();
void test_insert_multi();
double test_push_speed_vector(uint32_t sz);
double test_push_speed_array(uint32_t sz);
void test_push_speed_array_multi();
void test_push_speed_vector_multi();
void test_array_of_array();
double test_dynamic_alloc_speed_array(uint32_t sz);
double test_dynamic_alloc_speed_vector(uint32_t sz);
void test_dynamic_alloc_speed_array_multi();
void test_dynamic_alloc_speed_vector_multi();
double ellapsed_time_s(std::chrono::high_resolution_clock::time_point t1, std::chrono::high_resolution_clock::time_point t2);
void test_erase();

int main(int argc, char* argv[])
{
    std::cout << "BEGIN DYNAMIC ARRAY" << std::endl;
    test_simple();
    test_push();
    test_erase();
    test_array_of_array();
    test_dynamic_alloc_speed_array_multi();
    test_dynamic_alloc_speed_vector_multi();
    test_insert_multi();
    test_push_speed_array_multi();
    test_push_speed_vector_multi();
    test_general();
    
    std::cout << "END DYNAMIC ARRAY" << std::endl;
    return 0;
}

void test_simple()
{
    using namespace util;
    
    typedef dynamic_array<uint32_t> arr_t;
    typedef dynamic_array<arr_t, dynamic_allocator<arr_t>> arr_arr_t;
    
    arr_t arr;
    arr_arr_t arr_arr(1000);
    
    arr_arr.seek_tail_to_start();
    
    for (uint32_t i = 0; i < 1000; i++)
    {
        arr.push(i);
    }
    
    for (uint32_t i = 0; i < 1000; i++)
    {
        arr_arr.push(arr);
    }
    
    uint32_t i = 0;

    while (arr_arr.tail() > 0)
    {
        arr_arr.erase(i);
    }
//
    for (uint32_t i = 0; i < 1000; i++)
    {
        arr_arr.insert(arr, arr_arr.tail());
    }
}

void test_erase()
{
    using namespace util;
    typedef dynamic_array<uint32_t> arr_t;
    typedef dynamic_array<arr_t, dynamic_allocator<arr_t>> arr_arr_t;
    
    dynamic_array<uint32_t> arr1;
    uint32_t sz = 1e3;
    
    for (uint32_t i = 0; i < sz; i++)
    {
        arr1.push(i);
    }
    
    uint32_t at_index = 10;
    
    assert(arr1.tail() == sz);
    
    arr1.erase(at_index);
    
    assert(arr1.tail() == sz-1);
    
    for (uint32_t i = 0; i < at_index; i++)
    {
        assert(arr1.at(i) == i);
    }
    
    for (uint32_t i = at_index; i < arr1.tail(); i++)
    {
        assert(arr1.at(i) == i+1);
    }
    
    arr_arr_t arr_arr1;
    arr_t arr2;
    
    arr2.push(10);
    arr2.push(20);
    
    arr_arr1.push(arr2);
    arr_arr1.push(arr2);
    
    arr_arr1.erase(0);
    
    arr_t arr3;
    
    arr3.push(10);
    arr3.push(11);
    arr3.push(12);
    
    assert(arr3.tail() == 3);
    
    arr3.erase(0);
    
    assert(arr3.tail() == 2);
    assert(arr3.at(0) == 11);
    
    arr3.erase(1);
    
    assert(arr3.tail() == 1);
    assert(arr3.at(0) == 11);
}

void test_dynamic_alloc_speed_vector_multi()
{
    double mean = 0.0;
    double iters = 0.0;
    double total_time = 0.0;
    uint32_t sz = 1e2;
    
    uint32_t n_iters = 1000;
    
    for (uint32_t i = 0; i < n_iters; i++)
    {
        double res = test_dynamic_alloc_speed_vector(sz);
        mean = (mean * iters + res) / (iters + 1.0);
        iters += 1.0;
        total_time += res;
    }
    
    std::cout << "Push string (vector) (mean): " << (mean * 1000.0) << " (ms), ";
    std::cout << sz << " elements" << std::endl;
    std::cout << "Push string (vector) (total) " << (total_time * 1000.0) << " (ms)" << std::endl;
    std::cout << "--" << std::endl;
}


void test_dynamic_alloc_speed_array_multi()
{
    double mean = 0.0;
    double iters = 0.0;
    double total_time = 0.0;
    uint32_t sz = 1e2;
    
    uint32_t n_iters = 1000;
    
    for (uint32_t i = 0; i < n_iters; i++)
    {
        double res = test_dynamic_alloc_speed_array(sz);
        mean = (mean * iters + res) / (iters + 1.0);
        iters += 1.0;
        total_time += res;
    }
    
    std::cout << "Push string (dynamic_array) (mean): " << (mean * 1000.0) << " (ms), ";
    std::cout << sz << " elements" << std::endl;
    std::cout << "Push string (dynamic_array) (total) " << (total_time * 1000.0) << " (ms)" << std::endl;
    std::cout << "--" << std::endl;
}

double test_dynamic_alloc_speed_vector(uint32_t sz)
{
    using namespace util;
    using namespace std::chrono;
    
    std::vector<std::string> vec1;
    high_resolution_clock::time_point t1;
    high_resolution_clock::time_point t2;
    
    t1 = high_resolution_clock::now();
    
    for (uint32_t i = 0; i < sz; i++)
    {
        std::string to_push = "example string" + std::to_string(rand() % 1000);
        vec1.push_back(to_push);
    }
    
    t2 = high_resolution_clock::now();
    
    return ellapsed_time_s(t1, t2);
}

double test_dynamic_alloc_speed_array(uint32_t sz)
{
    using namespace util;
    using namespace std::chrono;
    
    dynamic_array<std::string, dynamic_allocator<std::string>> arr1;
    high_resolution_clock::time_point t1;
    high_resolution_clock::time_point t2;
    
    arr1.seek_tail_to_start();
    
    t1 = high_resolution_clock::now();
    
    for (uint32_t i = 0; i < sz; i++)
    {
        std::string to_push = "example string" + std::to_string(rand() % 1000);
        arr1.push(to_push);
    }
    
    t2 = high_resolution_clock::now();
    
    return ellapsed_time_s(t1, t2);
}

void test_array_of_array()
{
    using namespace util;
    
    typedef dynamic_array<std::string, dynamic_allocator<std::string>> array_t;
    typedef dynamic_array<array_t, dynamic_allocator<array_t>> array_array_t;
    
    array_array_t arr1;
    
    array_t arr2;
    array_t arr_t_3;
    
    arr2.push("hi");
    arr2.push("hello");
    arr2.push("sup");
    
    arr_t_3.push("hi2");
    arr_t_3.push("hi3");
    
    arr1.push(arr2);
    arr1.push(arr_t_3);
    
    array_array_t arr3 = arr1;
    
    array_t arr4 = arr3.at(0);
    
    assert(arr4.at(0) == std::string("hi"));
    assert(arr4.at(1) == std::string("hello"));
    assert(arr4.at(2) == std::string("sup"));
}

void test_push_speed_vector_multi()
{
    double mean = 0.0;
    double iters = 0.0;
    double total_time = 0.0;
    uint32_t sz = 1e5;
    
    uint32_t n_iters = 1000;
    
    for (uint32_t i = 0; i < n_iters; i++)
    {
        double res = test_push_speed_vector(sz);
        mean = (mean * iters + res) / (iters + 1.0);
        iters += 1.0;
        total_time += res;
    }
    
    std::cout << "Push (vector) (mean): " << (mean * 1000.0) << " (ms), ";
    std::cout << sz << " elements" << std::endl;
    std::cout << "Push (vector) (total) " << (total_time * 1000.0) << " (ms)" << std::endl;
    std::cout << "--" << std::endl;
}

void test_push_speed_array_multi()
{
    double mean = 0.0;
    double iters = 0.0;
    double total_time = 0.0;
    uint32_t sz = 1e5;
    
    uint32_t n_iters = 1000;
    
    for (uint32_t i = 0; i < n_iters; i++)
    {
        double res = test_push_speed_array(sz);
        mean = (mean * iters + res) / (iters + 1.0);
        iters += 1.0;
        total_time += res;
    }
    
    std::cout << "Push (dynamic_array) (mean): " << (mean * 1000.0) << " (ms), ";
    std::cout << sz << " elements" << std::endl;
    std::cout << "Push (dynamic_array) (total) " << (total_time * 1000.0) << " (ms)" << std::endl;
    std::cout << "--" << std::endl;
}

double test_push_speed_vector(uint32_t sz)
{
    using namespace util;
    using namespace std::chrono;
    
    high_resolution_clock::time_point t1;
    high_resolution_clock::time_point t2;
    
    std::vector<uint32_t> vec1;
    
    t1 = high_resolution_clock::now();
    
    for (uint32_t i = 0; i < sz; i++)
    {
        vec1.push_back(i);
    }
    
    t2 = high_resolution_clock::now();
    
    return ellapsed_time_s(t1, t2);
}

double test_push_speed_array(uint32_t sz)
{
    using namespace util;
    using namespace std::chrono;
    
    high_resolution_clock::time_point t1;
    high_resolution_clock::time_point t2;
    
    dynamic_array<uint32_t> arr1;
    
    t1 = high_resolution_clock::now();
    
    for (uint32_t i = 0; i < sz; i++)
    {
        arr1.push(i);
    }
    
    t2 = high_resolution_clock::now();
    
    return ellapsed_time_s(t1, t2);
}

void test_insert_multi()
{
    uint32_t n_iters = 1000;
    
    for (uint32_t i = 0; i < n_iters; i++)
    {
        test_insert();
    }
}

void test_insert()
{
    using namespace util;
    
    dynamic_array<uint32_t> arr1;
    arr1.seek_tail_to_end();

    arr1.push(10);
    arr1.push(11);
    arr1.push(12);
    arr1.push(13);

    arr1.insert(100, 1);

    assert(arr1.at(1) == 100);
    assert(arr1.at(0) == 10);
    assert(arr1.at(2) == 11);
    assert(arr1.at(3) == 12);
    assert(arr1.at(4) == 13);
    
    arr1.push(1000);
    
    assert(arr1.at(arr1.tail() - 1) == 1000);
    
    dynamic_array<uint32_t> arr2;
    
    for (uint32_t i = 0; i < 100; i++)
    {
        arr2.push(0);
    }
    
    dynamic_array<uint32_t> arr3;
    
    arr3.insert(1000, 0);
    arr3.insert(100, 100);
    
    assert(arr3.at(0) == 1000);
    assert(arr3.at(100) == 100);
    
//    arr2.insert(100, 100);
//    arr2.push(0);
}

void test_push()
{
    using namespace util;
    
    dynamic_array<uint32_t> arr1;
    
    for (uint32_t i = 0; i < 10000; i++)
    {
        arr1.push(i);
    }
    
    dynamic_array<uint32_t> arr2(100);
    
    for (uint32_t i = 0; i < 10000; i++)
    {
        arr2.push(i);
    }
}

void test_general()
{
    using namespace util;
    using namespace std::chrono;
    
    int sz1 = 2;
    
    dynamic_array<int> arr(sz1);
    std::vector<int> vec(sz1);
    
    arr.push(3);
    arr.push(4);
    arr.push(8);
    
    assert(arr.at(0+sz1) == 3);
    assert(arr.at(1+sz1) == 4);
    assert(arr.at(2+sz1) == 8);
    
    assert(arr.tail() == sz1 + 3);
    
    arr.push(6);
    arr.push(8);
    arr.push(10);
    
    assert(arr.size() == 8);
    assert(arr.at(5+sz1) == 10);
    
    arr.clear();
    arr.clear();
    
    assert(arr.size() == 0);
    assert(arr.tail() == 0);
    
    arr.push(100);
    arr.push(200);
    
    assert(arr.size() == 2);
    assert(arr.at(0) == 100);
    
    arr.clear();
    
    long new_size = (long) 1e6;
    
    high_resolution_clock::time_point t1 = high_resolution_clock::now();
    
    arr.resize(new_size);
    
    for (int i = 0; i < new_size; i++)
    {
        arr.unchecked_place(i, i);
    }
    
    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
    std::cout << "Ellapsed (dynamic_array): " << time_span.count() * 1000 << "(ms)" << std::endl;
    
    arr.place(1000, new_size-1);
    
    assert(arr.at(new_size-1) == 1000);
    
    dynamic_array<int> arr2(0);
    
    t1 = high_resolution_clock::now();
    
    vec.resize(new_size);
    
    for (int i = 0; i < new_size; i++)
    {
        vec[i] = i;
    }
    
    t2 = high_resolution_clock::now();
    time_span = duration_cast<duration<double>>(t2 - t1);
    std::cout << "Ellapsed (vector): " << time_span.count() * 1000 << "(ms)" << std::endl;
    
    t1 = high_resolution_clock::now();
    dynamic_array<int> arr3 = arr2;
    t2 = high_resolution_clock::now();
    time_span = duration_cast<duration<double>>(t2 - t1);
    
    std::cout << "Ellapsed (duplication): " << time_span.count() * 1000 << "(ms)" << std::endl;
    
    arr3.push(102);
    arr2.push(101);
    
    assert(arr2.at(0) == 101);
    assert(arr3.at(0) == 102);
    
    dynamic_array<int> arr4 = std::move(arr3);
    
    dynamic_array<int> arr5(100);
    
    arr5.push(10);
    
    assert(arr5.size() == 200);
    assert(arr5.tail() == 101);
    arr5.place(88, 99);
    assert(arr5.tail() == 101);
    assert(arr5.size() == 200);
}

double ellapsed_time_s(std::chrono::high_resolution_clock::time_point t1, std::chrono::high_resolution_clock::time_point t2)
{
    return std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1).count();
}
