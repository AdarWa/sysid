//
// Created by adarw on 8/3/26.
//

#include "log_import_window.hpp"

#include <iostream>

#include "imgui.h"

namespace sysid::gui {
    void generate_log_import_window() {
        ImGui::Begin("Control Panel");
        ImGui::Text("Application status: Running");

        if (ImGui::Button("Increment Counter")) {
            std::cout << "Hello!" << std::endl;
        }
        ImGui::SameLine();
        ImGui::Text("Count = %d", 2);

        ImGui::End();
    }
}
