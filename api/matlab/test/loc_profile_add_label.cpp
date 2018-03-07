#include "mex.h"
#include "locator.hpp"
#include <iostream>
#include <cstdint>

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  
  using namespace util;
  
  uint32_t n_labs;
  uint32_t sz;
  
  if (nrhs != 2)
  {
    n_labs = 20;
    sz = 10e3;
  }
  else
  {
    n_labs = (uint32_t) mxGetScalar(prhs[0]);
    sz = (uint32_t) mxGetScalar(prhs[1]);
  }
  
  locator loc(n_labs);
  
  for (uint32_t i = 0; i < n_labs; i++)
  {
    loc.add_category(i);
    loc.set_category(i, i, std::move(bit_array(sz, true)));
  }
}