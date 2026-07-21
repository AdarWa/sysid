//
// Created by adarw on 7/20/26.
//

#include "QuasistaticSolver.hpp"

#include "feedforward.hpp"

namespace sysid {
    QuasistaticSolver::QuasistaticSolver(const System& system) : system(system) {
        ks = problem.decision_variable();
        kv = problem.decision_variable();
        kg = problem.decision_variable();

        // TODO: Make initial guesses

        // constraints
        problem.subject_to(ks >= 0);
        problem.subject_to(kv >= 0);

        if (system.gravityType == GravityType::NONE) {
            kg.set_value(0);
            problem.subject_to(kg == 0);
        }else {
            problem.subject_to(kg >= 0);
        }
    }

    void QuasistaticSolver::putData(const std::shared_ptr<SampleVector>& log) {
        J = 0;
        for (size_t i = 0; i < log->N; i++) {
            // The purpose of the quasistatic test is to find K_v, K_s and K_g , this can be easily done when the acceleration is zero.
            // The quasistatic part must have an acceleration be as close as it can to zero.
            const double velocity = log->getVelocity(system.systemType, i);

            double theta = 0;
            if (system.gravityType == GravityType::ARM && system.systemType == SystemType::POSITIONAL) {
                theta = log->getPosition(system.systemType, i);
            }

            auto u_pred = calculateFeedforward(ks, kv, 0, kg, velocity, 0, theta, system.gravityType);
            const auto& u = log->u(i);
            J += slp::pow(u_pred - u, 2);
        }

        problem.minimize(J);
    }

    OptimizationResult<OLSMetrics, FeedforwardGains> QuasistaticSolver::solve() {
        if (const slp::ExitStatus status = problem.solve(); status != slp::ExitStatus::SUCCESS) {
            throw std::runtime_error("Quasistatic solver could not converge to a solution!");
        }
        return OptimizationResult<OLSMetrics, FeedforwardGains>{
            {
                0,
                J.value()
            },{
            ks.value(),
            kv.value(),
            0,
            kg.value(),
                system.gravityType
            }
        };
    }
} // sysid