//
// Created by adarw on 8/3/26.
//

#ifndef SYSID_WINDOWING_UTILS_HPP
#define SYSID_WINDOWING_UTILS_HPP
#include <optional>
#include <string_view>
#include <GLFW/glfw3.h>

namespace sysid::gui {
    GLFWwindow* init_window();
    void begin_frame();
    void end_frame(GLFWwindow* window);
    void terminate_window(GLFWwindow* window);
    void set_bg_color(GLFWwindow* window, const float r, const float g, const float b, const float alpha);

    template <typename T>
    void dragndrop_source(const std::string_view identifier, const T& payload);

    template <typename T>
    std::optional<T> dragndrop_target(const char* identifier);
}

#endif //SYSID_WINDOWING_UTILS_HPP