#include "loc_multimap_api.hpp"
#include <array>
#include <iostream>
#include <vector>

namespace util {    
    namespace globals {
        util::storage_t multimaps;
        
        uint32_t next_id = 0;
        
        std::array<util::mex_func_t, util::ops::N_OPS> funcs;
        
        bool INITIALIZED = false;
    }
}

void util::init_multimap_functions()
{
    using namespace util;
    
    if (globals::INITIALIZED)
    {
        return;
    }
    
    globals::funcs[ops::CREATE] =                   &util::create;
    globals::funcs[ops::DESTROY] =                  &util::destroy;
    globals::funcs[ops::INSERT] =                   &util::insert;
    globals::funcs[ops::AT] =                       &util::at;
    globals::funcs[ops::INSTANCES] =                &util::instances;
    globals::funcs[ops::KEYS] =                     &util::keys;
    globals::funcs[ops::VALUES] =                   &util::values;
    globals::funcs[ops::CONTAINS] =                 &util::contains_kv;
    globals::funcs[ops::COPY] =                     &util::copy;
    globals::funcs[ops::SIZE] =                     &util::size;
    globals::funcs[ops::IS_MULTIMAP] =              &util::is_multimap;
    globals::funcs[ops::AT_OR_UNDEFINED] =          &util::at_or_undefined;
    globals::funcs[ops::ERASE] =                    &util::erase;
    
    globals::INITIALIZED = true;
    
    std::cout << std::endl << "Initialized multimap api." << std::endl << std::endl;
}

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    //  store function pointers
    util::init_multimap_functions();
    
    //  make sure at least one input (op code) is present
    util::assert_nrhs(1, 10, nrhs, "multimap:main");
    
    util::assert_scalar(prhs[0], "multimap:main", "Op-code must be scalar.");
    
    uint32_t op_code = mxGetScalar(prhs[0]);
    
    if (op_code >= util::ops::N_OPS)
    {
        mexErrMsgIdAndTxt("multimap:main", "Invalid op-code.");
        return;
    }
    
    //  call function associated with the op-code
    util::globals::funcs[op_code](nlhs, plhs, nrhs, prhs);    
}

void util::assert_char_or_uint32_t(const mxArray* in_arr)
{
    util::assert_char_or_uint32_t_class(mxGetClassID(in_arr));
}

void util::assert_char_or_uint32_t_class(mxClassID id)
{
    if (id != mxUINT32_CLASS && id != mxCHAR_CLASS)
    {
        mexErrMsgIdAndTxt("multimap:assert_char_or_uint32_t", "Input must be char or uint32.");
        return;
    }
}

util::loc_multimap_t& util::get_multimap(uint32_t id)
{
    auto it = util::globals::multimaps.find(id);
    
    if (it == util::globals::multimaps.end())
    {
        mexErrMsgIdAndTxt("multimap:get_multimap", "Unrecognized id.");
    }
    
    return it->second;
}

void util::is_multimap(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    using namespace util;
    
    assert_nrhs(nrhs, 2, "multimap:is_multimap");
    assert_nlhs(nlhs, 1, "multimap:is_multimap");
    
    assert_scalar(prhs[1], "multimap:is_multimap", "Id must be scalar.");
    
    uint32_t id = mxGetScalar(prhs[1]);
    bool exists = globals::multimaps.find(id) != globals::multimaps.end();
    
    plhs[0] = mxCreateLogicalScalar(exists);
}

void util::size(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    using namespace util;
    
    assert_nrhs(nrhs, 2, "multimap:size");
    assert_nlhs(nlhs, 1, "multimap:size");
    
    assert_scalar(prhs[1], "multimap:size", "Id must be scalar.");
    
    const loc_multimap_t& c_multimap = get_multimap(mxGetScalar(prhs[1]));
    
    plhs[0] = mxCreateUninitNumericMatrix(1, 1, mxUINT32_CLASS, mxREAL);
    
    uint32_t* data = (uint32_t*) mxGetData(plhs[0]);
    data[0] = c_multimap.size();
    
}

