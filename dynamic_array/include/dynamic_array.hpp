//
//  dynamic_array.hpp
//  locator
//
//  Created by Nick Fagan on 2/15/18.
//

#pragma once

#include <cstdlib>
#include <cstring>
#include <new>
#include <iostream>

namespace util
{
    template<typename T>
    class dynamic_array;
}

template<typename T>
class util::dynamic_array
{
public:
    dynamic_array(unsigned long initial_size);
    dynamic_array();
    ~dynamic_array() noexcept;
    
    dynamic_array(const dynamic_array<T>& other);
    dynamic_array<T>& operator=(const dynamic_array<T>& other);
    dynamic_array(dynamic_array<T>&& rhs) noexcept;
    dynamic_array<T>& operator=(dynamic_array<T>&& other) noexcept;
    
    T at(unsigned long index) const;
    
    void push(T element);
    void place(T element, unsigned long at_index);
    void unchecked_place(T element, unsigned long at_index);
    void seek_tail_to_end();
    T* unsafe_get_pointer() const;
    
    void resize(unsigned long to_size);
    void clear();
    
    unsigned long size() const;
    unsigned long tail() const;
    
private:
    T* m_elements;
    unsigned long m_size;
    unsigned long m_tail;
    
    void dispose();
    T* create(unsigned long to_size);
    
    static unsigned long get_next_size_larger(unsigned long current_size);
    static T* allocate(unsigned long with_size);
};

//
//  impl
//

template<typename T>
util::dynamic_array<T>::dynamic_array(unsigned long initial_size)
{
    static_assert(std::is_trivially_copyable<T>::value, "Type must be trivially copyable.");
    m_tail = initial_size;
    m_size = initial_size;
    m_elements = create(initial_size);
}

template<typename T>
util::dynamic_array<T>::dynamic_array()
{
    static_assert(std::is_trivially_copyable<T>::value, "Type must be trivially copyable.");
    m_size = 0;
    m_tail = 0;
    m_elements = nullptr;
}

//  copy-construct
template<typename T>
util::dynamic_array<T>::dynamic_array(const util::dynamic_array<T>& other)
{
    if (other.m_size > 0)
    {
        m_elements = create(other.m_size);
        memcpy(m_elements, other.m_elements, other.m_size * sizeof(T));
    }
    else
    {
        m_elements = nullptr;
    }
    
    m_size = other.m_size;
    m_tail = other.m_tail;
}

//  copy-assign
template<typename T>
util::dynamic_array<T>& util::dynamic_array<T>::operator=(const util::dynamic_array<T>& other)
{
    util::dynamic_array<T> tmp(other);
    *this = std::move(tmp);
    return *this;
}

//  move-construct
template<typename T>
util::dynamic_array<T>::dynamic_array(util::dynamic_array<T>&& rhs) noexcept
{
    m_elements = rhs.m_elements;
    m_size = rhs.m_size;
    m_tail = rhs.m_tail;
    
    rhs.m_size = 0;
    rhs.m_tail = 0;
    rhs.m_elements = nullptr;
}

//  move-assign
template<typename T>
util::dynamic_array<T>& util::dynamic_array<T>::operator=(util::dynamic_array<T>&& rhs) noexcept
{
    dispose();
    
    m_elements = rhs.m_elements;
    m_size = rhs.m_size;
    m_tail = rhs.m_tail;
    
    rhs.m_size = 0;
    rhs.m_tail = 0;
    rhs.m_elements = nullptr;
    
    return *this;
}

template<typename T>
util::dynamic_array<T>::~dynamic_array() noexcept
{
    dispose();
}

template<typename T>
T* util::dynamic_array<T>::create(unsigned long with_size)
{
    if (with_size == 0)
    {
        return nullptr;
    }
    
    return allocate(with_size);
}

template<typename T>
T util::dynamic_array<T>::at(unsigned long index) const
{
    return m_elements[index];
}

template<typename T>
void util::dynamic_array<T>::dispose()
{
    std::free(m_elements);
    m_size = 0;
    m_tail = 0;
    m_elements = nullptr;
}

template<typename T>
void util::dynamic_array<T>::clear()
{
    if (m_elements != nullptr)
    {
        dispose();
    }
}

template<typename T>
void util::dynamic_array<T>::resize(unsigned long to_size)
{
    size_t dest_size = to_size * sizeof(T);
    
    if (m_elements != nullptr)
    {
        m_elements = (T*) std::realloc(m_elements, dest_size);
    }
    else
    {
        m_elements = create(to_size);
    }
    
    if (to_size < m_tail)
    {
        m_tail = to_size;
    }
    
    m_size = to_size;
}

template<typename T>
unsigned long util::dynamic_array<T>::size() const
{
    return m_size;
}

template<typename T>
unsigned long util::dynamic_array<T>::tail() const
{
    return m_tail;
}

template<typename T>
void util::dynamic_array<T>::push(T element)
{
    if (m_tail == m_size)
    {
        resize(get_next_size_larger(m_size));
    }
    
    m_elements[m_tail] = element;
    
    m_tail++;
}

template<typename T>
unsigned long util::dynamic_array<T>::get_next_size_larger(unsigned long current_size)
{
    return current_size == 0 ? 1 : current_size * 2;
}

template<typename T>
void util::dynamic_array<T>::place(T element, unsigned long at_index)
{
    if (m_size == 0 || at_index > m_size)
    {
        throw std::runtime_error("Index exceeds array size.");
    }
    
    m_elements[at_index] = element;
}

template<typename T>
void util::dynamic_array<T>::unchecked_place(T element, unsigned long at_index)
{    
    m_elements[at_index] = element;
}

template<typename T>
T* util::dynamic_array<T>::unsafe_get_pointer() const
{
    return m_elements;
}

template<typename T>
void util::dynamic_array<T>::seek_tail_to_end()
{
    m_tail = m_size;
}

template<typename T>
T* util::dynamic_array<T>::allocate(unsigned long with_size)
{
    T* data = (T*) std::malloc(with_size * sizeof(T));
    
    if (data == nullptr)
    {
        throw std::bad_alloc();
    }
    
    return data;
}
