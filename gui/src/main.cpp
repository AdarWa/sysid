//
// Created by adarw on 8/2/26.
//
#include <imgui.h>
#include <iostream>
#include <GLFW/glfw3.h>
#include <format>
#include <string>

#include "nfd.hpp"
#include "windowing_utils.hpp"
#include "windows/log_factory_window.hpp"
#include "windows/log_import_window.hpp"
#include "windows/log_view.hpp"

using namespace sysid::gui;

static bool g_open_error_popup = false;
static std::string g_error_msg;

static void trigger_error(const std::string& msg) {
    g_error_msg = msg;
    g_open_error_popup = true;
}

static void render_error_modal() {
    if (g_open_error_popup) {
        ImGui::OpenPopup("Error");
        g_open_error_popup = false;
    }

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Error", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::TextUnformatted(g_error_msg.c_str());
        ImGui::Separator();

        if (ImGui::Button("OK", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

int main() {
    GLFWwindow* window = nullptr;
    try {
        NFD_Init();
        window = init_window();

        while (!glfwWindowShouldClose(window)) {
            begin_frame();

            try {
                generate_log_import_window();
                generate_log_view();
                generate_log_factory_window();
            } catch (const std::exception& e) {
                trigger_error(std::format("Unhandled Error:\n{}", e.what()));
            }

            render_error_modal();

            set_bg_color(window, 0.5f, 0.5f, 0.5f, 0.7f);
            end_frame(window);
        }
    } catch (const std::exception& e) {
        std::cerr << "Fatal termination: " << e.what() << std::endl;
    }

    if (window)
        terminate_window(window);
    NFD_Quit();

    return 0;
}