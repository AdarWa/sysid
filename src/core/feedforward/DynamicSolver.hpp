//
// Created by adarw on 7/20/26.
//

#ifndef SYSID_DYNAMICSOLVER_HPP
#define SYSID_DYNAMICSOLVER_HPP
#include "gains.hpp"
#include "sleipnir/optimization/problem.hpp"
#include "../sample/logged_sample.hpp"

namespace sysid {
    class DynamicSolver {
    private:
        slp::Problem<double> problem;
        slp::Variable<double> ka;
        const System& system;
        const FeedforwardGains& quasistaticGains;
    public:
        explicit DynamicSolver(const System& system, const FeedforwardGains& quasistaticGains);
        void putData(const std::shared_ptr<SampleVector>& log);
        FeedforwardGains solve();
    };
} // sysid

#endif //SYSID_DYNAMICSOLVER_HPP