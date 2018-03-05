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
    m_size_int = get_size_int();
}

util::bit_array::bit_array(uint32_t size)
{
    m_size = size;
    m_size_int = get_size_int();
    m_data.resize(get_data_size(size));
    m_data.seek_tail_to_end();
}

util::bit_array::bit_array(uint32_t size, bool fill_with)
{
    m_size = size;
    m_size_int = get_size_int();
    m_data.resize(get_data_size(size));
    m_data.seek_tail_to_end();
    fill(fill_with);
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
util::bit_array::bit_array(util::bit_array&& rhs) noexcept :
    m_data(std::move(rhs.m_data))
{
    m_size = rhs.m_size;
    m_size_int = rhs.m_size_int;
    
    rhs.m_size = 0;
}

//  move-assign
util::bit_array& util::bit_array::operator=(util::bit_array&& rhs) noexcept
{
    m_data = std::move(rhs.m_data);
    m_size = rhs.m_size;
    m_size_int = rhs.m_size_int;
    
    rhs.m_size = 0;
    
    return *this;
}

void util::bit_array::push(bool value)
{
    uint32_t bin = get_bin(m_size);
    uint32_t bit = get_bit(m_size);
    
    if (bit == 0)
    {
        m_data.push(0u);
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
    
    if (value)
    {
        current = current | (1u << bit);
    }
    else
    {
        current = current & ~(1u << bit);
    }
    
    data[bin] = current;
}

void util::bit_array::keep(const util::dynamic_array<uint32_t>& at_indices)
{
    for (uint32_t i = 0; i < at_indices.tail(); i++)
    {
        if (at_indices.at(i) >= m_size)
        {
            throw std::runtime_error("Index exceeds array dimensions.");
        }
    }
    
    unchecked_keep(at_indices);
}

void util::bit_array::empty()
{
    m_data.clear();
    m_size = 0;
}

void util::bit_array::unchecked_keep(const util::dynamic_array<uint32_t> &at_indices)
{
    uint32_t new_size = at_indices.tail();
    
    if (new_size == 0)
    {
        empty();
        return;
    }
    
    uint32_t new_data_size = get_data_size(new_size);
    
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

bool util::bit_array::assign_true(const util::dynamic_array<uint32_t> &at_indices, int32_t index_offset)
{
    uint32_t* at_indices_data = at_indices.unsafe_get_pointer();
    uint32_t* own_data = m_data.unsafe_get_pointer();
    uint32_t indices_size = at_indices.tail();
    
    for (uint32_t i = 0; i < indices_size; i++)
    {
        uint32_t idx = at_indices_data[i] + index_offset;
        
        if (idx >= m_size)
        {
            return false;
        }
        
        uint32_t bin = get_bin(idx);
        uint32_t bit = get_bit(idx);
        
        own_data[bin] |= (1u << bit);
    }
    
    return true;
}


void util::bit_array::unchecked_assign_true(const util::dynamic_array<uint32_t> &at_indices, int32_t index_offset)
{
    uint32_t* at_indices_data = at_indices.unsafe_get_pointer();
    uint32_t* own_data = m_data.unsafe_get_pointer();
    uint32_t indices_size = at_indices.tail();
    
    for (uint32_t i = 0; i < indices_size; i++)
    {
        uint32_t idx = at_indices_data[i] + index_offset;
        uint32_t bin = get_bin(idx);
        uint32_t bit = get_bit(idx);
        
        own_data[bin] |= (1u << bit);
    }
}

void util::bit_array::append(const util::bit_array &other)
{
    if (other.m_size == 0)
    {
        return;
    }
    
    if (m_size == 0)
    {
        *this = other;
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
    
    //  fill remaining elements in final bin with 0
    uint32_t last_bin0 = ~(0u) >> bit_offset;
    
    m_data_ptr[orig_tail-1] &= last_bin0;

    for (uint32_t i = 0; i < other_tail; i++)
    {
        uint32_t other0 = other_data_ptr[i];
        uint32_t other1 = other0;

        other0 = other0 << last_bit;
        other1 = other1 >> bit_offset;

        m_data_ptr[orig_tail + i - 1] |= other0;
        m_data_ptr[orig_tail + i] |= other1;
    }
}

void util::bit_array::fill(bool with)
{
    uint32_t fill_with = with ? ~(0u) : 0u;
    uint32_t fill_to = get_data_size(m_size);
    std::memset(m_data.unsafe_get_pointer(), fill_with, fill_to * sizeof(uint32_t));
}

void util::bit_array::flip()
{
    uint32_t data_size = get_data_size(m_size);
    uint32_t* data = m_data.unsafe_get_pointer();
    
    for (uint32_t i = 0; i < data_size; i++)
    {
        data[i] = ~(data[i]);
    }
}

bool util::bit_array::at(uint32_t index) const
{
    uint32_t bin = get_bin(index);
    uint32_t bit = get_bit(index);
    
    return m_data.at(bin) & (1 << bit);
}

uint32_t util::bit_array::sum() const
{
    
    if (m_size == 0)
    {
        return 0u;
    }
    
    uint32_t c_sum = 0;
    uint32_t data_size = get_data_size(m_size);
    uint32_t* data = m_data.unsafe_get_pointer();
    
    for (size_t i = 0; i < data_size-1; i++)
    {
        c_sum += util::bit_array::bit_sum(data[i]);
    }
    
    //  only sum the active values in the final bin
    uint32_t last_datum = get_final_bin_with_zeros(data, data_size);
    
    c_sum += util::bit_array::bit_sum(last_datum);
    
    return c_sum;
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

uint32_t util::bit_array::get_final_bin_with_zeros() const
{
    uint32_t data_size = get_data_size(m_size);
    uint32_t* data = m_data.unsafe_get_pointer();
    
    return get_final_bin_with_zeros(data, data_size);
}

uint32_t util::bit_array::get_final_bin_with_zeros(uint32_t *data, uint32_t data_size) const
{
    uint32_t bit_offset = m_size_int - get_bit(m_size);
    
    uint32_t last_bin0 = ~(0u) >> bit_offset;
    uint32_t last_datum = data[data_size-1];
    
    last_datum &= last_bin0;
    
    return last_datum;
}

uint32_t util::bit_array::get_data_size(uint32_t n_elements) const
{
    double res = double(n_elements) / double(m_size_int);
    return uint32_t(std::ceil(res));
}

uint32_t util::bit_array::get_size_int() const
{
    return sizeof(uint32_t) * 8u;
}

bool util::bit_array::all_bits_set(uint32_t value, uint32_t n)
{
    uint32_t mask = (1 << n) - 1;
    value &= mask;
    return value == mask;
}

uint32_t util::bit_array::bit_sum(uint32_t i)
{
    //  https://stackoverflow.com/questions/109023/how-to-count-the-number-of-set-bits-in-a-32-bit-integer
    i = i - ((i >> 1) & 0x55555555);
    i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
    return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}

void util::bit_array::unchecked_dot_or(util::bit_array &out,
                                       const util::bit_array &a,
                                       const util::bit_array &b,
                                       uint32_t start,
                                       uint32_t stop)
{
    uint32_t first_bin = a.get_bin(start);
    uint32_t last_bit = a.get_bit(stop);
    uint32_t last_bin = last_bit == 0u ? a.get_bin(stop-1) : a.get_bin(stop);
    
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
    uint32_t last_bit = a.get_bit(stop);
    uint32_t last_bin = last_bit == 0u ? a.get_bin(stop-1) : a.get_bin(stop);
    
    uint32_t* a_data = a.m_data.unsafe_get_pointer();
    uint32_t* b_data = b.m_data.unsafe_get_pointer();
    uint32_t* out_data = out.m_data.unsafe_get_pointer();
    
    for (size_t i = first_bin; i <= last_bin; i++)
    {
        out_data[i] = a_data[i] & b_data[i];
    }
}

void util::bit_array::unchecked_dot_and_not(util::bit_array &out,
                                        const util::bit_array &a,
                                        const util::bit_array &b,
                                        uint32_t start,
                                        uint32_t stop)
{
    uint32_t first_bin = a.get_bin(start);
    uint32_t last_bit = a.get_bit(stop);
    uint32_t last_bin = last_bit == 0u ? a.get_bin(stop-1) : a.get_bin(stop);
    
    uint32_t* a_data = a.m_data.unsafe_get_pointer();
    uint32_t* b_data = b.m_data.unsafe_get_pointer();
    uint32_t* out_data = out.m_data.unsafe_get_pointer();
    
    for (size_t i = first_bin; i <= last_bin; i++)
    {
        out_data[i] = a_data[i] & ~(b_data[i]);
    }
}

void util::bit_array::unchecked_dot_eq(util::bit_array &out,
                                        const util::bit_array &a,
                                        const util::bit_array &b,
                                        uint32_t start,
                                        uint32_t stop)
{
    uint32_t first_bin = a.get_bin(start);
    uint32_t last_bit = a.get_bit(stop);
    uint32_t last_bin = last_bit == 0u ? a.get_bin(stop-1) : a.get_bin(stop);
    
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

bool util::bit_array::unchecked_all(uint32_t start, uint32_t stop) const
{
    if (m_size == 0)
    {
        return false;
    }
    
    uint32_t first_bin = get_bin(start);
    uint32_t first_bit = get_bit(start);
    uint32_t last_bin = get_bin(stop);
    uint32_t last_bit = get_bit(stop);
    
    uint32_t* a_data = m_data.unsafe_get_pointer();
    
    uint32_t n_check_first = first_bit == 0u ? m_size_int : first_bit;
    uint32_t n_check_last = first_bit == 0u ? m_size_int : last_bit;
    
    if (util::bit_array::bit_sum(a_data[first_bin]) != n_check_first)
    {
        return false;
    }
    
    uint32_t stop_idx = last_bit == 0u ? last_bin-1 : last_bin;
    
    for (size_t i = first_bin + 1; i < stop_idx; i++)
    {
        uint32_t n_check = (i < stop_idx-1) ? m_size_int : n_check_last;
        
        if (util::bit_array::bit_sum(a_data[i]) != n_check)
        {
            return false;
        }
    }
    
    return true;
}

bool util::bit_array::all() const
{
    return unchecked_all(0, m_size);
}

bool util::bit_array::any() const
{
    if (m_size == 0)
    {
        return false;
    }
    
    uint32_t* a_data = m_data.unsafe_get_pointer();
    uint32_t data_size = get_data_size(m_size);
    
    for (uint32_t i = 0; i < data_size-1; i++)
    {
        if (a_data[i] != 0u)
        {
            return true;
        }
    }
    
    //  make sure the bits beyond `m_size` are zeroed
    uint32_t last_datum = get_final_bin_with_zeros(a_data, data_size);
    
    return last_datum != 0u;
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

util::dynamic_array<uint32_t> util::bit_array::find(const util::bit_array &a, uint32_t index_offset)
{
    uint32_t n_true = a.sum();
    
    util::dynamic_array<uint32_t> result(n_true);
    
    result.seek_tail_to_start();
    
    if (n_true == 0)
    {
        return result;
    }
    
    uint32_t data_size = a.get_data_size(a.m_size);
    uint32_t last_bit = a.get_bit(a.m_size);
    uint32_t* data = a.m_data.unsafe_get_pointer();
    uint32_t size_int = a.m_size_int;
    
    for (size_t i = 0; i < data_size; i++)
    {
        uint32_t datum = data[i];
        
        if (datum == 0u)
        {
            continue;
        }
        
        uint32_t stop_bit;
        
        if (i < data_size - 1)
        {
            stop_bit = a.m_size_int;
        }
        else
        {
            stop_bit = last_bit == 0 ? a.m_size_int : last_bit;
        }
        
        for (uint32_t j = 0; j < stop_bit; j++)
        {
            if (datum & (1u << j))
            {
                result.push(i * size_int + j + index_offset);
            }
        }
    }
    
    return result;
}
