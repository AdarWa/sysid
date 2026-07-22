//
// Created by adarw on 7/22/26.
//

#ifndef SYSID_ROLLINGBUFFER_HPP
#define SYSID_ROLLINGBUFFER_HPP
#include <Eigen/Eigen>
#include <stddef.h>

namespace sysid {

    class RollingBuffer {
    private:
        Eigen::VectorXd buffer;
        size_t N{0};

        void shift();

    public:
        explicit RollingBuffer(const size_t n);

        void reset();
        void push(double value);
        double head();
    };
} // sysid

#endif //SYSID_ROLLINGBUFFER_HPP