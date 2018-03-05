#include "mex.h"
#include "loc_api.hpp"
#include <iostream>
#include <cstdint>
#include <unordered_map>

namespace util {
    static storage_t locators;
    static uint32_t next_id = 0;
}

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    if (nrhs == 0)
    {
        mexErrMsgIdAndTxt("locator:main", "Not enough input arguments.");
        return;
    }
    
    util::assert_scalar(prhs[0], "locator:main", "Op-code must be scalar.");
    
    uint32_t op_code = (uint32_t) mxGetScalar(prhs[0]);  
    
    switch (op_code)
    {
        case util::ops::CREATE:
            util::create(nlhs, plhs, nrhs, prhs);
            return;
            
        case util::ops::DESTROY:
            util::destroy(nlhs, plhs, nrhs, prhs);
            return;
            
        case util::ops::FIND:
            util::find(nlhs, plhs, nrhs, prhs);
            return;
            
        case util::ops::ADD_CATEGORY:
            util::add_category(nlhs, plhs, nrhs, prhs);
            return;
            
        case util::ops::SET_CATEGORY:
            util::set_category(nlhs, plhs, nrhs, prhs);
            return;
            
        case util::ops::HAS_CATEGORY:
            util::has_category(nlhs, plhs, nrhs, prhs);
            return;
            
        case util::ops::RM_CATEGORY:
            util::rm_category(nlhs, plhs, nrhs, prhs);
            return;
            
        case util::ops::GET_CATEGORIES:
            util::get_categories(nlhs, plhs, nrhs, prhs);
            return;
            
        case util::ops::GET_LABELS:
            util::get_labels(nlhs, plhs, nrhs, prhs);
            return;
            
        case util::ops::IS_LOCATOR:
            util::is_locator(nlhs, plhs, nrhs, prhs);
            return;
            
        case util::ops::REQUIRE_CATEGORY:
            util::require_category(nlhs, plhs, nrhs, prhs);
            return;
            
        case util::ops::SIZE:
            util::size(nlhs, plhs, nrhs, prhs);
            return;
            
        case util::ops::COPY:
            util::copy(nlhs, plhs, nrhs, prhs);
            return;
            
        case util::ops::INSTANCES:
            util::instances(nlhs, plhs, nrhs, prhs);
            return;
            
        default:
            mexErrMsgIdAndTxt("locator:main", "Unrecognized op-code.");
    }
    
}

//
//  implementation
//

void util::copy_entries_into_array(types::entries_t& src, mxArray* dest, uint32_t n_copy)
{
    uint32_t* src_ptr = src.unsafe_get_pointer();
    uint32_t* dest_ptr = (uint32_t*) mxGetData(dest);
    
    std::memcpy(dest_ptr, src_ptr, n_copy * sizeof(uint32_t));
}

mxArray* util::make_entries_into_array(types::entries_t& src, uint32_t n_copy)
{
    mxArray* out = mxCreateUninitNumericMatrix(n_copy, 1, mxUINT32_CLASS, mxREAL);
    
    util::copy_entries_into_array(src, out, n_copy);
    
    return out;
}

util::locator& util::get_locator(uint32_t id)
{
    auto it = util::locators.find(id);
    
    if (it == util::locators.end())
    {
        mexErrMsgIdAndTxt("locator:get_locator", "Unrecognized id.");
    }
    
    return it->second;
}

void util::assert_scalar(const mxArray *arr, const char* id, const char* msg)
{
    if (!mxIsScalar(arr))
    {
        mexErrMsgIdAndTxt(id, msg);
    }
}

void util::assert_isa(const mxArray *arr, unsigned int class_id, const char* id, const char* msg)
{
    if (mxGetClassID(arr) != class_id)
    {
        mexErrMsgIdAndTxt(id, msg);
        return;
    }
}

void util::instances(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    if (nlhs == 0)
    {
        return;
    }
    
    if (nrhs != 1)
    {
        mexErrMsgIdAndTxt("locator:instances", "Wrong number of inputs.");
        return;
    }
    
    uint32_t n_instances = (uint32_t) util::locators.size();
    
    if (n_instances == 0)
    {
        plhs[0] = mxCreateUninitNumericMatrix(0, 1, mxUINT32_CLASS, mxREAL);
        return;
    }
    
    util::types::entries_t ids(n_instances);
    
    ids.seek_tail_to_start();
    
    for (const auto& it : util::locators)
    {
        ids.push(it.first);
    }
    
    plhs[0] = make_entries_into_array(ids, n_instances);
}

