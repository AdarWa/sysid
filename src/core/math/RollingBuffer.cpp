//
// Created by adarw on 7/22/26.
//

#include "RollingBuffer.hpp"

namespace sysid {
    void RollingBuffer::shift() {
        for (size_t i = 0; i < N-1; i++) {
            buffer(i) = buffer(i+1);
        }
    }

    RollingBuffer::RollingBuffer(const size_t n): buffer(n), N(n) {
        if (n <= 0) {
            throw std::runtime_error("Rolling buffer cannot be initialized with size zero or one!");
        }
        this->reset();
    }

    void RollingBuffer::reset() {
        buffer.setZero();
    }

    void RollingBuffer::push(double value) {
        shift();
        buffer(N-1) = value;
    }

    double RollingBuffer::head() {
        return buffer(0);
    }
} // sysid