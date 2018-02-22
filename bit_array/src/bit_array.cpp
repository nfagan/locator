//
//  bit_array.cpp
//  bit_matrix
//
//  Created by Nick Fagan on 2/17/18.
//

#include "bit_array.hpp"
#include <stdexcept>
#include <cstring>
#include <cmath>

util::bit_array::bit_array()
{
    m_size = 0;
    m_size_int = sizeof(uint32_t) * 8u;
}

util::bit_array::bit_array(uint32_t size)
{
    m_size = size;
    m_size_int = sizeof(uint32_t) * 8u;
    m_data.resize(get_data_size(size));
}

util::bit_array::~bit_array() noexcept
{
    //
}

//  copy-construct
util::bit_array::bit_array(const util::bit_array& other) : m_data(other.m_data)
{
    m_size = other.m_size;
    m_size_int = other.m_size_int;
}

//  copy-assign
util::bit_array& util::bit_array::operator=(const util::bit_array& other)
{
    util::bit_array tmp(other);
    *this = std::move(tmp);
    return *this;
}

//  move-construct
util::bit_array::bit_array(util::bit_array&& rhs) noexcept
{
    m_data = std::move(rhs.m_data);
    m_size = rhs.m_size;
    m_size_int = rhs.m_size_int;
    
    rhs.m_data = 0;
    rhs.m_size = 0;
}

//  move-assign
util::bit_array& util::bit_array::operator=(util::bit_array&& rhs) noexcept
{
    m_data = std::move(rhs.m_data);
    m_size = rhs.m_size;
    m_size_int = rhs.m_size_int;
    
    rhs.m_data = 0;
    rhs.m_size = 0;
    
    return *this;
}

void util::bit_array::push(bool value)
{
    uint32_t bin = get_bin(m_size);
    uint32_t bit = get_bit(m_size);
    
    if (bit == 0)
    {
        m_data.push(0);
    }
    
    unchecked_place(value, bin, bit);
    
    m_size++;
}

void util::bit_array::place(bool value, uint32_t at_index)
{
    if (at_index > m_size-1)
    {
        throw std::runtime_error("Index exceeds array dimensions.");
    }
    
    unchecked_place(value, at_index);
}

void util::bit_array::unchecked_place(bool value, uint32_t at_index)
{
    uint32_t bin = get_bin(at_index);
    uint32_t bit = get_bit(at_index);
    
    unchecked_place(value, bin, bit);
}

void util::bit_array::unchecked_place(bool value, uint32_t bin, uint32_t bit)
{
    uint32_t* data = m_data.unsafe_get_pointer();
    uint32_t current = data[bin];
    uint32_t one = 1u;
    
    if (value)
    {
        current = current | (one << bit);
    }
    else
    {
        current = current & ~(one << bit);
    }
    
    data[bin] = current;
}

void util::bit_array::unchecked_keep(const util::dynamic_array<uint32_t> &at_indices)
{
    uint32_t new_data_size = get_data_size(at_indices.size());
    
    uint32_t new_size = at_indices.tail();
    
    util::dynamic_array<uint32_t> tmp(new_data_size);
    
    uint32_t* tmp_ptr = tmp.unsafe_get_pointer();
    uint32_t* data_ptr = m_data.unsafe_get_pointer();
    uint32_t* at_indices_ptr = at_indices.unsafe_get_pointer();
    
    std::memset(tmp_ptr, 0u, new_data_size * sizeof(uint32_t));
    
    for (uint32_t i = 0; i < new_size; i++)
    {
        uint32_t idx = at_indices_ptr[i];
        uint32_t datum = data_ptr[get_bin(idx)];
        uint32_t bit = get_bit(idx);
        uint32_t into_bin = get_bin(i);
        uint32_t into_bit = get_bit(i);
        
        bool res = datum & (1u << bit);
        
        if (res)
        {
            tmp_ptr[into_bin] |= (1u << into_bit);
        }
    }
    
    m_data = std::move(tmp);
    m_size = new_size;
}

void util::bit_array::append(const util::bit_array &other)
{
    if (other.m_size == 0)
    {
        return;
    }
    
    uint32_t other_orig_data_size = other.m_data.size();
    uint32_t orig_data_size = m_data.size();
    uint32_t new_data_size = orig_data_size + other_orig_data_size;
    uint32_t orig_size = m_size;
    
    m_data.resize(new_data_size);
    
    uint32_t* m_data_ptr = m_data.unsafe_get_pointer();
    uint32_t* other_data_ptr = other.m_data.unsafe_get_pointer();
    
    uint32_t last_bit = get_bit(orig_size);
    
    uint32_t orig_tail = get_data_size(orig_size);
    uint32_t other_tail = get_data_size(other.m_size);
    
    m_size += other.m_size;
    
    //  fast copy of elements if they're already aligned.
    if (last_bit == 0)
    {
        std::memcpy(&m_data_ptr[orig_tail], other_data_ptr, other_tail * sizeof(uint32_t));
        return;
    }
    
    std::memset(&m_data_ptr[orig_tail], 0u, other_tail * sizeof(uint32_t));
    
    uint32_t bit_offset = m_size_int - last_bit;
    
    std::cout << bit_offset << std::endl;
    
    for (uint32_t i = 0; i < other_tail-1; i++)
    {
        uint32_t other0 = other_data_ptr[i];
        uint32_t other1 = other_data_ptr[i+1];
        
        other0 = other0 >> bit_offset;
        other1 = other1 << bit_offset;
        
//        other0 = other0 & ~((1u << bit_offset) - 1u);
//        other1 = other1 & ~((1u << bit_offset) - 1u);
        
        m_data_ptr[orig_tail-1 + i] |= other0;
        m_data_ptr[orig_tail + i] |= other1;
        
//        m_data_ptr[orig_tail-1 + i] |= (other_data_ptr[i] << bit_offset);
//        m_data_ptr[orig_tail + i] |= (other_data_ptr[i+1] << bit_offset);
    }
}

