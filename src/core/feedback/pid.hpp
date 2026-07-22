//
// Created by adarw on 7/22/26.
//

#ifndef SYSID_PID_HPP
#define SYSID_PID_HPP

#include "fopdt.hpp"
#include "../optimization/OptimizationResult.hpp"

namespace sysid {
    template <NumericCompatible T>
    struct PIDGains : Gains {
        T kp{0.0};
        T ki{0.0};
        T kd{0.0};

        PIDGains() = default;
        PIDGains(const T kp, const T ki, const T kd) : kp(kp), ki(ki), kd(kd) {}
    };

    struct FeedbackSimulationVector {
        Eigen::VectorXd y;
        Eigen::VectorXd u;
        Eigen::VectorXd e;
    };

    template <NumericCompatible PID, NumericCompatible FOPDT>
    FeedbackSimulationVector simulate_fopdt_pid_feedback(const PIDGains<PID>& gains, const FOPDTGains<FOPDT>& fopdt_gains, const double dt, const size_t N,
        const double setpoint, const double input_max);
}

#endif //SYSID_PID_HPP