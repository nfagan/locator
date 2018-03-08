#pragma once

#include "mex.h"
#include "locator.hpp"
#include "loc_opcodes.hpp"
#include <unordered_map>
#include <string>

namespace util {
    typedef std::unordered_map<uint32_t, util::locator> storage_t;
    
    util::locator& get_locator(uint32_t id);
    
    void create(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    void destroy(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    
    void size(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    
    void add_category(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    void set_category(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);    
    void require_category(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    void rm_category(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    
    void keep(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
            
    void has_category(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    void has_label(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    
    void get_categories(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    void get_labels(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    
    void which_category(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    
    void is_locator(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    
    void equals(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    
    void find(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    
    void copy(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    
    void instances(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    
    void assert_nrhs(int actual, int expected, const char* id);
    void assert_scalar(const mxArray *arr, const char* id, const char* msg);
    void assert_isa(const mxArray *arr, unsigned int class_id, const char* id, const char* msg);
    
    util::types::entries_t copy_array_into_entries(const mxArray* src, uint32_t n_copy);
    void copy_entries_into_array(const types::entries_t& src, mxArray* dest, uint32_t n_copy);
    mxArray* make_entries_into_array(types::entries_t& src, uint32_t n_copy);
}