void util::contains_kv(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    using namespace util;
    
    assert_nrhs(nrhs, 3, "multimap:contains");
    assert_nlhs(nlhs, 1, "multimap:contains");
    
    assert_scalar(prhs[1], "multimap:contains", "Id must be scalar.");
    
    const mxArray* key_or_value_arr = prhs[2];
    
    mxClassID id_key_value = mxGetClassID(key_or_value_arr);
    
    assert_char_or_uint32_t_class(id_key_value);
    
    const loc_multimap_t& c_multimap = get_multimap(mxGetScalar(prhs[1]));
    
    //  key is uint32_t
    if (id_key_value == mxUINT32_CLASS)
    {
        assert_scalar(key_or_value_arr, "multimap:contains", "Key must be scalar.");
        
        uint32_t key = mxGetScalar(key_or_value_arr);
        
        plhs[0] = mxCreateLogicalScalar(c_multimap.contains(key));
        
        return;
    }
    //  key is string
    
    bool str_result;
    
    std::string key = get_string(key_or_value_arr, &str_result);
    
    if (!str_result)
    {
        mexErrMsgIdAndTxt("multimap:insert", "String copy failed.");
        return;
    }
    
    plhs[0] = mxCreateLogicalScalar(c_multimap.contains(key));
}

void util::erase(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    using namespace util;
    
    assert_nrhs(nrhs, 3, "multimap:erase");
    assert_nlhs(nlhs, 0, "multimap:erase");
    
    const mxArray* in_id = prhs[1];
    const mxArray* in_keys = prhs[2];
    
    assert_scalar(in_id, "multimap:erase", "Id must be scalar.");
    
    loc_multimap_t& c_multimap = get_multimap(mxGetScalar(in_id));
    
    mxClassID id_key = mxGetClassID(in_keys);
    
    if (id_key == mxUINT32_CLASS)
    {
        uint32_t* keys = (uint32_t*) mxGetData(in_keys);
        size_t n_els = mxGetNumberOfElements(in_keys);
        
        for (size_t i = 0; i < n_els; i++)
        {
            c_multimap.erase(keys[i]);
        }
        
        return;
    }
    
    if (id_key == mxCHAR_CLASS)
    {
        bool str_result;

        std::string key = get_string(in_keys, &str_result);

        if (!str_result)
        {
            mexErrMsgIdAndTxt("multimap:erase", "String copy failed.");
        }
        
        c_multimap.erase(key);
        
        return;
    }
    
    const char* type_err_msg = "Input must be char, uint32_t, or cellstr.";
    
    //  key is cell array of string
    if (id_key != mxCELL_CLASS)
    {
        mexErrMsgIdAndTxt("multimap:erase", type_err_msg);
    }
    
    size_t n_els = mxGetNumberOfElements(in_keys);
    
    for (size_t i = 0; i < n_els; i++)
    {
        const mxArray* str_array = mxGetCell(in_keys, i);
        
        if (mxGetClassID(str_array) != mxCHAR_CLASS)
        {
            mexErrMsgIdAndTxt("multimap:erase", type_err_msg);
        }
        
        bool str_result;

        std::string key = get_string(str_array, &str_result);

        if (!str_result)
        {
            mexErrMsgIdAndTxt("multimap:erase", "String copy failed.");
        }
        
        c_multimap.erase(key);
    }
}

void util::keys(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    using namespace util;
    
    assert_nrhs(nrhs, 2, "multimap:keys");
    assert_nlhs(nlhs, 1, "multimap:keys");
    
    assert_scalar(prhs[1], "multimap:keys", "Id must be scalar.");
    
    const loc_multimap_t& c_multimap = get_multimap(mxGetScalar(prhs[1]));
    
    std::vector<std::string> c_keys = c_multimap.keys();
    
    mxArray* out_arr = mxCreateCellMatrix(c_keys.size(), 1);
    
    size_t i = 0;
    size_t sz = c_keys.size();
    
    for (size_t i = 0; i < sz; i++)
    {
        mxArray* str = mxCreateString(c_keys[i].c_str());
        mxSetCell(out_arr, i, str);
    }
    
    plhs[0] = out_arr;
}

