//
// Created by adarw on 8/8/26.
//

#include "log_factory_window.hpp"

#include <filesystem>
#include <imgui.h>
#include <iostream>
#include <string>

#include "../LogStorage.hpp"
#include "../windowing_utils.hpp"
#include "core/importer/ImportFactory.hpp"

static std::string get_file_extension(const std::string& path_str) {
    const std::filesystem::path p(path_str);
    return p.extension().string();
}

void sysid::gui::generate_log_factory_window() {
    ImGui::Begin("Log Factory");

    static std::optional<std::string> file_path = std::nullopt;

    ImGui::BeginDisabled(!file_path.has_value() || LogStorage::hasLog());
    if (file_path)
        ImGui::Text("(%s)", file_path->c_str());
    else
        ImGui::Text("(Drag and Drop Log Here)");
    if (const auto log = dragndrop_target<std::string>("SELECTED_LOG")) {
        file_path = log;
    };

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    static bool verbose = false;
    ImGui::Checkbox("Verbose Import", &verbose);
    ImGui::Spacing();

    if (ImGui::Button("Begin Import")) {
        try {
            if (!file_path) throw std::runtime_error("Can't import with no file selected!");
            std::string ext = get_file_extension(*file_path);
            ext.erase(0,1); // remove dot
            if (!std::ranges::contains(ImportFactory::getImportableFormats(), ext)) throw std::runtime_error(std::format("Can't find importer for type {}", ext));

            const LogFile log = ImportFactory::getImporters()[ext]->importFile(*file_path, verbose);
            LogStorage::setLog(log);
        }catch (const std::exception& e) {
            ImGui::EndDisabled();
            ImGui::End();
            throw;
        }
    }

    ImGui::EndDisabled();

    if (LogStorage::hasLog()) {
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button("Clear Log")) {
            LogStorage::clearLog();
        }
    }

    ImGui::End();
}
