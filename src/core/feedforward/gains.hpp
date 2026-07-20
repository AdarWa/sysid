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
        double ka;
        double kg;
        GravityType gravity;
    };
}

#endif //SYSID_GAINS_HPP