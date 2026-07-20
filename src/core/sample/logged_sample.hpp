//
// Created by adarw on 7/20/26.

#ifndef SYSID_LOGGED_SAMPLE_HPP
#define SYSID_LOGGED_SAMPLE_HPP
#include <vector>
#include <ranges>
#include <Eigen/Eigen>

namespace sysid {
    /**
     * A struct representing a sample
     *
     * u: The input of the system, usually voltage
     *
     * y_meas: The direct output of the system. for a velocity controlled system this will be velocity.
     *         for positional systems it will be the position.
     *
     * dydt_meas: The derivative of the output. for a velocity system this will be the acceleration.
     *                       for positional systems it will the velocity.
     */
    struct LoggedSample {
        double u{0.0}; // e.g. voltage
        double y_meas{0.0}; // e.g. velocity
        double dydt_meas{0.0}; // e.g. acceleration
    };

    struct SampleVector {
        size_t N{0};
        Eigen::VectorXd u;
        Eigen::VectorXd y_meas;
        Eigen::VectorXd dydt_meas;

        explicit SampleVector(const size_t size) : N(size), u(size), y_meas(size), dydt_meas(size) {}
    };

    enum class SystemType {
        POSITIONAL,
        VELOCITY
    };

    enum class GravityType {
        ELEVATOR, // cos(0deg)
        ARM,      // cos(theta)
        NONE      // cos(90deg)
    };

    struct System {
        SystemType systemType{SystemType::VELOCITY};
        GravityType gravityType{GravityType::NONE};
        double dt{0.0};

        System(const SystemType system_type, const GravityType gravity_type, const double dt)
            : systemType(system_type),
              gravityType(gravity_type),
              dt(dt) {
            if (gravityType == GravityType::ARM && systemType != SystemType::POSITIONAL) {
                throw std::runtime_error("An ARM system must be controlled by a positional system!");
            }
        }
    };

    struct SampleLog{
        std::vector<LoggedSample> samples;

        [[nodiscard]]
        SampleVector vectorize() const {
            SampleVector log(this->samples.size());
            for (auto [i, sample] : std::views::enumerate(samples)) {
                log.u(i) = sample.u;
                log.y_meas(i) = sample.y_meas;
                log.dydt_meas(i) = sample.dydt_meas;
            }
            return log;
        }
    };
}

#endif //SYSID_LOGGED_SAMPLE_HPP
