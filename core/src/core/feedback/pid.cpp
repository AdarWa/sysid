//
// Created by adarw on 7/22/26.
//

#include "pid.hpp"

#include <algorithm>
#include <utility>
#include "../math/RollingBuffer.hpp"

namespace sysid {
    FeedbackSimulationVector simulate_fopdt_pid_feedback(const PIDGains& gains, const FOPDTGains& fopdt_gains, const double dt, const size_t N,
        const double setpoint, const double input_max) {

        const int delay_steps = std::max(1, static_cast<int>(fopdt_gains.theta / dt));

        Eigen::VectorXd y = Eigen::VectorXd::Zero(N);
        Eigen::VectorXd u = Eigen::VectorXd::Zero(N);
        Eigen::VectorXd e = Eigen::VectorXd::Zero(N);

        RollingBuffer u_buffer(delay_steps);

        double integral = 0.0;
        double prev_error = 0.0;

        for (int k = 1; k < N; k++) {
            // FOPDT differential form after discretization and some algebra, see derivation
            y(k) = y(k-1) + (dt / fopdt_gains.tau) * (-y(k-1) + fopdt_gains.K * u_buffer.head());

            e(k) = setpoint - y(k);

            integral += e(k) * dt;
            const double derivative = (e(k) - prev_error) / dt;

            const double curr_u = gains.kp * e(k) + gains.ki * integral + gains.kd * derivative;
            const double clamped_u = std::clamp(curr_u, 0.0, input_max);

            u(k) = clamped_u;
            prev_error = e(k);

            u_buffer.push(clamped_u);

        }

        return FeedbackSimulationVector{std::move(y), std::move(u), std::move(e)};
    }
}
