//
// Created by adarw on 8/8/26.
//

#include "log_view.hpp"

#include <cmath>
#include <imgui.h>
#include <implot.h>

void sysid::gui::generate_log_view() {
    static float xs[1001], ys1[1001], ys2[1001];
    static bool data_generated = false;

    if (!data_generated) {
        for (int i = 0; i < 1001; ++i) {
            xs[i] = i * 0.01f;
            ys1[i] = std::sin(xs[i] * 2.0f * 3.14159f);
            ys2[i] = std::cos(xs[i] * 2.0f * 3.14159f);
        }
        data_generated = true;
    }

    ImGui::Begin("My ImPlot Window");

    // BeginPlot creates the plotting region. Passing ImVec2(-1, 0) instructs the
    // plot to fill the available width of the window and calculate a default height.
    if (ImPlot::BeginPlot("Sine and Cosine Waves", ImVec2(-1, 0))) {

        // PlotLine processes the arrays and renders the line geometries.
        // It requires a label, a pointer to the X values, a pointer to the Y values,
        // and the total element count.
        ImPlot::PlotLine("Sine", xs, ys1, 1001);
        ImPlot::PlotLine("Cosine", xs, ys2, 1001);

        ImPlot::EndPlot();
    }

    ImGui::End();
}
