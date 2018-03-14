#include "mex.h"
#include "loc_api.hpp"
#include <iostream>
#include <cstdint>
#include <unordered_map>
#include <array>

namespace util {    
    namespace globals {
        util::storage_t locators;
        
        uint32_t next_id = 0;
        
        std::array<util::mex_func_t, util::ops::N_OPS> funcs;
        
        bool INITIALIZED = false;
    }
}

void util::init_locator_functions()
{
    using namespace util;
    
    if (globals::INITIALIZED)
    {
        return;
    }
    
    globals::funcs[ops::CREATE] =                   &util::create;
    globals::funcs[ops::DESTROY] =                  &util::destroy;
    globals::funcs[ops::FIND] =                     &util::find;
    globals::funcs[ops::ADD_CATEGORY] =             &util::add_category;
    globals::funcs[ops::SET_CATEGORY] =             &util::set_category;
    globals::funcs[ops::RM_CATEGORY] =              &util::rm_category;
    globals::funcs[ops::HAS_CATEGORY] =             &util::has_category;
    globals::funcs[ops::GET_CATEGORIES] =           &util::get_categories;
    globals::funcs[ops::GET_LABELS] =               &util::get_labels;
    globals::funcs[ops::IS_LOCATOR] =               &util::is_locator;
    globals::funcs[ops::REQUIRE_CATEGORY] =         &util::require_category;
    globals::funcs[ops::SIZE] =                     &util::size;
    globals::funcs[ops::KEEP] =                     &util::keep;
    globals::funcs[ops::COPY] =                     &util::copy;
    globals::funcs[ops::INSTANCES] =                &util::instances;
    globals::funcs[ops::WHICH_CATEGORY] =           &util::which_category;
    globals::funcs[ops::HAS_LABEL] =                &util::has_label;
    globals::funcs[ops::EQUALS] =                   &util::equals;
    globals::funcs[ops::APPEND] =                   &util::append;
    globals::funcs[ops::IN_CATEGORY] =              &util::in_category;
    globals::funcs[ops::COUNT] =                    &util::count;
    globals::funcs[ops::COLLAPSE_CATEGORY] =        &util::collapse_category;
    globals::funcs[ops::SET_CATEGORY_MULT] =        &util::set_category_mult;
    globals::funcs[ops::N_LABELS] =                 &util::n_labels;
    globals::funcs[ops::RESIZE] =                   &util::resize;
    globals::funcs[ops::COMBINATIONS] =             &util::combinations;
    globals::funcs[ops::FULL_CATEGORY] =            &util::full_category;
    globals::funcs[ops::GET_RANDOM_LABEL] =         &util::get_random_label;
    globals::funcs[ops::FILL_CATEGORY] =            &util::fill_category;
    globals::funcs[ops::FIND_ALL] =                 &util::find_all;
    
    globals::INITIALIZED = true;
    
    std::cout << std::endl << "Initialized locator api." << std::endl << std::endl;
}

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    //  store function pointers
    util::init_locator_functions();
    
    //  make sure at least one input (op code) is present
    util::assert_nrhs(1, 10, nrhs, "locator:main");
    
    util::assert_scalar(prhs[0], "locator:main", "Op-code must be scalar.");
    
    uint32_t op_code = mxGetScalar(prhs[0]);
    
    if (op_code >= util::ops::N_OPS)
    {
        mexErrMsgIdAndTxt("locator:main", "Invalid op-code.");
        return;
    }
    
    //  call function associated with the op-code
    util::globals::funcs[op_code](nlhs, plhs, nrhs, prhs);    
}

//
//  implementation
//

util::locator& util::get_locator(uint32_t id)
{
    auto it = util::globals::locators.find(id);
    
    if (it == util::globals::locators.end())
    {
        mexErrMsgIdAndTxt("locator:get_locator", "Unrecognized id.");
    }
    
    return it->second;
}

