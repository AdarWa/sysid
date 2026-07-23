//
// Created by adarw on 7/22/26.
//

#include "FOPDTCostFeedbackSolver.hpp"

namespace sysid {
    double FOPDTCostFeedbackSolver::calculateCostFunction(const std::vector<double>& vGains, std::vector<double>& grad, void* _) const {
        const PIDGains gains = PIDGains::fromVector(vGains);
        const size_t N = simulationTime / system.dt;

        double setpoint = 0.5 * inputs.K * tuneables.max_input;
        if (tuneables.setpoint) {
            setpoint = *tuneables.setpoint;
        }
        const FeedbackSimulationVector simulated = simulate_fopdt_pid_feedback(gains, inputs, system.dt, N, setpoint, tuneables.max_input);

        const FeedbackSimulationVector normalized = simulated.normalizeAround(setpoint, tuneables.max_input);

        const Eigen::VectorXd time_vector = Eigen::VectorXd::LinSpaced(N, 0, N-1) * system.dt;

        const double itae = (normalized.e.cwiseAbs() * time_vector).sum() * system.dt;

        const double inputPenalty = normalized.u.cwiseSquare().sum() * system.dt;

        const Eigen::VectorXd dudt = (normalized.u.tail(N-1) - normalized.u.head(N-1)) / system.dt;
        const double aggressionPenalty = dudt.cwiseSquare().sum() * system.dt;

        const double finalCost = tuneables.wE * itae + tuneables.wC * inputPenalty + tuneables.wA * aggressionPenalty;

        return finalCost;
    }

    static double calculateCostFunctionWrapper(const std::vector<double>& x, std::vector<double>& grad, void* data) {
        auto* solver = static_cast<const FOPDTCostFeedbackSolver*>(data);
        return solver->calculateCostFunction(x, grad, nullptr);
    }

    void FOPDTCostFeedbackSolver::setupProblem() {
        problem.set_lower_bounds({0.0,0.0,0.0});
        problem.set_upper_bounds({0.0,0.0,0.0});

        problem.set_min_objective(calculateCostFunctionWrapper, this);
        problem.set_xtol_rel(tolerance);
    }

    PIDGains FOPDTCostFeedbackSolver::getInitialGuess() {
        return {0,0,0};
    }

    OptimizationResult<OLSMetrics, PIDGains> FOPDTCostFeedbackSolver::solve() {
        setupProblem();
        std::vector<double> gains = getInitialGuess().toVector();
        double J = 0;
        if (const nlopt::result result = problem.optimize(gains, J); result != nlopt::result::SUCCESS) {
            throw std::runtime_error("FOPDTCostFeedbackSolver could not converge to a solution!");
        }
        return {
            .metrics = {

            },
            .gains = PIDGains::fromVector(gains)
        };
    }
} // sysid