//
//  profile.hpp
//  locator
//
//  Created by Nick Fagan on 2/28/18.
//

#pragma once

#include <functional>
#include <string>
#include <chrono>

namespace util {
    namespace profile {
        typedef std::chrono::high_resolution_clock clock_t;
        typedef std::chrono::high_resolution_clock::time_point time_point_t;
        
        void simple(std::function<double(void)> func, std::string alias, uint32_t n_iters);
        
        double ellapsed_time_s(time_point_t t1, time_point_t t2);
    }
}