void util::bit_array::fill(bool with)
{
    uint32_t fill_with = with ? ~(0u) : 0u;
    uint32_t fill_to = get_data_size(m_size);
    std::memset(m_data.unsafe_get_pointer(), fill_with, fill_to * sizeof(uint32_t));
}

bool util::bit_array::at(uint32_t index) const
{
    uint32_t bin = get_bin(index);
    uint32_t bit = get_bit(index);
    
    return m_data.at(bin) & (1u << bit);
}

uint32_t util::bit_array::size() const
{
    return m_size;
}

uint32_t util::bit_array::get_bin(uint32_t index) const
{
    return index / m_size_int;
}

uint32_t util::bit_array::get_bit(uint32_t index) const
{
    return index % m_size_int;
}

uint32_t util::bit_array::get_data_size(uint32_t n_elements) const
{
    double res = double(n_elements) / double(m_size_int);
    return uint32_t(std::ceil(res));
}

bool util::bit_array::all_bits_set(uint32_t value, uint32_t n)
{
    uint32_t mask = (1u << n) - 1;
    value &= mask;
    return value == mask;
}

void util::bit_array::unchecked_dot_or(util::bit_array &out,
                                       const util::bit_array &a,
                                       const util::bit_array &b,
                                       uint32_t start,
                                       uint32_t stop)
{
    uint32_t first_bin = a.get_bin(start);
    uint32_t last_bin = a.get_bin(stop);
    
    uint32_t* a_data = a.m_data.unsafe_get_pointer();
    uint32_t* b_data = b.m_data.unsafe_get_pointer();
    uint32_t* out_data = out.m_data.unsafe_get_pointer();
    
    for (size_t i = first_bin; i <= last_bin; i++)
    {
        out_data[i] = a_data[i] | b_data[i];
    }
}

void util::bit_array::unchecked_dot_and(util::bit_array &out,
                                        const util::bit_array &a,
                                        const util::bit_array &b,
                                        uint32_t start,
                                        uint32_t stop)
{
    uint32_t first_bin = a.get_bin(start);
    uint32_t last_bin = a.get_bin(stop);
    
    uint32_t* a_data = a.m_data.unsafe_get_pointer();
    uint32_t* b_data = b.m_data.unsafe_get_pointer();
    uint32_t* out_data = out.m_data.unsafe_get_pointer();
    
    for (size_t i = first_bin; i <= last_bin; i++)
    {
        out_data[i] = a_data[i] & b_data[i];
    }
}

void util::bit_array::unchecked_dot_eq(util::bit_array &out,
                                        const util::bit_array &a,
                                        const util::bit_array &b,
                                        uint32_t start,
                                        uint32_t stop)
{
    uint32_t first_bin = a.get_bin(start);
    uint32_t last_bin = a.get_bin(stop);
    
    uint32_t* a_data = a.m_data.unsafe_get_pointer();
    uint32_t* b_data = b.m_data.unsafe_get_pointer();
    uint32_t* out_data = out.m_data.unsafe_get_pointer();
    
    for (size_t i = first_bin; i <= last_bin; i++)
    {
        out_data[i] = ~(a_data[i] ^ b_data[i]);
    }
}

void util::bit_array::dot_or(util::bit_array &out,
                             const util::bit_array &a,
                             const util::bit_array &b)
{
    binary_check_dimensions(out, a, b);
    unchecked_dot_or(out, a, b, 0, a.m_size);
}

void util::bit_array::dot_and(util::bit_array &out,
                             const util::bit_array &a,
                             const util::bit_array &b)
{
    binary_check_dimensions(out, a, b);
    unchecked_dot_and(out, a, b, 0, a.m_size);
}

bool util::bit_array::unchecked_all(const util::bit_array &a, uint32_t start, uint32_t stop)
{
    uint32_t first_bin = a.get_bin(start);
    uint32_t first_bit = a.get_bit(start);
    uint32_t last_bin = a.get_bin(stop);
    uint32_t last_bit = a.get_bit(stop);
    
    uint32_t* a_data = a.m_data.unsafe_get_pointer();
    
    if (!util::bit_array::all_bits_set(a_data[first_bin], first_bit))
    {
        return false;
    }
    
    for (size_t i = first_bin + 1; i < last_bin; i++)
    {
        if (!util::bit_array::all_bits_set(a_data[i], a.m_size_int))
        {
            return false;
        }
    }
    
    return util::bit_array::all_bits_set(a_data[last_bin], last_bit);
}

bool util::bit_array::all(const util::bit_array &a)
{
    return util::bit_array::unchecked_all(a, 0, a.m_size);
}

void util::bit_array::binary_check_dimensions(const util::bit_array &out,
                                              const util::bit_array &a,
                                              const util::bit_array &b)
{
    if (a.size() != b.size() || a.size() != out.size())
    {
        throw std::runtime_error("Dimension mismatch.");
    }
}
