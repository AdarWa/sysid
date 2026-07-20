//
// Created by adarw on 7/20/26.

#ifndef SYSID_LOGGED_SAMPLE_HPP
#define SYSID_LOGGED_SAMPLE_HPP
#include <vector>
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
    double u; // e.g. voltage
    double y_meas; // e.g. velocity
    double dydt_meas; // e.g. acceleration
};

struct SampleVector {
    double N;
    Eigen::VectorXd u;
    Eigen::VectorXd y_meas;
    Eigen::VectorXd dydt_meas;
};

enum class SystemType {
    POSITIONAL,
    VELOCITY
};

struct Log {
    virtual ~Log();
    SystemType systemType;
    double dt;
};

struct SampleLog : Log {
    std::vector<LoggedSample> samples;
};

struct SampleVectorLog : Log {
    SampleVector samples;
};

#endif //SYSID_LOGGED_SAMPLE_HPP
