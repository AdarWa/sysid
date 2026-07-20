//
// Created by adarw on 7/20/26.
//

#ifndef SYSID_QUASISTATICSOLVER_HPP
#define SYSID_QUASISTATICSOLVER_HPP
#include "gains.hpp"
#include "../sample/logged_sample.hpp"
#include "sleipnir/optimization/problem.hpp"

namespace sysid {
    /**
    * Solver class for the quasistatic part of the motion.
    * Solves for K_s, K_v, and K_g. The dynamic solver can then
    * be used after in order to find K_a based on the found K_s, K_v, K_g.
     */
    class QuasistaticSolver {
    private:
        slp::Problem<double> problem;
        slp::Variable<double> ks, kv, kg;
        const System& system;

    public:
        explicit QuasistaticSolver(const System& system);
        void putData(const std::shared_ptr<SampleVector>& log);
        FeedforwardGains solve();
    };
} // sysid

#endif //SYSID_QUASISTATICSOLVER_HPP