void util::values(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    using namespace util;
    
    assert_nrhs(nrhs, 2, "multimap:values");
    assert_nlhs(nlhs, 1, "multimap:values");
    
    assert_scalar(prhs[1], "multimap:values", "Id must be scalar.");
    
    const loc_multimap_t& c_multimap = get_multimap(mxGetScalar(prhs[1]));
    
    std::vector<uint32_t> c_values = c_multimap.values();
    
    size_t sz = c_values.size();
    
    plhs[0] = mxCreateUninitNumericMatrix(sz, 1, mxUINT32_CLASS, mxREAL);
    
    uint32_t* data = (uint32_t*) mxGetData(plhs[0]);
    
    memcpy(data, &c_values[0], sz * sizeof(uint32_t));
}

std::string util::get_string_or_err(const loc_multimap_t& map, uint32_t key)
{
    if (!map.contains(key))
    {
        mexErrMsgIdAndTxt("multimap:get_string_or_err", "Key does not exist.");
    }

    return map.at(key);
}

uint32_t util::get_uint32_t_or_err(const loc_multimap_t& map, mxArray* key_arr)
{
    bool str_result;
    
    std::string key = util::get_string(key_arr, &str_result);
    
    if (!str_result)
    {
        mexErrMsgIdAndTxt("multimap:get_uint32_t_or_err", "String copy failed.");
    }
    
    if (!map.contains(key))
    {
        mexErrMsgIdAndTxt("multimap:get_uint32_t_or_err", "Key does not exist.");
    }
    
    return map.at(key);
}

