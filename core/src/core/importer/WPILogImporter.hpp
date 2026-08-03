//
// Created by adarw on 8/3/26.
//

#ifndef SYSID_WPILOGIMPORTER_HPP
#define SYSID_WPILOGIMPORTER_HPP
#include "IImporter.hpp"

namespace sysid {
    class WPILogImporter : public IImporter{
    public:
        SampleVectorPtr importFile(std::string filePath) override;
    };
} // sysid

#endif //SYSID_WPILOGIMPORTER_HPP