//
// Created by adarw on 7/20/26.
//

#ifndef SYSID_GAINS_HPP
#define SYSID_GAINS_HPP

#include "../sample/logged_sample.hpp"

namespace sysid {
    struct FeedforwardGains {
        double ks;
        double kv;
        double kg;
        GravityType gravity;

        FeedforwardGains(const double ks, const double kv, const double kg, const GravityType gravity)
            : ks(ks), kv(kv), kg(kg), gravity(gravity) {}
    };
}

#endif //SYSID_GAINS_HPP