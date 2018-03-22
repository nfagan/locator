#pragma once

#include "mex.h"
#include "locator.hpp"
#include <functional>
#include <string>

namespace util {
    typedef std::function<void(int, mxArray**, int, const mxArray**)> mex_func_t;
    
    std::string get_string(const mxArray* in_arr, bool* success);
    
    void assert_nlhs(int actual, int expected, const char* id);
    void assert_nlhs(int minimum, int maximum, int actual, const char* id);
    void assert_nrhs(int actual, int expected, const char* id);
    void assert_nrhs(int minimum, int maximum, int actual, const char* id);
    void assert_scalar(const mxArray *arr, const char* id, const char* msg);
    void assert_isa(const mxArray *arr, unsigned int class_id, const char* id, const char* msg);
    
    util::types::entries_t copy_array_into_entries(const mxArray* src, uint32_t n_copy);
    util::types::entries_t copy_array_into_entries(const mxArray* src);
    void copy_entries_into_array(const types::entries_t& src, mxArray* dest, uint32_t n_copy);
    mxArray* make_entries_into_array(const types::entries_t& src, uint32_t n_copy);
}