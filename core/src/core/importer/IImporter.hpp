//
// Created by adarw on 8/3/26.
//

#ifndef SYSID_IIMPORTER_HPP
#define SYSID_IIMPORTER_HPP
#include <memory>
#include <string>

#include "core/sample/logged_sample.hpp"

namespace sysid {
    class IImporter {
    public:
        virtual ~IImporter() = default;

        virtual SampleVectorPtr importFile(std::string filePath) = 0;
    };
} // sysid

#endif //SYSID_IIMPORTER_HPP