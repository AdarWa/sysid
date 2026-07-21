//
// Created by adarw on 7/20/26.
//

#include "DynamicSolver.hpp"

#include "feedforward.hpp"

namespace sysid {
    DynamicSolver::DynamicSolver(const System& system, const FeedforwardGains& quasistaticGains) : system(system),
        quasistaticGains(quasistaticGains) {
        ka = problem.decision_variable();

        // TODO: Make an initial guess

        // constraints
        problem.subject_to(ka >= 0);
    }

    void DynamicSolver::putData(const std::shared_ptr<SampleVector>& log) {
        J = 0;
        for (size_t i = 0; i < log->N; i++) {
            const double velocity = log->getVelocity(system.systemType, i);
            const double acceleration = log->getAcceleration(system.systemType, i);

            double theta = 0;
            if (system.gravityType == GravityType::ARM && system.systemType == SystemType::POSITIONAL) {
                theta = log->getPosition(system.systemType, i);
            }

            auto u_pred = calculateFeedforward(quasistaticGains.ks, quasistaticGains.kv, ka, quasistaticGains.kg, velocity, acceleration, theta, system.gravityType);
            const auto& u = log->u(i);
            J += slp::pow(u_pred - u, 2);
        }

        problem.minimize(J);
    }

    OptimizationResult<OLSMetrics, FeedforwardGains> DynamicSolver::solve() {
        if (const slp::ExitStatus status = problem.solve(); status != slp::ExitStatus::SUCCESS) {
            throw std::runtime_error("Dynamic solver could not converge to a solution!");
        }
        return OptimizationResult<OLSMetrics, FeedforwardGains>{
            {
                0,
                J.value()
            }, {
              quasistaticGains.ks,
              quasistaticGains.kv,
              ka.value(),
              quasistaticGains.kg,
                system.gravityType,
            }
        };
    }
} // sysid