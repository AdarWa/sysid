//
// Created by adarw on 7/22/26.
//

#ifndef SYSID_PID_HPP
#define SYSID_PID_HPP

#include "../optimization/OptimizationResult.hpp"

namespace sysid {
    struct PIDGains : Gains {
        double kp{0.0};
        double ki{0.0};
        double kd{0.0};

        PIDGains() = default;
        PIDGains(const double kp, const double ki, const double kd) : kp(kp), ki(ki), kd(kd) {}
    };
}

#endif //SYSID_PID_HPP