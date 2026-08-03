//
// Created by adarw on 8/2/26.
//
#include "imgui.h"
#include <GLFW/glfw3.h>
#include <iostream>

#include "windowing_utils.hpp"
#include "windows/log_import_window.hpp"


using namespace sysid::gui;

int main() {
    GLFWwindow* window = init_window();
    while (!glfwWindowShouldClose(window)) {
        begin_frame();

        generate_log_import_window();

        set_bg_color(window, 0.5,0.5,0.5,0.7);
        end_frame(window);
    }

    terminate_window(window);
    return 0;
}
