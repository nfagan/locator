//
//  allocators.hpp
//  locator
//
//  Created by Nick Fagan on 2/28/18.
//

#pragma once

#include <cstdint>
#include <new>

namespace util {
    template<typename T>
    class trivial_allocator;
    
    template<typename T>
    class dynamic_allocator;
}

//
//  trivial allocator
//

template<typename T>
class util::trivial_allocator
{
public:
    trivial_allocator() = delete;
    ~trivial_allocator() = delete;
    
    trivial_allocator(const trivial_allocator& other) = delete;
    trivial_allocator& operator=(const trivial_allocator& other) = delete;
    trivial_allocator(trivial_allocator&& rhs) noexcept = delete;
    trivial_allocator& operator=(trivial_allocator&& other) noexcept = delete;
    
    static T* create(uint32_t with_size);
    static T* allocate(uint32_t with_size);
    static T* resize(T* data, uint32_t to_size, uint32_t original_size);
    static void copy(T* dest, T* source, uint32_t sz);
    static void dispose(T* data);
    
    constexpr static bool is_valid_alloc_t = std::is_trivially_copyable<T>::value;
};

template<typename T>
T* util::trivial_allocator<T>::create(uint32_t with_size)
{
    if (with_size == 0)
    {
        return nullptr;
    }
    
    return allocate(with_size);
}

template<typename T>
T* util::trivial_allocator<T>::resize(T* data, uint32_t to_size, uint32_t original_size)
{
    size_t dest_size = to_size * sizeof(T);
    return (T*) std::realloc(data, dest_size);
}

template<typename T>
T* util::trivial_allocator<T>::allocate(uint32_t with_size)
{
    T* data = (T*) std::malloc(with_size * sizeof(T));
    
    if (data == nullptr)
    {
        throw std::bad_alloc();
    }
    
    return data;
}

template<typename T>
void util::trivial_allocator<T>::copy(T* dest, T* source, uint32_t sz)
{
    memcpy(dest, source, sz * sizeof(T));
}

template<typename T>
void util::trivial_allocator<T>::dispose(T* data)
{
    std::free(data);
}

//
//  dynamic allocator
//

template<typename T>
class util::dynamic_allocator
{
public:
    dynamic_allocator() = delete;
    ~dynamic_allocator() = delete;
    
    dynamic_allocator(const dynamic_allocator& other) = delete;
    dynamic_allocator& operator=(const dynamic_allocator& other) = delete;
    dynamic_allocator(dynamic_allocator&& rhs) noexcept = delete;
    dynamic_allocator& operator=(dynamic_allocator&& other) noexcept = delete;
    
    static T* create(uint32_t with_size);
    static T* allocate(uint32_t with_size);
    static T* resize(T* data, uint32_t to_size, uint32_t original_size);
    static void copy(T* dest, T* source, uint32_t sz);
    static void dispose(T* data);
    
    constexpr static bool is_valid_alloc_t = true;
};

template<typename T>
T* util::dynamic_allocator<T>::create(uint32_t with_size)
{
    if (with_size == 0)
    {
        return nullptr;
    }
    
    return allocate(with_size);
}

template<typename T>
T* util::dynamic_allocator<T>::resize(T* data, uint32_t to_size, uint32_t original_size)
{    
    T* new_data = allocate(to_size);
    
    uint32_t n_move = to_size > original_size ? original_size : to_size;
    
    for (uint32_t i = 0; i < n_move; i++)
    {
        new_data[i] = std::move(data[i]);
    }
    
    delete[] data;
    
    return new_data;
}

template<typename T>
T* util::dynamic_allocator<T>::allocate(uint32_t with_size)
{
    T* data = new T[with_size];
    
    if (data == nullptr)
    {
        throw std::bad_alloc();
    }
    
    return data;
}

template<typename T>
void util::dynamic_allocator<T>::dispose(T* data)
{
    delete[] data;
}

template<typename T>
void util::dynamic_allocator<T>::copy(T* dest, T* source, uint32_t sz)
{
    for (uint32_t i = 0; i < sz; i++)
    {
        dest[i] = source[i];
    }
}
