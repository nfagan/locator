#include "mex.h"
#include "locator.hpp"
#include <iostream>
#include <cstdint>

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  
  using namespace util;
  
  uint32_t n_labs;
  uint32_t sz;
  uint32_t n_search;
  
  if (nrhs != 3)
  {
    n_labs = 20;
    sz = 10e3;
    n_search = n_labs;
  }
  else
  {
    n_labs = (uint32_t) mxGetScalar(prhs[0]);
    n_search = (uint32_t) mxGetScalar(prhs[1]);
    sz = (uint32_t) mxGetScalar(prhs[2]);
  }
  
  locator loc(n_labs);
  
  types::entries_t all_labels(n_labs);
  
  all_labels.seek_tail_to_start();
  
  for (uint32_t i = 0; i < n_labs; i++)
  {
    loc.add_category(i);
    loc.set_category(i, i, bit_array(sz, true));
    
    if (i < n_search)
    {
      all_labels.push(i);
    }
  }
  
  types::entries_t result = loc.find(all_labels, 1u);
  
  if (result.tail() != sz)
  {
    std::cout << "Failed: did not find all labels." << std::endl;
  }
  
  if (nlhs == 1)
  {
    uint32_t* result_ptr = result.unsafe_get_pointer();
    uint32_t n_elements = result.tail();
    
    plhs[0] = mxCreateUninitNumericMatrix(n_elements, 1, mxUINT32_CLASS, mxREAL);
    uint32_t* out_ptr = (uint32_t*) mxGetData(plhs[0]);
    
    std::memcpy(out_ptr, result_ptr, n_elements * sizeof(uint32_t));    
  }    
}