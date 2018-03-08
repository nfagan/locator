#include "mex.h"
#include "loc_opcodes.hpp"
#include <cstdlib>
#include <iostream>

const util::op_map_t op_map({
    {"create", util::ops::CREATE},
    {"destroy", util::ops::DESTROY},
    {"add_cat", util::ops::ADD_CATEGORY},
    {"set_cat", util::ops::SET_CATEGORY},
    {"get_cats", util::ops::GET_CATEGORIES},
    {"get_labs", util::ops::GET_LABELS},
    {"find", util::ops::FIND},
    {"has_cat", util::ops::HAS_CATEGORY},
    {"is_loc", util::ops::IS_LOCATOR},
    {"require_cat", util::ops::REQUIRE_CATEGORY},
    {"rm_cat", util::ops::RM_CATEGORY},
    {"size", util::ops::SIZE},
    {"keep", util::ops::KEEP},
    {"copy", util::ops::COPY},
    {"instances", util::ops::INSTANCES},
    {"which_cat", util::ops::WHICH_CATEGORY},
    {"has_lab", util::ops::HAS_LABEL},
    {"eq", util::ops::EQUALS},
    {"append", util::ops::APPEND}
});

void use_std_string(mxArray *plhs[], const mxArray *prhs[]);

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    if (nrhs == 0)
    {
        mexErrMsgIdAndTxt("locator_api_opcodes:main", "Not enough input arguments.");
        return;
    }
    
    if (nlhs == 0)
    {
        return;
    }
    
    use_std_string(plhs, prhs);
}

void use_std_string(mxArray *plhs[], const mxArray *prhs[])
{
    const mxArray* in_str = prhs[0];
    
    int sz = mxGetNumberOfElements(in_str);
    
    size_t strlen = (sz+1) * sizeof(char);
    
    char* str = (char*) std::malloc(strlen);
    
    if (str == nullptr)
    {
        mexErrMsgIdAndTxt("locator_api_opcodes:main", "String allocation failed.");
        return;
    }
    
    int result = mxGetString(in_str, str, strlen);
    
    if (result != 0)
    {
        std::free(str);
        
        mexErrMsgIdAndTxt("locator_api_opcodes:main", "String copy failed.");
        return;
    }
    
    auto res = op_map.find(std::string(str));
    
    if (res == op_map.end())
    {
        std::free(str);
        
        mexErrMsgIdAndTxt("locator_api_opcodes:main", "Unrecognized op code str.");
        return;
    }
    
    plhs[0] = mxCreateUninitNumericMatrix(1, 1, mxUINT32_CLASS, mxREAL);
    
    uint32_t* out_ptr = (uint32_t*) mxGetData(plhs[0]);
    
    out_ptr[0] = res->second;
    
    std::free(str);
}