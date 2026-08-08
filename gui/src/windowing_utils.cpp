//
// Created by adarw on 8/3/26.
//

#include "windowing_utils.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <unordered_map>

namespace sysid::gui {
    static void glfw_error_callback(int error, const char* description) {
        std::cerr << "GLFW Error " << error << ": " << description << "\n";
    }

    GLFWwindow* init_window() {
        glfwSetErrorCallback(glfw_error_callback);
        if (!glfwInit())
            throw std::runtime_error("GLFW could not be initialized!");

        const auto glsl_version = "#version 130";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

        GLFWwindow* window = glfwCreateWindow(1280, 720, "System Identification Tool", nullptr, nullptr);
        if (window == nullptr)
            throw std::runtime_error("window is nullptr!");

        glfwMakeContextCurrent(window);
        glfwSwapInterval(1);

        IMGUI_CHECKVERSION();

        ImGui::CreateContext();
        const ImGuiIO& io = ImGui::GetIO(); (void)io;

        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init(glsl_version);
        return window;
    }

    void begin_frame() {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void end_frame(GLFWwindow* window) {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    void terminate_window(GLFWwindow* window) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(window);
        glfwTerminate();
    }

    /**
     *
     * @param window glfw window pointer
     * @param r red value from 0 to 1
     * @param g green value from 0 to 1
     * @param b blue value from 0 to 1
     * @param alpha alpha value from 0 to 1
     */
    void set_bg_color(GLFWwindow* window, const float r, const float g, const float b, const float alpha) {
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(r, g, b, alpha);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    template <typename T>
    void dragndrop_source(const std::string_view identifier, const T& payload) {
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
            ImGui::SetDragDropPayload(identifier.data(), &payload, sizeof(T));
            ImGui::EndDragDropSource();
        }
    }

    template <typename T>
    std::optional<T> dragndrop_target(const char* identifier) {
        if (ImGui::BeginDragDropTarget()) {
            const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(identifier);

            if (payload && payload->DataSize == sizeof(T)) {
                T result = *static_cast<const T*>(payload->Data);
                ImGui::EndDragDropTarget();
                return result;
            }

            ImGui::EndDragDropTarget();
        }
        return std::nullopt;
    }
}
