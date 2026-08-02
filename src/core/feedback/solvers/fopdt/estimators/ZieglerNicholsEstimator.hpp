//
// Created by adarw on 7/24/26.
//

#ifndef SYSID_ZIEGLERNICHOLSESTIMATOR_HPP
#define SYSID_ZIEGLERNICHOLSESTIMATOR_HPP
#include "../IPIDEstimatorFOPDT.hpp"

namespace sysid {
    class ZieglerNicholsEstimator : public IPIDEstimatorFOPDT {
    public:
        [[nodiscard]] PIDGains estimatePID(const FOPDTGains& gains) const override {
            return {
                1.2 * (gains.tau / (gains.K * gains.theta)),
                (0.6*gains.tau) / (gains.K * gains.theta * gains.theta),
                0.6 * (gains.tau / gains.K)
            };
        }
    };
}

#endif //SYSID_ZIEGLERNICHOLSESTIMATOR_HPP
