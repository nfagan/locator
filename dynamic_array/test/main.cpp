#include <dynamic_array.hpp>
#include <iostream>
#include <assert.h>
#include <chrono>
#include <vector>

int main(int argc, char* argv[])
{
    using namespace util;
    using namespace std::chrono;
    
    dynamic_array<int> arr(2);
    std::vector<int> vec(2);
    
    arr.push(3);
    arr.push(4);
    arr.push(8);
    
    assert(arr.at(0) == 3);
    assert(arr.at(1) == 4);
    assert(arr.at(2) == 8);
    
    assert(arr.size() == 4);
    assert(arr.count() == 3);
    
    arr.push(6);
    arr.push(8);
    arr.push(10);
    
    assert(arr.size() == 8);
    assert(arr.at(5) == 10);
    
    arr.clear();
    arr.clear();
    
    assert(arr.size() == 0);
    assert(arr.count() == 0);
    
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
    
    arr.place(1000, new_size+1);
    
    assert(arr.at(new_size+1) == 1000);
    assert(arr.size() == (new_size+1) * 2);
    
    dynamic_array<int> arr2(0);
    
    arr2.place(1001, 1000);
    
    assert(arr2.at(1000) == 1001);
    
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
    
    assert(arr3.at(1000) == 1001);
    
    arr3.place(102, 0);
    arr2.place(101, 0);
    
    assert(arr2.at(0) == 101);
    assert(arr3.at(0) == 102);
    
    dynamic_array<int> arr4 = std::move(arr3);
    
    arr3.push(1001);
    
    assert(arr3.at(0) == 1001);
    
    dynamic_array<int> arr5(100);
    
    arr5.push(10);
    
    assert(arr5.size() == 100);
    assert(arr5.count() == 1);
    arr5.place(88, 99);
    assert(arr5.count() == 2);
    assert(arr5.size() == 100);
    
//    dynamic_array<int> arr6(arr5);
}