void util::at_helper(mxArray* plhs[], const util::loc_multimap_t& c_multimap,
        const mxArray* key_arr, bool allow_undefined)
{
    mxClassID id_key = mxGetClassID(key_arr);
    
    //  if key is uint32
    if (id_key == mxUINT32_CLASS)
    {
        size_t n_els = mxGetNumberOfElements(key_arr);
        uint32_t* keys = (uint32_t*) mxGetData(key_arr);
        
        //  if key is scalar, return char
        if (n_els == 1)
        {
            uint32_t key = keys[0];
            
            if (!allow_undefined)
            {
                std::string res = get_string_or_err(c_multimap, key);
                plhs[0] = mxCreateString(res.c_str());
            }
            else if (c_multimap.contains(key))
            {
                std::string res = c_multimap.at(key);
                plhs[0] = mxCreateString(res.c_str());
            }
            else
            {
                plhs[0] = mxCreateString("");
            }
            
            return;
        }
        
        //  otherwise, return cell array of char
        mxArray* strs = mxCreateCellMatrix(1, n_els);
        
        for (size_t i = 0; i < n_els; i++)
        {
            uint32_t key = keys[i];
            
            if (!c_multimap.contains(key))
            {
                if (!allow_undefined)
                {
                    mxDestroyArray(strs);
                    mexErrMsgIdAndTxt("multimap:get_string_or_err", "Key does not exist.");
                    return;
                }
                else
                {
                    mxSetCell(strs, i, mxCreateString(""));
                    continue;
                }
            }
            
            mxSetCell(strs, i, mxCreateString(c_multimap.at(key).c_str()));
        }
        
        plhs[0] = strs;
        
        return;
    }
    
    //  key is char (scalar)
    if (id_key == mxCHAR_CLASS)
    {
        bool str_result;

        std::string key = get_string(key_arr, &str_result);

        if (!str_result)
        {
            mexErrMsgIdAndTxt("multimap:at", "String copy failed.");
        }
        
        uint32_t val;

        if (!c_multimap.contains(key))
        {
            if (allow_undefined)
            {
                val = util::locator::UNDEFINED_LABEL;
            }
            else
            {
                mexErrMsgIdAndTxt("multimap:at", "Key does not exist.");
            }
        }
        else
        {
            val = c_multimap.at(key);
        }

        plhs[0] = mxCreateUninitNumericMatrix(1, 1, mxUINT32_CLASS, mxREAL);

        uint32_t* data = (uint32_t*) mxGetData(plhs[0]);

        data[0] = val;
        
        return;
    }
    
    const char* type_err_msg = "Input must be char, uint32_t, or cellstr.";
    
    //  key is cell array of string
    if (id_key != mxCELL_CLASS)
    {
        mexErrMsgIdAndTxt("multimap:at", type_err_msg);
    }
    
    size_t n_els = mxGetNumberOfElements(key_arr);
    
    mxArray* nums = mxCreateUninitNumericMatrix(1, n_els, mxUINT32_CLASS, mxREAL);
    uint32_t* nums_data = (uint32_t*) mxGetData(nums);
    
    for (size_t i = 0; i < n_els; i++)
    {
        const mxArray* str_array = mxGetCell(key_arr, i);
        
        if (mxGetClassID(str_array) != mxCHAR_CLASS)
        {
            mxDestroyArray(nums);
            mexErrMsgIdAndTxt("multimap:at", type_err_msg);
        }
        
        bool str_result;
        
        std::string key = get_string(str_array, &str_result);

        if (!str_result)
        {
            mxDestroyArray(nums);
            mexErrMsgIdAndTxt("multimap:at", "String copy failed.");
        }
        
        uint32_t val;

        if (!c_multimap.contains(key))
        {
            if (allow_undefined)
            {
                val = util::locator::UNDEFINED_LABEL;
            }
            else
            {   
                mxDestroyArray(nums);
                mexErrMsgIdAndTxt("multimap:at", "Key does not exist.");
            }
        }
        else
        {
            val = c_multimap.at(key);
        }
        
        nums_data[i] = val;
    }
    
    plhs[0] = nums;
}

void util::at_or_undefined(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    using namespace util;
    
    assert_nrhs(nrhs, 3, "multimap:at");
    assert_nlhs(nlhs, 1, "multimap:at");
    
    const mxArray* map_id = prhs[1];
    const mxArray* key_arr = prhs[2];
    
    assert_scalar(map_id, "multimap:at", "Id must be scalar.");
    
    const loc_multimap_t& c_multimap = get_multimap(mxGetScalar(map_id));
    
    bool allow_undefined = true;
    
    util::at_helper(plhs, c_multimap, key_arr, allow_undefined);
}

void util::at(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    using namespace util;
    
    assert_nrhs(nrhs, 3, "multimap:at");
    assert_nlhs(nlhs, 1, "multimap:at");
    
    const mxArray* map_id = prhs[1];
    const mxArray* key_arr = prhs[2];
    
    assert_scalar(map_id, "multimap:at", "Id must be scalar.");
    
    const loc_multimap_t& c_multimap = get_multimap(mxGetScalar(map_id));
    
    bool allow_undefined = false;
    
    util::at_helper(plhs, c_multimap, key_arr, allow_undefined);
}

