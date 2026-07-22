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
    protected:
        System system;
        TUNEABLES tuneables;
        INPUTS inputs;
        SampleVectorPtr data;

    public:
        virtual ~IFeedbackSolver() = default;

        // Setters
        void setSystem(const System& system_);
        void setTuneables(TUNEABLES tuneables_);
        void setInputs(INPUTS inputs_);
        void setData(SampleVectorPtr data_);

        // Getters
        System getSystem() const;
        TUNEABLES getTuneables() const;
        INPUTS getInputs() const;
        SampleVectorPtr getData() const;

        // Methods
        virtual OptimizationResult<METRICS, GAINS> solve() = 0;
    };
} // sysid

#endif //SYSID_IFEEDBACKSOLVER_HPP