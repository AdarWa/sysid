//
// Created by adarw on 8/3/26.
//

#ifndef SYSID_WINDOWING_UTILS_HPP
#define SYSID_WINDOWING_UTILS_HPP
#include <GLFW/glfw3.h>

GLFWwindow* init_window();
void begin_frame();
void end_frame(GLFWwindow* window);
void terminate_window(GLFWwindow* window);
void set_bg_color(GLFWwindow* window, const float r, const float g, const float b, const float alpha);

#endif //SYSID_WINDOWING_UTILS_HPP