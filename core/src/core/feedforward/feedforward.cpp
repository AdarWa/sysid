//
// Created by adarw on 7/20/26.
//

#include "feedforward.hpp"

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

    slp::Variable<double> calculateFeedforward(const slp::Variable<double>& ks, const slp::Variable<double>& kv, const slp::Variable<double>& ka, const slp::Variable<double>& kg, const slp::Variable<double>& v, const slp::Variable<double>& a, const slp::Variable<double>& theta, GravityType gravityType) {
        return ks * signum(v) + kv * v + ka * a + calculateGravity(kg, theta, gravityType);
    }
}
