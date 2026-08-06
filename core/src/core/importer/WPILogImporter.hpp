//
// Created by adarw on 8/3/26.
//

#ifndef SYSID_WPILOGIMPORTER_HPP
#define SYSID_WPILOGIMPORTER_HPP
#include "IImporter.hpp"

namespace sysid {
    class WPILogImporter : public IImporter{
    public:
        LogFile importFile(const std::string& filePath, const bool verbose) override;
    };
} // sysid

#endif //SYSID_WPILOGIMPORTER_HPP