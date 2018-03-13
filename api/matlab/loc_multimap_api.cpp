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

void util::at(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    using namespace util;
    
    assert_nrhs(nrhs, 3, "multimap:at");
    assert_nlhs(nlhs, 1, "multimap:at");
    
    assert_scalar(prhs[1], "multimap:at", "Id must be scalar.");
    
    const mxArray* key_arr = prhs[2];
    
    mxClassID id_key = mxGetClassID(key_arr);
    assert_char_or_uint32_t_class(id_key);
    
    loc_multimap_t& c_multimap = get_multimap(mxGetScalar(prhs[1]));
    
    //  key is uint32_t
    if (id_key == mxUINT32_CLASS)
    {
        assert_scalar(key_arr, "multimap:at", "Key must be scalar.");
        
        uint32_t key = mxGetScalar(key_arr);
        
        if (!c_multimap.contains(key))
        {
            mexErrMsgIdAndTxt("multimap:at", "Key does not exist.");
        }
        
        std::string res = c_multimap.at(key);
        plhs[0] = mxCreateString(res.c_str());
        return;
    }
    //  key is string
    
    bool str_result;
    
    std::string key = get_string(key_arr, &str_result);
    
    if (!str_result)
    {
        mexErrMsgIdAndTxt("multimap:insert", "String copy failed.");
        return;
    }
    
    if (!c_multimap.contains(key))
    {
        mexErrMsgIdAndTxt("multimap:at", "Key does not exist.");
    }
    
    plhs[0] = mxCreateUninitNumericMatrix(1, 1, mxUINT32_CLASS, mxREAL);
    
    uint32_t* data = (uint32_t*) mxGetData(plhs[0]);
    
    data[0] = c_multimap.at(key);
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
    
    uint32_t id = (uint32_t) mxGetScalar(prhs[1]);
    
    size_t n_erased = util::globals::multimaps.erase(id);
    
    if (n_erased != 1)
    {
        mexErrMsgIdAndTxt("multimap:destroy", "Unrecognized id.");
    }
}