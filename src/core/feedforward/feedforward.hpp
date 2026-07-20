//
// Created by adarw on 7/20/26.
//

#ifndef SYSID_FEEDFORWARD_HPP
#define SYSID_FEEDFORWARD_HPP

#include <concepts>

#include "sleipnir/autodiff/variable.hpp"

namespace sysid {

    template <typename T>
    concept SleipnirCompatible = requires(const T& x) {
            { x } -> std::convertible_to<slp::Variable<double>>;
    };

    template <SleipnirCompatible T>
    T signum(const T& x);

    // template <SignumCompatible T>
    // concept FeedforwardCompatible = requires(const T& x) {
    //
    // };

    template <typename T>
    T calculateFeedforward(T ks, T kv, T ka, T kg, T v, T a, T theta);
}

#endif //SYSID_FEEDFORWARD_HPP