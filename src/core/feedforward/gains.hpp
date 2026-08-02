//
// Created by adarw on 7/20/26.
//

#ifndef SYSID_GAINS_HPP
#define SYSID_GAINS_HPP

#include "../optimization/OptimizationResult.hpp"
#include "../sample/logged_sample.hpp"

namespace sysid {
    struct FeedforwardGains : Gains{
        double ks{0.0};
        double kv{0.0};
        double ka{0.0};
        double kg{0.0};
        GravityType gravity{GravityType::NONE};

        FeedforwardGains() = default;
        FeedforwardGains(const double ks, const double kv, const double ka, const double kg, const GravityType gravity) : ks(ks), kv(kv), ka(ka), kg(kg), gravity(gravity) {}
    };
}

#endif //SYSID_GAINS_HPP