//
// Created by adarw on 7/20/26.
//

#ifndef SYSID_FEEDFORWARD_HPP
#define SYSID_FEEDFORWARD_HPP

#include <concepts>

#include "../sample/logged_sample.hpp"
#include "sleipnir/autodiff/variable.hpp"

namespace sysid {
    template <typename T>
    concept SleipnirCompatible = requires(const T& x) {
            { x } -> std::convertible_to<slp::Variable<double>>;
    };

    template <SleipnirCompatible T>
    T signum(const T& x);


    slp::Variable<double> calculateFeedforward(const slp::Variable<double>& ks, const slp::Variable<double>& kv, const slp::Variable<double>& ka, const slp::Variable<double>& kg, const slp::Variable<double>& v, const slp::Variable<double>& a, const slp::Variable<double>& theta, GravityType gravityType);
}

#endif //SYSID_FEEDFORWARD_HPP