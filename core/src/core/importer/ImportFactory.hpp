//
// Created by adarw on 8/3/26.
//

#ifndef SYSID_IMPORTFACTORY_HPP
#define SYSID_IMPORTFACTORY_HPP

#include <algorithm>
#include <format>
#include <memory>
#include <ranges>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "IImporter.hpp"
#include "WPILogImporter.hpp"

namespace sysid {
    class ImportFactory {
    private:
        inline static std::vector<std::string> formats;

        static void updateFormats() {
            formats = std::ranges::to<std::vector>(getImporters() | std::views::keys);
        }

    public:
        static std::unordered_map<std::string, std::unique_ptr<IImporter>>& getImporters() {
            static std::unordered_map<std::string, std::unique_ptr<IImporter>> importers = []() {
                std::unordered_map<std::string, std::unique_ptr<IImporter>> map;
                // !!! ADD ALL DEFAULT IMPORTERS HERE !!!
#ifdef WITH_WPIUTIL
                map["wpilog"] = std::make_unique<WPILogImporter>();
#endif
                // !!! END DEFAULT IMPORTERS !!!
                return map;
            }();
            return importers;
        }

        static void addImporter(const std::string& fileType, std::unique_ptr<IImporter> ptr) {
            if (!ptr) {
                throw std::invalid_argument("Importer must not be null!");
            }
            auto& map = getImporters();
            if (map.contains(fileType)) {
                throw std::runtime_error(std::format("Format '{}' already exists!", fileType));
            }
            map[fileType] = std::move(ptr);
            updateFormats();
        }

        static const std::vector<std::string>& getImportableFormats() {
            if (formats.empty() && !getImporters().empty()) {
                updateFormats();
            }
            return formats;
        }
    };
} // namespace sysid

#endif // SYSID_IMPORTFACTORY_HPP