void util::fill_category(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    using namespace util;
    
    assert_nrhs(nrhs, 4, "locator:fill_category");
    assert_nlhs(nlhs, 0, "locator:fill_category");
    
    assert_scalar(prhs[1], "locator:fill_category", "Id must be scalar.");
    assert_scalar(prhs[2], "locator:fill_category", "Category must be scalar.");
    assert_scalar(prhs[3], "locator:fill_category", "Label must be scalar.");
    
    locator& c_locator = get_locator(mxGetScalar(prhs[1]));
    
    uint32_t cat = mxGetScalar(prhs[2]);
    uint32_t lab = mxGetScalar(prhs[3]);
    
    uint32_t res = c_locator.set_category(cat, lab, bit_array(c_locator.size(), true));
    
    if (res == locator_status::OK)
    {
        return;
    }
    
    if (res == locator_status::CATEGORY_DOES_NOT_EXIST)
    {
        mexErrMsgIdAndTxt("locator:fill_category", "Category does not exist.");
        return;
    }
    
    if (res == locator_status::LABEL_EXISTS_IN_OTHER_CATEGORY)
    {
        mexErrMsgIdAndTxt("locator:fill_category", "Label already exists in another category.");
        return;
    }
    
    mexErrMsgIdAndTxt("locator:fill_category", "Unexpected error.");
}

void util::get_random_label(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    using namespace util;
    
    assert_nrhs(nrhs, 2, "locator:get_random_label");
    assert_nlhs(nlhs, 1, "locator:get_random_label");
    
    assert_scalar(prhs[1], "locator:resize", "Id must be scalar.");
    
    const locator& c_locator = get_locator(mxGetScalar(prhs[1]));
    
    plhs[0] = mxCreateUninitNumericMatrix(1, 1, mxUINT32_CLASS, mxREAL);
    
    uint32_t* id_arr = (uint32_t*) mxGetData(plhs[0]);
    
    id_arr[0] = c_locator.get_random_label_id();  
}

void util::find_all(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    using namespace util;
    
    assert_nrhs(nrhs, 3, "locator:find_all");
    assert_nlhs(nlhs, 2, "locator:find_all");
    
    assert_scalar(prhs[1], "locator:find_all", "Id must be scalar.");
    
    locator& c_locator = get_locator(mxGetScalar(prhs[1]));
    
    const mxArray* in_cats = prhs[2];
    uint32_t n_in_cats = mxGetNumberOfElements(in_cats);
    
    const types::entries_t in_cats_entries = copy_array_into_entries(in_cats, n_in_cats);
    
    bool exists;
    
    const types::entries_t combs = c_locator.combinations(in_cats_entries, &exists);
    const uint32_t n_result = combs.tail();
    
    if (!exists)
    {
        mexErrMsgIdAndTxt("locator:find_all", "Category does not exist.");
        return;
    }
    
    if (n_result == 0)
    {
        plhs[0] = mxCreateCellMatrix(1, 0);
        plhs[1] = mxCreateUninitNumericMatrix(0, n_in_cats, mxUINT32_CLASS, mxREAL);
        return;
    }
    
    uint32_t n_indices = n_result / n_in_cats;
    
    mxArray* all_indices = mxCreateCellMatrix(1, n_indices);
    
    uint32_t* all_combs_ptr = combs.unsafe_get_pointer();
    
    types::entries_t labs_to_find(n_in_cats);
    uint32_t* labs_to_find_ptr = labs_to_find.unsafe_get_pointer();
    
    uint32_t cell_idx = 0;
    
    for (uint32_t i = 0; i < n_result; i += n_in_cats)
    {
        std::memcpy(labs_to_find_ptr, all_combs_ptr + i, n_in_cats * sizeof(uint32_t));
                
        types::entries_t one_find_result = c_locator.find(labs_to_find, 1u);
        
        mxArray* one_find_arr = make_entries_into_array(one_find_result, one_find_result.tail());
        
        mxSetCell(all_indices, cell_idx, one_find_arr);
        
        cell_idx++;
    }
    
    plhs[0] = all_indices;
    plhs[1] = make_entries_into_array(combs, n_result);
}

