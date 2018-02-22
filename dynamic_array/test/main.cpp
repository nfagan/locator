#include <dynamic_array.hpp>
#include <iostream>
#include <assert.h>
#include <chrono>
#include <vector>
#include <cstdint>

void test_general();
void test_push();

int main(int argc, char* argv[])
{
    test_push();
    test_general();
    
//    dynamic_array<int> arr6(arr5);
}

void test_push()
{
    using namespace util;
    
    std::cout << "Begin push" << std::endl;
    
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
    
    std::cout << "End push" << std::endl;
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
