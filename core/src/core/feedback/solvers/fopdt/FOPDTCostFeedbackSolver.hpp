//
// Created by adarw on 7/22/26.
//

#ifndef SYSID_FOPDTCOSTFEEDBACKSOLVER_HPP
#define SYSID_FOPDTCOSTFEEDBACKSOLVER_HPP
#include "../IFeedbackSolver.hpp"
#include "../../fopdt.hpp"
#include "../../pid.hpp"
#include "../../../optimization/Metrics.hpp"
#include <nlopt.hpp>
#include <optional>
#include "IPIDEstimatorFOPDT.hpp"

namespace sysid {

    struct FOPDTCostFeedbackTuneables {
        double wE{0.0}; // error weight
        double wC{0.0}; // input cost weight
        double wA{0.0}; // aggression weight
        std::optional<double> setpoint;
        double max_input{12.0};
    };

    class FOPDTCostFeedbackSolver : IFeedbackSolver<CostMetric, PIDGains, FOPDTCostFeedbackTuneables, FOPDTGains>{
    private:
        nlopt::opt problem;
        double simulationTime;
        double tolerance;
        const IPIDEstimatorFOPDT& estimator;
        void setupProblem();

    public:
        explicit FOPDTCostFeedbackSolver(const double simulationTime, const double tolerance, const IPIDEstimatorFOPDT& estimator) : problem(nlopt::algorithm::LN_BOBYQA, 3), simulationTime(simulationTime), tolerance(tolerance), estimator(estimator) {}
        double calculateCostFunction(const std::vector<double>& vGains, std::vector<double>& grad, void* _) const;
        [[nodiscard]] PIDGains getInitialGuess() const;
        OptimizationResult<CostMetric, PIDGains> solve() override;
    };
} // sysid

#endif //SYSID_FOPDTCOSTFEEDBACKSOLVER_HPP