void util::combinations(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    using namespace util;
    
    assert_nrhs(nrhs, 3, "locator:combinations");
    assert_nlhs(nlhs, 1, "locator:combinations");
    
    assert_scalar(prhs[1], "locator:combinations", "Id must be scalar.");
    
    const locator& c_locator = get_locator(mxGetScalar(prhs[1]));
    
    const mxArray* in_cats = prhs[2];
    uint32_t n_in_cats = mxGetNumberOfElements(in_cats);
    
    const types::entries_t in_cats_entries = copy_array_into_entries(in_cats, n_in_cats);
    
    bool exists;
    
    const types::entries_t result = c_locator.combinations(in_cats_entries, &exists);
    const uint32_t n_result = result.tail();
    
    if (!exists)
    {
        mexErrMsgIdAndTxt("locator:combinations", "Category does not exist.");
        return;
    }
    
    if (n_result == 0)
    {
        plhs[0] = mxCreateUninitNumericMatrix(0, n_in_cats, mxUINT32_CLASS, mxREAL);
        return;
    }
    
    plhs[0] = make_entries_into_array(result, n_result);
}

void util::full_category(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    using namespace util;
    
    assert_nrhs(nrhs, 3, "locator:full_category");
    assert_nlhs(nlhs, 1, "locator:full_category");
    
    assert_scalar(prhs[1], "locator:full_category", "Id must be scalar.");
    
    const mxArray* in_cats = prhs[2];
    const uint32_t n_cats = mxGetNumberOfElements(in_cats);
    
    const locator& c_locator = get_locator(mxGetScalar(prhs[1]));
    
    uint32_t* cats = (uint32_t*) mxGetData(in_cats);
    
    for (uint32_t i = 0; i < n_cats; i++)
    {
        if (!c_locator.has_category(cats[i]))
        {
            mexErrMsgIdAndTxt("locator:full_category", "Category does not exist.");
        }
    }
    
    if (c_locator.is_empty())
    {
        plhs[0] = mxCreateUninitNumericMatrix(0, n_cats, mxUINT32_CLASS, mxREAL);
        return;
    }
    
    uint32_t sz = c_locator.size();
    
    types::entries_t linear_indices(sz * n_cats);
    uint32_t* linear_indices_ptr = linear_indices.unsafe_get_pointer();
    
    for (uint32_t i = 0; i < n_cats; i++)
    {
        bool dummy;
        auto one_cat = c_locator.full_category(cats[i], &dummy);
        
        uint32_t* src_ptr = one_cat.unsafe_get_pointer();
        uint32_t* dest_ptr = linear_indices_ptr + (i * sz);
        
        std::memcpy(dest_ptr, src_ptr, sz * sizeof(uint32_t));         
    }
    
    plhs[0] = make_entries_into_array(linear_indices, sz * n_cats);
}

void util::resize(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    using namespace util;
    
    assert_nrhs(nrhs, 3, "locator:resize");
    assert_nlhs(nlhs, 0, "locator:resize");
    
    assert_scalar(prhs[1], "locator:resize", "Id must be scalar.");
    assert_scalar(prhs[2], "locator:resize", "New size must be scalar.");
    
    locator& c_locator = get_locator(mxGetScalar(prhs[1]));
    
    uint32_t to_size = mxGetScalar(prhs[2]);
    
    c_locator.resize(to_size);
}

void util::n_labels(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    using namespace util;
    
    assert_nrhs(nrhs, 2, "locator:n_labels");
    assert_nlhs(nlhs, 1, "locator:n_labels");
    
    assert_scalar(prhs[1], "locator:n_labels", "Id must be scalar.");
    
    const locator& c_locator = get_locator(mxGetScalar(prhs[1]));
    
    plhs[0] = mxCreateUninitNumericMatrix(1, 1, mxUINT32_CLASS, mxREAL);
    
    uint32_t* data = (uint32_t*) mxGetData(plhs[0]);
    
    data[0] = c_locator.n_labels();
}

