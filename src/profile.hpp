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
        void simple(std::function<double(void)> func, std::string alias, uint32_t n_iters);
        
        double ellapsed_time_s(std::chrono::high_resolution_clock::time_point t1, std::chrono::high_resolution_clock::time_point t2);
    }
}
