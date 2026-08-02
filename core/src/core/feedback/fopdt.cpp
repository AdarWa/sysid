//
// Created by adarw on 7/21/26.
//

#include "fopdt.hpp"

#include <iostream>

namespace sysid {

    template <NumericCompatible T>
    T fopdt_dynamics(T t, T K, T tau, T theta) {
        T t_diff = t - theta;
        T t_eff = slp::log(1.0 + slp::exp(20.0 * t_diff)) / 20.0;

        T smooth_step = 1.0 / (1.0 + slp::exp(-50.0 * t_diff));
        return K * (1.0 - slp::exp(-t_eff / tau)) * smooth_step;
    }


    FOPDTSolver::FOPDTSolver(const System& system) : system(system) {
        if (system.systemType == SystemType::POSITIONAL) {
            std::cerr << "Warning: A FOPDT model shouldn't be fitted to a positional system." << std::endl;
        }
        K = problem.decision_variable();
        tau = problem.decision_variable();
        theta = problem.decision_variable();
    }

    void FOPDTSolver::makeCostFunction(SampleVectorPtr log) {
        J = 0;
        for (auto [idx, y_meas] : std::views::enumerate(log->y_meas)) {
            const dvar t = idx * system.dt;
            dvar y_pred = fopdt_dynamics(t, K, tau, theta);
            J += slp::pow(y_meas - y_pred, 2);
        }

        problem.minimize(J);
    }

    void FOPDTSolver::putData(SampleVectorPtr log) {
        K.set_value(log->y_meas(log->N-1)); // Initial guess for K
        tau.set_value(system.dt * log->N / 2); // Initial guess for tau
        theta.set_value(5*system.dt); // Initial guess for theta

        double max_time = log->N * system.dt;
        problem.subject_to(K >= 0);
        problem.subject_to(K <= log->y_meas.maxCoeff());

        problem.subject_to(tau >= 1e-4);
        problem.subject_to(tau <= max_time);

        problem.subject_to(theta >= 0);
        problem.subject_to(theta <= max_time);

        makeCostFunction(log);
    }

    OptimizationResult<OLSMetrics, FOPDTGains> FOPDTSolver::solve() {
        if (const slp::ExitStatus status = problem.solve(); status != slp::ExitStatus::SUCCESS) {
            throw std::runtime_error("FOPDT solver could not converge to a solution!");
        }
        return OptimizationResult<OLSMetrics, FOPDTGains> {
            {
                0,
                J.value()
            },
            {
                K.value(),
                tau.value(),
                theta.value()
            }
        };
    }
} // sysid