void util::collapse_category(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    using namespace util;
    
    assert_nrhs(nrhs, 3, "locator:collapse_category");
    assert_nlhs(nlhs, 0, "locator:collapse_category");
    
    const mxArray* in_id = prhs[1];
    const mxArray* in_cats = prhs[2];
    
    assert_scalar(in_id, "locator:collapse_category", "Id must be scalar.");
    
    locator& c_locator = get_locator(mxGetScalar(in_id));
    
    uint32_t n_cats = mxGetNumberOfElements(in_cats);
    uint32_t* in_cats_ptr = (uint32_t*) mxGetData(in_cats);
    
    bool throw_error = false;
    
    for (uint32_t i = 0; i < n_cats; i++)
    {
        uint32_t status = c_locator.collapse_category(in_cats_ptr[i]);
        
        if (status == locator_status::OK)
        {
            continue;
        }
        
        if (status == locator_status::CATEGORY_DOES_NOT_EXIST)
        {
            throw_error = true;
            continue;
        }
    }
    
    if (throw_error)
    {
        mexErrMsgIdAndTxt("locator:collapse_category", "Category does not exist.");
    }
}

void util::count(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    using namespace util;
    
    assert_nrhs(nrhs, 3, "locator:count");
    assert_nlhs(nlhs, 1, "locator:count");
    
    const mxArray* in_id = prhs[1];
    const mxArray* in_labs = prhs[2];
    
    assert_scalar(in_id, "locator:count", "Id must be scalar.");
    
    const locator& c_locator = get_locator(mxGetScalar(in_id));
    
    uint32_t n_labs = mxGetNumberOfElements(in_labs);
    
    types::entries_t res(n_labs);
    
    uint32_t* res_ptr = res.unsafe_get_pointer();
    uint32_t* in_labs_ptr = (uint32_t*) mxGetData(in_labs);
    
    for (uint32_t i = 0; i < n_labs; i++)
    {
        res_ptr[i] = c_locator.count(in_labs_ptr[i]);
    }
    
    plhs[0] = make_entries_into_array(res, n_labs);
}

void util::in_category(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    using namespace util;
    
    assert_nrhs(nrhs, 3, "locator:all_in_category");
    assert_nlhs(nlhs, 1, "locator:all_in_category");
    
    const mxArray* in_id = prhs[1];
    const mxArray* in_cat = prhs[2];
    
    assert_scalar(in_id, "locator:all_in_category", "Id must be scalar.");
    assert_scalar(in_cat, "locator:all_in_category", "Category must be scalar.");
    
    uint32_t id = mxGetScalar(in_id);
    uint32_t cat = mxGetScalar(in_cat);
    
    const locator& c_locator = get_locator(id);
    
    bool exists;
    
    const types::entries_t result = c_locator.all_in_category(cat, &exists);
    
    if (!exists)
    {
        mexErrMsgIdAndTxt("locator:all_in_category", "Category does not exist.");
        return;
    }
    
    plhs[0] = make_entries_into_array(result, result.tail());
}

void util::append(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    using namespace util;
    
    assert_nrhs(nrhs, 3, "locator:append");
    assert_nlhs(nlhs, 0, "locator:append");
    
    const mxArray* in_id_a = prhs[1];
    const mxArray* in_id_b = prhs[2];
    
    assert_scalar(in_id_a, "locator:which_category", "Id must be scalar.");
    assert_scalar(in_id_b, "locator:which_category", "Id must be scalar.");
    
    locator& c_locator_a = get_locator(mxGetScalar(in_id_a));
    const locator& c_locator_b = get_locator(mxGetScalar(in_id_b));
    
    uint32_t result = c_locator_a.append(c_locator_b);
    
    if (result == locator_status::OK)
    {
        return;
    }
    
    if (result == locator_status::CATEGORIES_DO_NOT_MATCH)
    {
        mexErrMsgIdAndTxt("locator:append", "Categories do not match between locators.");
    }
    
    if (result == locator_status::LOC_OVERFLOW)
    {
        mexErrMsgIdAndTxt("locator:append", "Append operation would result in overflow.");
    }
}

