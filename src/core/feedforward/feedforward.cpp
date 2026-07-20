//
// Created by adarw on 7/20/26.
//

#include "feedforward.hpp"

#include "../sample/logged_sample.hpp"
#include "sleipnir/autodiff/variable.hpp"

namespace sysid {
    template <SleipnirCompatible T>
    T signum(const T& x)  {
        if (x > 0) {
            return 1;
        }
        if (x < 0) {
            return -1;
        }
        return 0;
    }

    template <SleipnirCompatible T>
    static T calculateGravity(const T& kg, const T& theta, const GravityType& gravityType) {
        if (gravityType == GravityType::NONE) {
            return 0;
        }
        if (gravityType == GravityType::ELEVATOR) {
            return kg;
        }
        if (gravityType == GravityType::ARM) {
            return slp::cos(theta) * kg;
        }
        return 0;
    }

    template <SleipnirCompatible T>
    T calculateFeedforward(T ks, T kv, T ka, T kg, T v, T a, T theta, GravityType gravityType) {
        return ks * signum(v) + kv * v + ka * a + calculateGravity(kg, theta, gravityType);
    }
}
