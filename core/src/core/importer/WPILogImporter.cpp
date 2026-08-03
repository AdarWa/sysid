//
// Created by adarw on 8/3/26.
//

#include "WPILogImporter.hpp"

#include <wpi/util/>

namespace sysid {
    SampleVectorPtr WPILogImporter::importFile(std::string filePath) {
#ifndef WITH_WPIUTIL
        throw std::runtime_error("SysId must be built with WPI support to parse WPILogs!");
#else
        throw nullptr;
#endif
    }
} // sysid