void util::equals(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    using namespace util;
    
    assert_nrhs(nrhs, 3, "locator:equals");
    
    if (nlhs != 1)
    {
        return;
    }
    
    const mxArray* in_id_a = prhs[1];
    const mxArray* in_id_b = prhs[2];
    
    assert_scalar(in_id_a, "locator:which_category", "Id must be scalar.");
    assert_scalar(in_id_b, "locator:which_category", "Id must be scalar.");
    
    locator& c_locator_a = get_locator(mxGetScalar(in_id_a));
    locator& c_locator_b = get_locator(mxGetScalar(in_id_b));
    
    bool is_equal = c_locator_a == c_locator_b;
    
    plhs[0] = mxCreateLogicalScalar(is_equal);
}

void util::which_category(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    using namespace util;
    
    assert_nrhs(nrhs, 3, "locator:which_category");
    
    if (nlhs != 1)
    {
        return;
    }
    
    const mxArray* in_id = prhs[1];
    const mxArray* in_labels = prhs[2];
    
    assert_scalar(in_id, "locator:which_category", "Id must be scalar.");
    
    locator& c_locator = get_locator(mxGetScalar(in_id));
    
    bool label_exists;
    
    uint32_t n_els = mxGetNumberOfElements(in_labels);
    
    types::entries_t res(n_els);
    
    uint32_t* res_ptr = res.unsafe_get_pointer();
    uint32_t* in_labels_ptr = (uint32_t*) mxGetData(in_labels);
    
    for (uint32_t i = 0; i < n_els; i++)
    {
        uint32_t in_category = c_locator.which_category(in_labels_ptr[i], &label_exists);
        
        if (!label_exists)
        {
            mexErrMsgIdAndTxt("locator:which_category", "Label does not exist.");
            return;
        }
        
        res_ptr[i] = in_category;
    }
    
    plhs[0] = make_entries_into_array(res, n_els);
}

void util::keep(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    using namespace util;
    
    assert_nrhs(nrhs, 3, "locator:keep");
    
    const mxArray* in_id = prhs[1];
    const mxArray* in_indices = prhs[2];
    
    assert_scalar(in_id, "locator:keep", "Id must be scalar.");
    
    uint32_t id = mxGetScalar(in_id);
    
    locator& c_locator = get_locator(id);
    
    uint32_t n_els = mxGetNumberOfElements(in_indices);
    
    if (n_els == 0)
    {
        c_locator.empty();
        return;
    }
    
    types::entries_t to_keep = copy_array_into_entries(in_indices, n_els);
    
    to_keep.unchecked_sort(n_els);
    
    uint32_t* to_keep_ptr = to_keep.unsafe_get_pointer();
    
    if (to_keep_ptr[0] < 1 || to_keep_ptr[n_els-1] > c_locator.size())
    {
        mexErrMsgIdAndTxt("locator:keep", "Indices exceed locator dimensions.");
    }
    
    const int32_t index_offset = -1;
    
    c_locator.unchecked_keep(to_keep, index_offset);
}

void util::instances(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    if (nlhs == 0)
    {
        return;
    }
    
    util::assert_nrhs(nrhs, 1, "locator:instances");
    
    uint32_t n_instances = (uint32_t) util::globals::locators.size();
    
    if (n_instances == 0)
    {
        plhs[0] = mxCreateUninitNumericMatrix(0, 1, mxUINT32_CLASS, mxREAL);
        return;
    }
    
    util::types::entries_t ids(n_instances);
    
    ids.seek_tail_to_start();
    
    for (const auto& it : util::globals::locators)
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
    
    util::assert_nrhs(nrhs, 2, "locator:copy");
    
    uint32_t in_id = mxGetScalar(prhs[1]);
    
    util::locator& c_locator = util::get_locator(in_id);
    
    util::locator loc_copy = c_locator;
    
    uint32_t out_id = util::globals::next_id;
    
    util::globals::locators[out_id] = std::move(loc_copy);
    
    plhs[0] = mxCreateUninitNumericMatrix(1, 1, mxUINT32_CLASS, mxREAL);
    uint32_t* out_ptr = (uint32_t*) mxGetData(plhs[0]);
    out_ptr[0] = out_id;
    
    util::globals::next_id++;
}

