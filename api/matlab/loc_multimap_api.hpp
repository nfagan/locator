#pragma once

#include "mex.h"
#include "loc_mex_helpers.hpp"
#include "locator.hpp"
#include "loc_multimap_opcodes.hpp"
#include <cstdint>

namespace util {
    typedef util::multimap<std::string, uint32_t> loc_multimap_t;
    typedef std::unordered_map<uint32_t, loc_multimap_t> storage_t;
    
    util::loc_multimap_t& get_multimap(uint32_t id);
    
    void init_multimap_functions();
    
    void create(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    void destroy(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    void copy(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    
    void insert(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    void at(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    
    void size(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    
    void contains_kv(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    
    void instances(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    void is_multimap(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    
    void keys(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    void values(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    
    void assert_char_or_uint32_t_class(mxClassID id);
    void assert_char_or_uint32_t(const mxArray* in_arr);
    
    std::string get_string_or_err(const loc_multimap_t& map, uint32_t key);
    uint32_t get_uint32_t_or_err(const loc_multimap_t& map, mxArray* key_arr);
}