void util::copy(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    if (nlhs == 0)
    {
        return;
    }
    
    if (nrhs != 2)
    {
        mexErrMsgIdAndTxt("locator:copy", "Wrong number of inputs.");
        return;
    }
    
    uint32_t in_id = mxGetScalar(prhs[1]);
    
    util::locator& c_locator = util::get_locator(in_id);
    
    util::locator loc_copy = c_locator;
    
    uint32_t out_id = util::next_id;
    
    util::locators[out_id] = std::move(loc_copy);
    
    plhs[0] = mxCreateUninitNumericMatrix(1, 1, mxUINT32_CLASS, mxREAL);
    uint32_t* out_ptr = (uint32_t*) mxGetData(plhs[0]);
    out_ptr[0] = out_id;
    
    util::next_id++;
}

void util::size(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    if (nlhs == 0)
    {
        return;
    }
    
    if (nrhs != 2)
    {
        mexErrMsgIdAndTxt("locator:size", "Wrong number of inputs.");
        return;
    }
    
    uint32_t id = (uint32_t) mxGetScalar(prhs[1]);
    
    util::locator& c_locator = util::get_locator(id);
    
    uint32_t sz = c_locator.size();
    
    plhs[0] = mxCreateUninitNumericMatrix(1, 1, mxUINT32_CLASS, mxREAL);
    uint32_t* out = (uint32_t*) mxGetData(plhs[0]);
    out[0] = sz;
}

void util::is_locator(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    if (nlhs == 0)
    {
        return;
    }
    
    bool exists = false;
    
    if (nrhs > 1)
    {
        uint32_t id = (uint32_t) mxGetScalar(prhs[1]);
        exists = util::locators.find(id) != util::locators.end();
    }
    
    plhs[0] = mxCreateLogicalScalar(exists);
}

void util::require_category(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    if (nrhs != 3)
    {
        mexErrMsgIdAndTxt("locator:require_category", "Wrong number of inputs.");
        return;
    }
    
    const mxArray* in_id = prhs[1];
    const mxArray* in_cats = prhs[2];
    
    uint32_t id = mxGetScalar(in_id);
    size_t n_cats = mxGetNumberOfElements(in_cats);    
    
    util::locator& c_locator = util::get_locator(id);
    
    uint32_t* in_cats_ptr = (uint32_t*) mxGetData(in_cats);
    
    for (uint32_t i = 0; i < n_cats; i++)
    {
        c_locator.require_category(in_cats_ptr[i]);
    }
}

void util::rm_category(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    if (nrhs != 3)
    {
        mexErrMsgIdAndTxt("locator:rm_category", "Wrong number of inputs.");
        return;
    }
    
    const mxArray* in_loc_id = prhs[1];
    const mxArray* in_categories = prhs[2];
    
    util::locator& c_locator = util::get_locator(mxGetScalar(in_loc_id));
    
    uint32_t* cats = (uint32_t*) mxGetData(in_categories);
    size_t n_els = mxGetNumberOfElements(in_categories);
    
    for (uint32_t i = 0; i < n_els; i++)
    {
        uint32_t res = c_locator.rm_category(cats[i]);
        
        std::cout << "OK" << std::endl;
        
        if (res == util::locator_status::OK)
        {
            continue;
        }
        
        if (res == util::locator_status::CATEGORY_DOES_NOT_EXIST)
        {
            mexErrMsgIdAndTxt("locator:rm_category", "Category does not exist.");
        }
    }
}

void util::has_category(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    if (nlhs == 0)
    {
        return;
    }
    
    if (nrhs != 3)
    {
        mexErrMsgIdAndTxt("locator:has_category", "Wrong number of inputs.");
        return;
    }
    
    const mxArray* in_loc_id = prhs[1];
    const mxArray* in_categories = prhs[2];
    
    util::locator& c_locator = util::get_locator(mxGetScalar(in_loc_id));
    
    uint32_t* cats = (uint32_t*) mxGetData(in_categories);
    size_t n_els = mxGetNumberOfElements(in_categories);
    
    mxArray* out_exists_arr = mxCreateLogicalMatrix(n_els, 1);
    mxLogical* out_ptr = mxGetLogicals(out_exists_arr);
    
    for (uint32_t i = 0; i < n_els; i++)
    {
        out_ptr[i] = (mxLogical) c_locator.has_category(cats[i]);
    }
    
    plhs[0] = out_exists_arr;
}

