//
// Created by adarw on 7/21/26.
//

#ifndef SYSID_OPTIMIZATIONRESULT_HPP
#define SYSID_OPTIMIZATIONRESULT_HPP

#include <concepts>

namespace sysid {

    struct Metrics {
        virtual ~Metrics();
    };
    struct Gains {
        virtual ~Gains();
    };

    template <std::derived_from<Metrics> T, std::derived_from<Gains> S>
    struct OptimizationResult {
        T metrics;
        S gains;
    };

}

#endif //SYSID_OPTIMIZATIONRESULT_HPP