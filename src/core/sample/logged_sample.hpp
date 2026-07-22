//
// Created by adarw on 7/20/26.

#ifndef SYSID_LOGGED_SAMPLE_HPP
#define SYSID_LOGGED_SAMPLE_HPP
#include <vector>
#include <ranges>
#include <Eigen/Eigen>

namespace sysid {

    enum class SystemType {
        POSITIONAL,
        VELOCITY
    };

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
     *
     * dydt2_meas: The second derivative of the output. for a velocity system this will be the jerk.
     *                       for positional systems it will the acceleration.
     */
    struct LoggedSample {
        double u{0.0}; // e.g. voltage
        double y_meas{0.0}; // e.g. position
        double dydt_meas{0.0}; // e.g. velocity
        double dydt2_meas{0.0}; // e.g. acceleration

        double getPosition(const SystemType& systemType) const {
            if (systemType == SystemType::POSITIONAL) {
                return y_meas;
            }
            return 0; // on velocity system
        }

        double getVelocity(const SystemType& systemType) const {
            if (systemType == SystemType::POSITIONAL) {
                return dydt_meas;
            }
            return y_meas; // on velocity system
        }

        double getAcceleration(const SystemType& systemType) const {
            if (systemType == SystemType::POSITIONAL) {
                return dydt2_meas;
            }
            return dydt_meas; // on velocity system
        }

        double getJerk(const SystemType& systemType) const {
            if (systemType == SystemType::POSITIONAL) {
                return 0; // jerk is negligible on positional systems
            }
            return dydt2_meas; // on velocity system
        }
    };

    struct SampleVector {
        size_t N{0};
        Eigen::VectorXd u;
        Eigen::VectorXd y_meas;
        Eigen::VectorXd dydt_meas;
        Eigen::VectorXd dydt2_meas;

        explicit SampleVector(const size_t size) : N(size), u(size), y_meas(size), dydt_meas(size), dydt2_meas(size) {}

        double getPosition(const SystemType& systemType, const size_t index) const {
            if (systemType == SystemType::POSITIONAL) {
                return y_meas(index);
            }
            return 0; // on velocity system
        }

        double getVelocity(const SystemType& systemType, const size_t index) const {
            if (systemType == SystemType::POSITIONAL) {
                return dydt_meas(index);
            }
            return y_meas(index); // on velocity system
        }

        double getAcceleration(const SystemType& systemType, const size_t index) const {
            if (systemType == SystemType::POSITIONAL) {
                return dydt2_meas(index);
            }
            return dydt_meas(index); // on velocity system
        }

        double getJerk(const SystemType& systemType, const size_t index) const {
            if (systemType == SystemType::POSITIONAL) {
                return 0; // jerk is negligible on positional systems
            }
            return dydt2_meas(index); // on velocity system
        }
    };

    using SampleVectorPtr = std::shared_ptr<SampleVector>;

    enum class GravityType {
        ELEVATOR, // cos(0deg)
        ARM,      // cos(theta)
        NONE      // cos(90deg)
    };

    struct System {
        SystemType systemType{SystemType::VELOCITY};
        GravityType gravityType{GravityType::NONE};
        double dt{0.0};

        System() = default;
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
                log.dydt2_meas(i) = sample.dydt2_meas;
            }
            return log;
        }
    };
}

#endif //SYSID_LOGGED_SAMPLE_HPP
