//
// Created by adarw on 7/22/26.
//

#ifndef SYSID_PID_HPP
#define SYSID_PID_HPP

#include "fopdt.hpp"
#include "../optimization/OptimizationResult.hpp"
#include "../sample/logged_sample.hpp"

namespace sysid {
    struct PIDGains : Gains {
        double kp{0.0};
        double ki{0.0};
        double kd{0.0};

        PIDGains() = default;
        PIDGains(const double kp, const double ki, const double kd) : kp(kp), ki(ki), kd(kd) {}

        std::vector<double> toVector() const {
            return {kp, ki, kd};
        }

        static PIDGains fromVector(const std::vector<double>& vector) {
            return {vector[0], vector[1], vector[2]};
        }
    };

    struct FeedbackSimulationVector {
        Eigen::VectorXd y;
        Eigen::VectorXd u;
        Eigen::VectorXd e;

        FeedbackSimulationVector normalizeAround(const double setpoint, const double max_u) const {
            return  {
                y / setpoint,
                u / max_u,
                e / setpoint
            };
        }
    };

    FeedbackSimulationVector simulate_fopdt_pid_feedback(const PIDGains& gains, const FOPDTGains& fopdt_gains, const double dt, const size_t N,
        const double setpoint, const double input_max);
}

#endif //SYSID_PID_HPP