void util::find(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    if (nrhs != 3)
    {
        mexErrMsgIdAndTxt("locator:find", "Wrong number of inputs.");
        return;
    }
    
    if (nlhs != 1)
    {
        mexErrMsgIdAndTxt("locator:find", "Wrong number of outputs.");
        return;
    }
    
    util::assert_scalar(prhs[1], "locator:find", "Id must be scalar.");
    
    uint32_t loc_id = mxGetScalar(prhs[1]);
    
    util::locator& c_locator = util::get_locator(loc_id);
    
    size_t n_in = mxGetNumberOfElements(prhs[2]);
    
    if (n_in == 0)
    {
        plhs[0] = mxCreateUninitNumericMatrix(0, 1, mxUINT32_CLASS, mxREAL);
        return;
    }
    
    util::types::entries_t to_search(n_in);
    
    uint32_t* in_ptr = (uint32_t*) mxGetData(prhs[2]);
    uint32_t* search_ptr = to_search.unsafe_get_pointer();    
    
    std::memcpy(search_ptr, in_ptr, n_in * sizeof(uint32_t));
    
    util::types::entries_t result = c_locator.find(to_search, 1u);
    
    uint32_t n_found = result.tail();
    
    plhs[0] = mxCreateUninitNumericMatrix(n_found, 1, mxUINT32_CLASS, mxREAL);
    
    if (n_found == 0)
    {
        return;
    }
    
    uint32_t* result_ptr = result.unsafe_get_pointer();   
    uint32_t* out_ptr = (uint32_t*) mxGetData(plhs[0]);
    
    std::memcpy(out_ptr, result_ptr, n_found * sizeof(uint32_t));  
}

void util::get_categories(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    if (nrhs != 2)
    {
        mexErrMsgIdAndTxt("locator:get_categories", "Wrong number of inputs.");
        return;
    }
    
    if (nlhs != 1)
    {
        return;
    }
    
    util::assert_scalar(prhs[1], "locator:get_categories", "Id must be scalar.");
    
    uint32_t id = (uint32_t) mxGetScalar(prhs[1]);
    
    util::locator& c_locator = util::get_locator(id);
    
    const util::types::entries_t& cats = c_locator.get_categories();
    
    uint32_t* cat_ptr = cats.unsafe_get_pointer();
    uint32_t n_elements = cats.tail();
    
    plhs[0] = mxCreateUninitNumericMatrix(n_elements, 1, mxUINT32_CLASS, mxREAL);
    
    if (n_elements > 0)
    {
        uint32_t* out_ptr = (uint32_t*) mxGetData(plhs[0]);
        std::memcpy(out_ptr, cat_ptr, n_elements * sizeof(uint32_t));
    }
}

void util::get_labels(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    if (nrhs != 2)
    {
        mexErrMsgIdAndTxt("locator:get_labels", "Wrong number of inputs.");
        return;
    }
    
    if (nlhs != 1)
    {
        return;
    }
    
    util::assert_scalar(prhs[1], "locator:get_labels", "Id must be scalar.");
    
    uint32_t id = (uint32_t) mxGetScalar(prhs[1]);
    
    util::locator& c_locator = util::get_locator(id);
    
    const util::types::entries_t& labs = c_locator.get_labels();
    
    uint32_t* lab_ptr = labs.unsafe_get_pointer();
    uint32_t n_elements = labs.tail();
    
    plhs[0] = mxCreateUninitNumericMatrix(n_elements, 1, mxUINT32_CLASS, mxREAL);
    
    if (n_elements > 0)
    {
        uint32_t* out_ptr = (uint32_t*) mxGetData(plhs[0]);
        std::memcpy(out_ptr, lab_ptr, n_elements * sizeof(uint32_t));
    }
}

