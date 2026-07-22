//
// Created by adarw on 7/22/26.
//

#ifndef SYSID_FOPDTCOSTFEEDBACKSOLVER_HPP
#define SYSID_FOPDTCOSTFEEDBACKSOLVER_HPP
#include "../IFeedbackSolver.hpp"
#include "../../fopdt.hpp"
#include "../../pid.hpp"
#include "../../../optimization/Metrics.hpp"

namespace sysid {

    struct FOPDTCostFeedbackTuneables {
        double wE{0.0}; // error weight
        double wC{0.0}; // cost weight
        double wA{0.0}; // aggression weight
    };

    class FOPDTCostFeedbackSolver : IFeedbackSolver<OLSMetrics, PIDGains<double>, FOPDTCostFeedbackTuneables, FOPDTGains<double>>{
    private:
        slp::Problem<double> problem;
        slp::Variable<double> kp, ki, kd;
        slp::Variable<double> J;
        void calculateCostFunction();

    public:
        explicit FOPDTCostFeedbackSolver();
        OptimizationResult<OLSMetrics, PIDGains<double>> solve() override;
    };
} // sysid

#endif //SYSID_FOPDTCOSTFEEDBACKSOLVER_HPP