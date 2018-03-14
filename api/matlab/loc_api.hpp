#pragma once

#include "mex.h"
#include "loc_mex_helpers.hpp"
#include "locator.hpp"
#include "loc_opcodes.hpp"
#include <unordered_map>
#include <string>
#include <functional>

namespace util {
    typedef std::unordered_map<uint32_t, util::locator> storage_t;
    
    util::locator& get_locator(uint32_t id);
    
    void init_locator_functions();
    
    void create(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    void destroy(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    
    void resize(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    
    void size(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    void count(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    void n_labels(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    
    void add_category(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    void set_category(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    void set_category_mult(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    void require_category(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    void rm_category(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    void collapse_category(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    void full_category(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    void fill_category(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    
    void is_full_category(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    
    void get_random_label(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    
    void combinations(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    
    void keep(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    void append(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
            
    void has_category(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    void has_label(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    
    void get_categories(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    void get_labels(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    
    void which_category(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    void in_category(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    
    void is_locator(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    
    void equals(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    
    void find(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    void find_all(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    
    void copy(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
    
    void instances(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
}
