//
// Created by adarw on 8/3/26.
//

#include "log_import_window.hpp"

#include <iostream>

#include "imgui.h"
#include <nfd.hpp>

#include "../windowing_utils.hpp"
#include "core/importer/ImportFactory.hpp"

namespace sysid::gui {

    static std::optional<std::string> show_file_dialog(const std::string& supportedFormats) {
        nfdchar_t* outPath = nullptr;
        const nfdfilteritem_t filterItem[1] = {
            {.name = "Supported Logs", .spec = supportedFormats.c_str()}
        };

        const nfdresult_t result = NFD_OpenDialog(&outPath, filterItem, 1, nullptr);
        if (result == NFD_OKAY) {
            const std::optional<std::string>& outStr = std::make_optional(std::string(outPath));
            NFD_FreePath(outPath);
            return outStr;
        }
        if (result == NFD_CANCEL){
            return std::nullopt;
        }
        throw std::runtime_error(std::format("Error while using dialog: {}", NFD_GetError()));
    }

    void generate_log_import_window() {
        static const std::vector<std::string>& formats = ImportFactory::getImportableFormats();
        static const auto joined_formats = formats
        | std::views::transform([](const auto& s) { return "." + s; })
        | std::views::join_with(std::string_view(", "))
        | std::ranges::to<std::string>();

        static std::optional<std::string> selected_log = std::nullopt;

        ImGui::Begin("Log Import");

        ImGui::Text("Supports: %s", joined_formats.c_str());

        if (ImGui::Button("Open Dialog")) {
            const std::string& comma_separated_formats = std::ranges::to<std::string>(std::views::join_with(formats, ","));
            selected_log = show_file_dialog(comma_separated_formats);
        }

        ImGui::Text("Selected Log: %s", [&]() {
           if (!selected_log)
               return "None";
            return selected_log->c_str();
        }());
        if (selected_log) {
            dragndrop_source<std::string>("SELECTED_LOG", *selected_log);
        }

        ImGui::End();
    }
}
