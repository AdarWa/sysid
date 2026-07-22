//
// Created by adarw on 7/22/26.
//

#include "IFeedbackSolver.hpp"

namespace sysid {

    template <std::derived_from<Metrics> METRICS, std::derived_from<Gains> GAINS, typename TUNEABLES, typename INPUTS>
    void IFeedbackSolver<METRICS, GAINS, TUNEABLES, INPUTS>::setSystem(const System& system_) {
        system = system_;
    }

    template <std::derived_from<Metrics> METRICS, std::derived_from<Gains> GAINS, typename TUNEABLES, typename INPUTS>
    void IFeedbackSolver<METRICS, GAINS, TUNEABLES, INPUTS>::setTuneables(TUNEABLES tuneables_) {
        tuneables = tuneables_;
    }

    template <std::derived_from<Metrics> METRICS, std::derived_from<Gains> GAINS, typename TUNEABLES, typename INPUTS>
    void IFeedbackSolver<METRICS, GAINS, TUNEABLES, INPUTS>::setInputs(INPUTS inputs_) {
        inputs = inputs_;
    }

    template <std::derived_from<Metrics> METRICS, std::derived_from<Gains> GAINS, typename TUNEABLES, typename INPUTS>
    void IFeedbackSolver<METRICS, GAINS, TUNEABLES, INPUTS>::setData(SampleVectorPtr data_) {
        data = data_;
    }

    template <std::derived_from<Metrics> METRICS, std::derived_from<Gains> GAINS, typename TUNEABLES, typename INPUTS>
    System IFeedbackSolver<METRICS, GAINS, TUNEABLES, INPUTS>::getSystem() const {
        return system;
    }

    template <std::derived_from<Metrics> METRICS, std::derived_from<Gains> GAINS, typename TUNEABLES, typename INPUTS>
    TUNEABLES IFeedbackSolver<METRICS, GAINS, TUNEABLES, INPUTS>::getTuneables() const {
        return tuneables;
    }

    template <std::derived_from<Metrics> METRICS, std::derived_from<Gains> GAINS, typename TUNEABLES, typename INPUTS>
    INPUTS IFeedbackSolver<METRICS, GAINS, TUNEABLES, INPUTS>::getInputs() const {
        return inputs;
    }

    template <std::derived_from<Metrics> METRICS, std::derived_from<Gains> GAINS, typename TUNEABLES, typename INPUTS>
    SampleVectorPtr IFeedbackSolver<METRICS, GAINS, TUNEABLES, INPUTS>::getData() const {
        return data;
    }

}