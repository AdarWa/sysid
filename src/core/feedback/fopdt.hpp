//
// Created by adarw on 7/21/26.
//

#ifndef SYSID_FOPDT_HPP
#define SYSID_FOPDT_HPP

#include "sleipnir/autodiff/variable.hpp"
#include "sleipnir/optimization/problem.hpp"
#include "../sample/logged_sample.hpp"
#include <concepts>

#include "../optimization/OptimizationResult.hpp"
#include "../optimization/Metrics.hpp"

namespace sysid {

    using dvar = slp::Variable<double>;

    template <typename T>
    concept NumericCompatible = std::same_as<std::remove_cvref_t<T>, double> ||
                            std::same_as<std::remove_cvref_t<T>, dvar>;

    template <NumericCompatible T>
    struct FOPDTGains : Gains {
        T K{0.0};
        T tau{0.0};
        T theta{0.0};

        FOPDTGains() = default;
        FOPDTGains(const T K, const T tau, const T theta) : K(K), tau(tau), theta(theta) {}
    };

    template <NumericCompatible T>
    T fopdt_dynamics(T t, T K, T tau, T theta);

    /**
     * This solver takes a raw samples and fits a First Order Plus Dead Time model.
     * this model is good for velocity controlled systems.
     */
    class FOPDTSolver {
    private:
        slp::Problem<double> problem;
        dvar K, tau, theta;
        dvar J;
        const System& system;

        void makeCostFunction(SampleVectorPtr log);
    public:
        explicit FOPDTSolver(const System& system);
        void putData(SampleVectorPtr log);
        OptimizationResult<OLSMetrics, FOPDTGains<double>> solve();
    };
} // sysid

#endif //SYSID_FOPDT_HPP