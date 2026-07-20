//
// Created by adarw on 7/20/26.

#ifndef SYSID_LOGGED_SAMPLE_HPP
#define SYSID_LOGGED_SAMPLE_HPP
#include <vector>
#include <ranges>
#include <Eigen/Eigen>

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

struct Log {
    virtual ~Log();
    SystemType systemType{SystemType::POSITIONAL};
    double dt{0.0};
};

struct SampleVectorLog : Log {
    SampleVector samples;

    explicit SampleVectorLog(const size_t N)
        : samples(N) {}
};

struct SampleLog : Log {
    std::vector<LoggedSample> samples;

    [[nodiscard]]
    SampleVectorLog vectorize() const {
        SampleVectorLog log(this->samples.size());
        log.dt = this->dt;
        log.systemType = this->systemType;
        for (auto [i, sample] : std::views::enumerate(samples)) {
            log.samples.u(i) = sample.u;
            log.samples.y_meas(i) = sample.y_meas;
            log.samples.dydt_meas(i) = sample.dydt_meas;
        }
        return log;
    }
};

#endif //SYSID_LOGGED_SAMPLE_HPP
