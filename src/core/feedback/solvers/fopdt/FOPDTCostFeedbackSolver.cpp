//
// Created by adarw on 7/22/26.
//

#include "FOPDTCostFeedbackSolver.hpp"

namespace sysid {
    void FOPDTCostFeedbackSolver::calculateCostFunction() {
        PIDGains<dvar> gains = {
            kp, ki, kd
        };
        FeedbackSimulationVector simulated = simulate_fopdt_pid_feedback<dvar, double>(gains, inputs, system.dt, data->N, 1.0, 12);
    }

    FOPDTCostFeedbackSolver::FOPDTCostFeedbackSolver() {
        kp = problem.decision_variable();
        ki = problem.decision_variable();
        kd = problem.decision_variable();
    }

    OptimizationResult<OLSMetrics, PIDGains<double>> FOPDTCostFeedbackSolver::solve() {
        throw nullptr;
    }
} // sysid