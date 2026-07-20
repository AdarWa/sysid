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


    slp::Variable<double> calculateFeedforward(slp::Variable<double> ks, slp::Variable<double> kv, slp::Variable<double> ka, slp::Variable<double> kg, slp::Variable<double> v, slp::Variable<double> a, slp::Variable<double> theta, GravityType gravityType);
}

#endif //SYSID_FEEDFORWARD_HPP