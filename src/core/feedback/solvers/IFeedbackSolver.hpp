//
// Created by adarw on 7/22/26.
//

#ifndef SYSID_IFEEDBACKSOLVER_HPP
#define SYSID_IFEEDBACKSOLVER_HPP

#include "../../optimization/OptimizationResult.hpp"
#include "../../sample/logged_sample.hpp"

namespace sysid {
    template <std::derived_from<Metrics> METRICS, std::derived_from<Gains> GAINS, typename TUNEABLES, typename INPUTS>
    class IFeedbackSolver {
        virtual ~IFeedbackSolver() = default;

        // Setters
        virtual void setSystem(const System& system_) = 0;
        virtual void setTuneables(TUNEABLES tuneables_) = 0;
        virtual void setInputs(INPUTS inputs_) = 0;
        virtual void setData(SampleVectorPtr data_) = 0;

        // Getters
        virtual System& getSystem() = 0;
        virtual TUNEABLES getTuneables() = 0;
        virtual INPUTS getInputs() = 0;
        virtual SampleVectorPtr getData() = 0;

        // Methods
        virtual OptimizationResult<METRICS, GAINS> solve() = 0;
    };
} // sysid

#endif //SYSID_IFEEDBACKSOLVER_HPP