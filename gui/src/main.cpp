//
// Created by adarw on 8/2/26.
//
#include "imgui.h"
#include <GLFW/glfw3.h>
#include <iostream>

#include "windowing_utils.hpp"


int main() {
    GLFWwindow* window = init_window();
    while (!glfwWindowShouldClose(window)) {
        begin_frame();

        ImGui::Begin("Control Panel");
        ImGui::Text("Application status: Running");

        if (ImGui::Button("Increment Counter")) {
            std::cout << "Hello!" << std::endl;
        }
        ImGui::SameLine();
        ImGui::Text("Count = %d", 2);

        ImGui::End();

        set_bg_color(window, 20,20,20,10);
        end_frame(window);
    }

    terminate_window(window);
    return 0;
}