void util::insert(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    using namespace util;
    
    assert_nrhs(nrhs, 4, "multimap:insert");
    assert_nlhs(nlhs, 0, "multimap:insert");
    
    assert_scalar(prhs[1], "multimap:insert", "Id must be scalar.");
    
    const mxArray* key_arr = prhs[2];
    const mxArray* val_arr = prhs[3];
    
    mxClassID id_key = mxGetClassID(key_arr);
    mxClassID id_val = mxGetClassID(val_arr);
    
    if (id_key == id_val)
    {
        mexErrMsgIdAndTxt("multimap:insert", "Key type cannot match value type.");
        return;
    }    
    
    assert_char_or_uint32_t_class(id_key);
    assert_char_or_uint32_t_class(id_val);
    
    loc_multimap_t& c_multimap = get_multimap(mxGetScalar(prhs[1]));
    
    bool str_result;
    
    //  key is uint32_t, value is string
    
    if (id_key == mxUINT32_CLASS)
    {
        assert_scalar(key_arr, "multimap:insert", "uint32 key must be scalar.");
        
        uint32_t key = mxGetScalar(key_arr);
        
        std::string val = get_string(val_arr, &str_result);
        
        if (val.size() == 0)
        {
            mexErrMsgIdAndTxt("multimap:insert", "Cannot insert an empty string.");
            return;
        }
        
        if (!str_result)
        {
            mexErrMsgIdAndTxt("multimap:insert", "String copy failed.");
            return;
        }
        
        c_multimap.insert(val, key);
        
        return;
    }
    
    //  key is string, value is uint32_t
    
    assert_scalar(val_arr, "multimap:insert", "uint32 value must be scalar.");
    
    std::string key = get_string(key_arr, &str_result);
    
    if (key.size() == 0)
    {
        mexErrMsgIdAndTxt("multimap:insert", "Key cannot be empty.");
        return;
    }
    
    uint32_t val = mxGetScalar(val_arr);
    
    if (!str_result)
    {
        mexErrMsgIdAndTxt("multimap:insert", "String copy failed.");
        return;
    }
    
    c_multimap.insert(key, val);
}

void util::instances(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    if (nlhs == 0)
    {
        return;
    }
    
    util::assert_nrhs(nrhs, 1, "multimap:instances");
    
    uint32_t n_instances = (uint32_t) util::globals::multimaps.size();
    
    if (n_instances == 0)
    {
        plhs[0] = mxCreateUninitNumericMatrix(0, 1, mxUINT32_CLASS, mxREAL);
        return;
    }
    
    util::types::entries_t ids(n_instances);
    
    ids.seek_tail_to_start();
    
    for (const auto& it : util::globals::multimaps)
    {
        ids.push(it.first);
    }
    
    plhs[0] = make_entries_into_array(ids, n_instances);
}

void util::copy(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    util::assert_nrhs(nrhs, 2, "multimap:copy");
    
    util::assert_scalar(prhs[1], "multimap:copy", "Id must be scalar.");
    
    uint32_t in_id = mxGetScalar(prhs[1]);
    
    const util::loc_multimap_t& c_multimap = util::get_multimap(in_id);
    
    util::loc_multimap_t c_copy = c_multimap;
    
    uint32_t id = util::globals::next_id;

    util::globals::multimaps[id] = c_copy;
    
    util::globals::next_id++;
    
    plhs[0] = mxCreateUninitNumericMatrix(1, 1, mxUINT32_CLASS, mxREAL);
    uint32_t* out_ptr = (uint32_t*) mxGetData(plhs[0]);
    out_ptr[0] = id;
}

void util::create(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    if (nlhs == 0)
    {
        return;
    }
    
    util::assert_nrhs(nrhs, 1, "multimap:create");
    
    uint32_t id = util::globals::next_id;

    util::globals::multimaps[id] = std::move(util::loc_multimap_t());
    
    util::globals::next_id++;
    
    plhs[0] = mxCreateUninitNumericMatrix(1, 1, mxUINT32_CLASS, mxREAL);
    uint32_t* out_ptr = (uint32_t*) mxGetData(plhs[0]);
    out_ptr[0] = id;
}

void util::destroy(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    if (nrhs == 1)
    {
        util::globals::multimaps.clear();
        util::globals::next_id = 0;
        return;
    }
    
    uint32_t* ids = (uint32_t*) mxGetData(prhs[1]);
    uint32_t n_ids = mxGetNumberOfElements(prhs[1]);
    
    for (uint32_t i = 0; i < n_ids; i++)
    {
        util::globals::multimaps.erase(ids[i]);
    }
}