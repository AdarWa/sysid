//
// Created by adarw on 7/24/26.
//

#ifndef SYSID_COHENCOONESTIMATOR_HPP
#define SYSID_COHENCOONESTIMATOR_HPP

#include "../IPIDEstimatorFOPDT.hpp"

namespace sysid {
    class ZieglerNicholsEstimator : IPIDEstimatorFOPDT {
        PIDGains estimatePID(const FOPDTGains& gains) override {
            double kp = (gains.tau / (gains.K * gains.theta)) * (4.0/3.0 + gains.theta / (gains.tau * 4));
            return {
                kp,
                (kp/gains.theta) * ((13+8* (gains.theta / gains.tau)) / (32+ 6 * (gains.theta / gains.tau))),
                (kp * gains.theta) * (4 / (11 + 2 * (gains.theta / gains.tau)))
            };
        }
    };
}

#endif //SYSID_COHENCOONESTIMATOR_HPP