//
//  bit_array.hpp
//  bit_matrix
//
//  Created by Nick Fagan on 2/17/18.
//

#pragma once

#include "dynamic_array.hpp"
#include <cstdint>

namespace util {
    class bit_array;
}

class util::bit_array
{
public:
    struct iterator
    {
        iterator(const bit_array* barray);
        void next();
        bool value() const;
        void set(bool value);
    private:
        uint32_t* m_data;
        uint32_t m_idx;
        uint32_t m_bin;
        uint32_t m_bit;
        uint32_t m_size_int;
    };
    
    explicit bit_array();
    explicit bit_array(uint32_t size);
    explicit bit_array(uint32_t size, bool fill_with);
    ~bit_array() noexcept;
    
    bit_array(const bit_array& other);
    bit_array& operator=(const bit_array& other);
    bit_array(bit_array&& rhs) noexcept;
    bit_array& operator=(bit_array&& other) noexcept;
    
    bit_array::iterator begin() const;
    
    uint32_t size() const;
    uint32_t sum() const;
    
    void resize(uint32_t to_size);
    
    void push(bool value);
    void place(bool value, uint32_t at_index);
    void unchecked_place(bool value, uint32_t at_index);
    void append(const bit_array &other);
    void keep(const util::dynamic_array<uint32_t> &at_indices);
    void unchecked_keep(const util::dynamic_array<uint32_t> &at_indices, int32_t index_offset = 0);
    
    bool assign_true(const util::dynamic_array<uint32_t> &at_indices, int32_t index_offset = 0);
    void unchecked_assign_true(const util::dynamic_array<uint32_t> &at_indices, int32_t index_offset = 0);
    
    bool at(uint32_t index) const;
    
    void fill(bool value);
    void empty();
    
    void flip();
    
    bool all() const;
    bool any() const;
    
    static void dot_or(bit_array& out, const bit_array& a, const bit_array& b);
    static void dot_and(bit_array& out, const bit_array& a, const bit_array& b);
    static void unchecked_dot_or(bit_array& out, const bit_array& a,
                                 const bit_array& b, uint32_t start, uint32_t stop);
    static void unchecked_dot_and(bit_array& out, const bit_array& a,
                                  const bit_array& b, uint32_t start, uint32_t stop);
    static void unchecked_dot_and_not(bit_array& out, const bit_array& a,
                                  const bit_array& b, uint32_t start, uint32_t stop);
    static void unchecked_dot_eq(bit_array& out, const bit_array& a,
                                  const bit_array& b, uint32_t start, uint32_t stop);
    
    static util::dynamic_array<uint32_t> find(const bit_array& a, uint32_t index_offset = 0u);
private:
    util::dynamic_array<uint32_t> m_data;
    
    uint32_t m_size;
    uint32_t m_size_int;
    
    uint32_t get_bin(uint32_t index) const;
    uint32_t get_bit(uint32_t index) const;
    uint32_t get_data_size(uint32_t n_elements) const;
    uint32_t get_final_bin_with_zeros() const;
    uint32_t get_final_bin_with_zeros(uint32_t* data, uint32_t data_size) const;
    uint32_t get_size_int() const;
    
    void unchecked_place(bool value, uint32_t bin, uint32_t bit);
    
    static void binary_check_dimensions(const bit_array& out, const bit_array& a, const bit_array& b);
    static bool all_bits_set(uint32_t value, uint32_t n);
    static uint32_t bit_sum(uint32_t i);
};
