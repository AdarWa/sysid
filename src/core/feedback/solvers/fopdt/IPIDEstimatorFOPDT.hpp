//
// Created by adarw on 7/24/26.
//

#ifndef SYSID_IPIDESTIMATORFOPDT_HPP
#define SYSID_IPIDESTIMATORFOPDT_HPP
#include "../../pid.hpp"

namespace sysid {
    class IPIDEstimatorFOPDT {
    public:
        virtual ~IPIDEstimatorFOPDT() = default;
        [[nodiscard]] virtual PIDGains estimatePID(const FOPDTGains& gains) const = 0;
    };
}

#endif //SYSID_IPIDESTIMATORFOPDT_HPP