void util::size(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    if (nlhs == 0)
    {
        return;
    }
    
    util::assert_nrhs(nrhs, 2, "locator:size");
    
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
        exists = util::globals::locators.find(id) != util::globals::locators.end();
    }
    
    plhs[0] = mxCreateLogicalScalar(exists);
}

void util::require_category(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    util::assert_nrhs(nrhs, 3, "locator:require_category");
    
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
    util::assert_nrhs(nrhs, 3, "locator:rm_category");
    
    const mxArray* in_loc_id = prhs[1];
    const mxArray* in_categories = prhs[2];
    
    util::locator& c_locator = util::get_locator(mxGetScalar(in_loc_id));
    
    uint32_t* cats = (uint32_t*) mxGetData(in_categories);
    size_t n_els = mxGetNumberOfElements(in_categories);
    
    for (uint32_t i = 0; i < n_els; i++)
    {
        uint32_t res = c_locator.rm_category(cats[i]);
        
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

void util::has_label(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    if (nlhs == 0)
    {
        return;
    }
    
    util::assert_nrhs(nrhs, 3, "locator:has_label");
    
    const mxArray* in_loc_id = prhs[1];
    const mxArray* in_labels = prhs[2];
    
    util::locator& c_locator = util::get_locator(mxGetScalar(in_loc_id));
    
    uint32_t* labs = (uint32_t*) mxGetData(in_labels);
    size_t n_els = mxGetNumberOfElements(in_labels);
    
    mxArray* out_exists_arr = mxCreateLogicalMatrix(n_els, 1);
    mxLogical* out_ptr = mxGetLogicals(out_exists_arr);
    
    for (uint32_t i = 0; i < n_els; i++)
    {
        out_ptr[i] = (mxLogical) c_locator.has_label(labs[i]);
    }
    
    plhs[0] = out_exists_arr;
}

void util::has_category(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    if (nlhs == 0)
    {
        return;
    }
    
    util::assert_nrhs(nrhs, 3, "locator:has_category");
    
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
    util::assert_nrhs(nrhs, 3, "locator:has_category");
    
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
    util::assert_nrhs(nrhs, 2, "locator:get_categories");
    
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
    util::assert_nrhs(nrhs, 2, "locator:get_labels");
    
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
    assert_nrhs(nrhs, 3, "locator:add_category");
    
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

void util::set_category_mult(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    using namespace util;
    
    assert_nrhs(nrhs, 5, "locator:set_category_mult");
    assert_nlhs(nlhs, 0, "locator:set_category_mult");
    
    assert_scalar(prhs[1], "locator:set_category_mult", "Id must be scalar.");
    assert_scalar(prhs[2], "locator:set_category_mult", "Category must be scalar.");
    
    const mxArray* in_id = prhs[1];
    const mxArray* in_cat = prhs[2];
    const mxArray* in_labels = prhs[3];
    const mxArray* in_indices = prhs[4];
    
    uint32_t category = mxGetScalar(in_cat);
    uint32_t* labels = (uint32_t*) mxGetData(in_labels);
    uint32_t* indices = (uint32_t*) mxGetData(in_indices);
    
    uint32_t n_labs = mxGetNumberOfElements(in_labels);
    uint32_t n_indices = mxGetNumberOfElements(in_indices);
    
    if (n_labs != n_indices)
    {
        mexErrMsgIdAndTxt("locator:set_category_mult", "Indices do not match number of labels.");
        return;
    }
    
    if (n_labs == 0)
    {
        return;
    }
    
    locator& c_locator = get_locator(mxGetScalar(in_id));
    
    bool loc_empty = c_locator.is_empty();
    
    if (!loc_empty && n_labs > c_locator.size())
    {
        mexErrMsgIdAndTxt("locator:set_category_mult", "Indices exceed locator dimensions.");
        return;
    }
    
    types::entries_t sorted_indices = copy_array_into_entries(in_indices, n_labs);
    
    sorted_indices.unchecked_sort(n_labs);
    
    uint32_t* indices_ptr = sorted_indices.unsafe_get_pointer();
    
    if (!loc_empty && (indices_ptr[n_labs-1] > c_locator.size() || indices_ptr[0] < 1))
    {
        mexErrMsgIdAndTxt("locator:set_category_mult", "Indices exceed locator dimensions.");
        return;
    }
    
    uint32_t index_sz;
    
    if (loc_empty)
    {
        index_sz = n_labs;
    }
    else
    {
        index_sz = c_locator.size();
    }
    
    bit_array logic_index(index_sz, false);
    
    bool assign_result = logic_index.assign_true(sorted_indices, -1);
    
    if (!assign_result)
    {
        mexErrMsgIdAndTxt("locator:set_category_mult", "Indices exceed locator dimensions.");
        return;
    }
    
    types::entries_t entries_labs = copy_array_into_entries(in_labels, n_labs);
    
    uint32_t result = c_locator.set_category(category, entries_labs, logic_index);
    
    if (result == locator_status::OK)
    {
        return;
    }
    
    if (result == locator_status::CATEGORY_DOES_NOT_EXIST)
    {
        mexErrMsgIdAndTxt("locator:set_category_mult", "Category does not exist.");
    }
    
    if (result == locator_status::LABEL_EXISTS_IN_OTHER_CATEGORY)
    {
        mexErrMsgIdAndTxt("locator:set_category_mult", "Label already exists in another category.");
        return;
    }
    
    if (result == locator_status::WRONG_NUMBER_OF_INDICES)
    {
        mexErrMsgIdAndTxt("locator:set_category_mult", "Indices do not match number of labels.");
        return;
    }
    
    if (result == locator_status::WRONG_INDEX_SIZE)
    {
        mexErrMsgIdAndTxt("locator:set_category_mult", "Index exceeds locator dimensions.");
        return;
    }
}

void util::set_category(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    assert_nrhs(nrhs, 6, "locator:set_category");
    
    util::assert_scalar(prhs[1], "locator:set_category", "Id must be scalar.");
    util::assert_scalar(prhs[2], "locator:set_category", "Category must be scalar.");
    util::assert_scalar(prhs[3], "locator:set_category", "Label must be scalar.");
    util::assert_scalar(prhs[5], "locator:set_category", "Rows must be scalar.");
    
    uint32_t loc_id = mxGetScalar(prhs[1]);
    uint32_t category = mxGetScalar(prhs[2]);
    uint32_t label = mxGetScalar(prhs[3]);
    uint32_t rows = mxGetScalar(prhs[5]);
    
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
    
    uint32_t id = util::globals::next_id;
    
    if (nrhs == 1)
    {
        util::globals::locators[id] = std::move(util::locator());
    }
    else
    {
        uint32_t size_hint = (uint32_t) mxGetScalar(prhs[1]);
        util::globals::locators[id] = std::move(util::locator(size_hint));
    }
    
    util::globals::next_id++;
    
    plhs[0] = mxCreateUninitNumericMatrix(1, 1, mxUINT32_CLASS, mxREAL);
    uint32_t* out_ptr = (uint32_t*) mxGetData(plhs[0]);
    out_ptr[0] = id;
}

void util::destroy(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    if (nrhs == 1)
    {
        util::globals::locators.clear();
        util::globals::next_id = 0;
        return;
    }
    
    uint32_t id = (uint32_t) mxGetScalar(prhs[1]);
    
    size_t n_erased = util::globals::locators.erase(id);
    
    if (n_erased != 1)
    {
        mexErrMsgIdAndTxt("locator:destroy", "Unrecognized id.");
    }
}