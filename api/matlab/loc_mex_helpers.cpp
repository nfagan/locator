#include "loc_mex_helpers.hpp"

void util::copy_entries_into_array(const util::types::entries_t& src, mxArray* dest, uint32_t n_copy)
{
    uint32_t* src_ptr = src.unsafe_get_pointer();
    uint32_t* dest_ptr = (uint32_t*) mxGetData(dest);
    
    std::memcpy(dest_ptr, src_ptr, n_copy * sizeof(uint32_t));
}

util::types::entries_t util::copy_array_into_entries(const mxArray* src, uint32_t n_copy)
{
    util::types::entries_t result(n_copy);
    
    uint32_t* dest_ptr = result.unsafe_get_pointer();
    uint32_t* src_ptr = (uint32_t*) mxGetData(src);
    
    std::memcpy(dest_ptr, src_ptr, n_copy * sizeof(uint32_t));
    
    return result;
}

mxArray* util::make_entries_into_array(const types::entries_t& src, uint32_t n_copy)
{
    mxArray* out = mxCreateUninitNumericMatrix(n_copy, 1, mxUINT32_CLASS, mxREAL);
    
    if (n_copy == 0)
    {
        return out;
    }
    
    util::copy_entries_into_array(src, out, n_copy);
    
    return out;
}

std::string util::get_string(const mxArray* in_str, bool* success)
{    
    int sz = mxGetNumberOfElements(in_str);
    
    size_t strlen = (sz+1) * sizeof(char);
    
    char* str = (char*) std::malloc(strlen);
    
    if (str == nullptr)
    {
        *success = false;
        return "";
    }
    
    int result = mxGetString(in_str, str, strlen);
    
    if (result != 0)
    {
        std::free(str);
        *success = false;
        return "";
    }
    
    *success = true;
    
    std::string res(str);
    std::free(str);
    
    return res;
}

void util::assert_scalar(const mxArray *arr, const char* id, const char* msg)
{
    if (!mxIsScalar(arr))
    {
        mexErrMsgIdAndTxt(id, msg);
    }
}

void util::assert_nrhs(int actual, int expected, const char* id)
{
    if (actual != expected)
    {
        mexErrMsgIdAndTxt(id, "Wrong number of inputs.");
    }
}

void util::assert_nrhs(int minimum, int maximum, int actual, const char* id)
{
    if (actual < minimum || actual > maximum)
    {
        mexErrMsgIdAndTxt(id, "Wrong number of inputs.");
    }
}

void util::assert_nlhs(int actual, int expected, const char* id)
{
    if (actual != expected)
    {
        mexErrMsgIdAndTxt(id, "Wrong number of outputs.");
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