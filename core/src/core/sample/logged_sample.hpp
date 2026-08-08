//
// Created by adarw on 7/20/26.

#ifndef SYSID_LOGGED_SAMPLE_HPP
#define SYSID_LOGGED_SAMPLE_HPP
#include <iostream>
#include <vector>
#include <ranges>
#include <memory>
#include <stdexcept>
#include <Eigen/Eigen>

namespace sysid {
    enum class SystemType {
        POSITIONAL,
        VELOCITY
    };

    enum class SystemState {
        DYNAMIC_FORWARD,
        DYNAMIC_BACKWARD,
        QUASISTATIC_FORWARD,
        QUASISTATIC_BACKWARD
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
        int64_t timestamp{0};
        SystemState systemState{SystemState::DYNAMIC_FORWARD};
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

        explicit SampleVector(const size_t size) : N(size), u(size), y_meas(size), dydt_meas(size), dydt2_meas(size) {
        }

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
        ARM, // cos(theta)
        NONE // cos(90deg)
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

    struct SampleLog {
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

        [[nodiscard]]
        SampleLog alignTimestamps(const int64_t dt, const bool fillHoles = true) const {
            if (samples.size() < 5) {
                throw std::runtime_error("SampleLog too small to align!");
            }
            const int64_t tStart = samples[0].timestamp;
            const int64_t tEnd = samples[samples.size() - 1].timestamp;
            const int64_t duration = tEnd - tStart;
            size_t vecSize = static_cast<size_t>(duration / dt) + 1;

            SampleLog newLog;
            newLog.samples.reserve(vecSize);
            for (size_t i = 0; i < vecSize; i++) {
                newLog.samples.push_back(LoggedSample(-1, SystemState::DYNAMIC_BACKWARD, 0, 0, 0, 0));
            }

            // Map original samples to the nearest dt grid point
            for (const auto& sample : samples) {
                const int64_t offset = sample.timestamp - tStart;
                if (offset < 0 || offset / dt >= static_cast<int64_t>(vecSize)) {
                    continue;
                }

                const auto newLogIndex = static_cast<size_t>(offset / dt);

                if (newLog.samples[newLogIndex].timestamp != -1) {
                    const int64_t cellTimestamp = static_cast<int64_t>(newLogIndex) * dt;
                    const int64_t oldDelta = std::abs(newLog.samples[newLogIndex].timestamp - tStart - cellTimestamp);
                    const int64_t newDelta = std::abs(sample.timestamp - tStart - cellTimestamp);

                    if (oldDelta < newDelta) {
                        continue;
                    }
                }
                newLog.samples[newLogIndex] = sample;
            }

            // Trim trailing unpopulated data points
            int64_t invalids = 0;
            for (int64_t i = static_cast<int64_t>(vecSize) - 1; i >= 0; i--) {
                if (newLog.samples[i].timestamp == -1) {
                    invalids++;
                } else {
                    break;
                }
            }

            if (invalids >= vecSize) {
                throw std::runtime_error("Got vector full of invalid data points!");
            }
            vecSize -= invalids;
            newLog.samples.resize(vecSize);

            // Validate hole sizes before processing them
            int64_t biggestHole = 0;
            int64_t consecutiveHoles = 0;
            for (const auto& [i, sample] : std::ranges::views::enumerate(newLog.samples)) {
                if (sample.timestamp == -1) {
                    consecutiveHoles++;
                    continue;
                }
                if (consecutiveHoles > biggestHole) {
                    biggestHole = consecutiveHoles;
                }
                consecutiveHoles = 0;
            }

            if (biggestHole * dt > 200) {
                throw std::runtime_error(std::format("Found a data hole sized {}. Aborting!", biggestHole * dt));
            }

            // Process remaining valid-sized holes
            if (fillHoles) {
                LoggedSample lastValid = newLog.samples[0];
                for (size_t i = 1; i < newLog.samples.size(); ++i) {
                    if (newLog.samples[i].timestamp == -1) {
                        newLog.samples[i] = lastValid;
                        newLog.samples[i].timestamp = tStart + static_cast<int64_t>(i) * dt;
                    } else {
                        lastValid = newLog.samples[i];
                    }
                }
            } else {
                std::erase_if(newLog.samples, [](const auto& sample) {
                    return sample.timestamp == -1;
                });
            }

            return newLog;
        }
    };

    struct LogFile {
        std::vector<std::pair<SystemState, SampleLog>> steps;
    };
}

#endif //SYSID_LOGGED_SAMPLE_HPP
