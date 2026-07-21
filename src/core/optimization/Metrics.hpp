//
// Created by adarw on 7/21/26.
//

#ifndef SYSID_METRICS_HPP
#define SYSID_METRICS_HPP
#include "OptimizationResult.hpp"

namespace sysid {
    struct OLSMetrics : Metrics {
        double r2{0.0};
        double mse{0.0};

        OLSMetrics() = default;
        OLSMetrics(const double r2, const double mse) : r2(r2), mse(mse) {}
    };
}

#endif //SYSID_METRICS_HPP