void util::add_category(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    if (nrhs != 3)
    {
        mexErrMsgIdAndTxt("locator:add_category", "Wrong number of inputs.");
        return;
    }
    
    if (nlhs > 0)
    {
        mexErrMsgIdAndTxt("locator:add_category", "Too many output arguments.");
        return;
    }
    
    util::assert_scalar(prhs[1], "locator:add_category", "Id must be scalar.");
    
    const mxArray* in_id = prhs[1];
    const mxArray* in_categories = prhs[2];
    
    uint32_t id = (uint32_t) mxGetScalar(in_id);
    uint32_t* categories = (uint32_t*) mxGetData(in_categories);
    
    size_t n_categories = mxGetNumberOfElements(in_categories);
    
    util::locator& c_locator = util::get_locator(id);
    
    for (uint32_t i = 0; i < n_categories; i++)
    {
        uint32_t result = c_locator.add_category(categories[i]);
        
        if (result == util::locator_status::OK)
        {
            continue;
        }
        
        if (result == util::locator_status::CATEGORY_EXISTS)
        {
            mexErrMsgIdAndTxt("locator:add_category", "Category already exists.");
        }
    }
}

void util::set_category(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    if (nrhs != 6)
    {
        mexErrMsgIdAndTxt("locator:set_category", "Wrong number of inputs.");
        return;
    }
    
    util::assert_scalar(prhs[1], "locator:set_category", "Id must be scalar.");
    util::assert_scalar(prhs[2], "locator:set_category", "Category must be scalar.");
    util::assert_scalar(prhs[3], "locator:set_category", "Label must be scalar.");
    util::assert_scalar(prhs[5], "locator:set_category", "Rows must be scalar.");
    
    uint32_t loc_id = (uint32_t) mxGetScalar(prhs[1]);
    uint32_t category = (uint32_t) mxGetScalar(prhs[2]);
    uint32_t label = (uint32_t) mxGetScalar(prhs[3]);
    uint32_t rows = (uint32_t) mxGetScalar(prhs[5]);
    
    util::locator& c_locator = util::get_locator(loc_id);
    
    if (!c_locator.is_empty() && c_locator.size() != rows)
    {
        mexErrMsgIdAndTxt("locator:set_category", "Indices exceed locator dimensions.");
        return;
    }
    
    const mxArray* in_indices_array = prhs[4];
    
    uint32_t* in_indices = (uint32_t*) mxGetData(in_indices_array);
    size_t n_els = mxGetNumberOfElements(in_indices_array);
    
    util::types::entries_t num_inds(n_els);
    
    uint32_t* inds_ptr = num_inds.unsafe_get_pointer();
    
    std::memcpy(inds_ptr, in_indices, n_els * sizeof(uint32_t));
    
    util::bit_array index(rows, false);
    
    bool assign_result = index.assign_true(num_inds, -1);
    
    if (!assign_result)
    {
        mexErrMsgIdAndTxt("locator:set_category", "Indices exceed locator dimensions.");
        return;
    }
    
    if (index.sum() != n_els)
    {
        std::cout << "ASSIGN FAILED" << std::endl;
    }
    
    uint32_t set_cat_result = c_locator.set_category(category, label, index);
    
    if (set_cat_result == util::locator_status::OK)
    {
        return;
    }
    
    if (set_cat_result == util::locator_status::CATEGORY_DOES_NOT_EXIST)
    {
        mexErrMsgIdAndTxt("locator:set_category", "Category does not exist.");
        return;
    }
    
    if (set_cat_result == util::locator_status::LABEL_EXISTS_IN_OTHER_CATEGORY)
    {
        mexErrMsgIdAndTxt("locator:set_category", "Label already exists in another category.");
        return;
    }
    
    if (set_cat_result == util::locator_status::WRONG_INDEX_SIZE)
    {
        mexErrMsgIdAndTxt("locator:set_category", "Indices exceed locator dimensions.");
        return;
    }
}

void util::create(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    if (nlhs == 0)
    {
        return;
    }
    
    uint32_t id = util::next_id;
    
    if (nrhs == 1)
    {
        util::locators[id] = std::move(util::locator());
    }
    else
    {
        uint32_t size_hint = (uint32_t) mxGetScalar(prhs[1]);
        util::locators[id] = std::move(util::locator(size_hint));
    }
    
    util::next_id++;
    
    plhs[0] = mxCreateUninitNumericMatrix(1, 1, mxUINT32_CLASS, mxREAL);
    uint32_t* out_ptr = (uint32_t*) mxGetData(plhs[0]);
    out_ptr[0] = id;
}

void util::destroy(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    if (nrhs == 1)
    {
        util::locators.clear();
        util::next_id = 0;
        return;
    }
    
    uint32_t id = (uint32_t) mxGetScalar(prhs[1]);
    
    size_t n_erased = util::locators.erase(id);
    
    if (n_erased != 1)
    {
        mexErrMsgIdAndTxt("locator:destroy", "Unrecognized id.");
    }
}