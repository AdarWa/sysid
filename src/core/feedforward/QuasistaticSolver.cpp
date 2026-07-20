//
// Created by adarw on 7/20/26.
//

#include "QuasistaticSolver.hpp"

namespace sysid {
    QuasistaticSolver::QuasistaticSolver() {
        ks = problem.decision_variable();
        kv = problem.decision_variable();
        kg = problem.decision_variable();
    }

    void QuasistaticSolver::putData(std::shared_ptr<SampleVectorLog> log) {
    }

    std::optional<FeedforwardGains> QuasistaticSolver::solve() {
    }
} // sysid