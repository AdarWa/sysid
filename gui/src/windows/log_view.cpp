//
// Created by adarw on 8/8/26.
//

#include "log_view.hpp"

#include <imgui.h>
#include <implot.h>

#include "core/sample/logged_sample.hpp"
#include "../LogStorage.hpp"

static constexpr double msToSeconds(const int64_t ms) {
    return static_cast<double>(ms) / 1000;
}

static void generate_plot(const sysid::LogFile& logFile) {
    if (ImPlot::BeginPlot("System Identification Data")) {
        try {
            ImPlot::SetupAxes("Timestamp", "Measurements");

            for (size_t i = 0; i < logFile.steps.size(); ++i) {
                const auto& [state, sampleLog] = logFile.steps[i];
                const size_t num_samples = sampleLog.samples.size();

                if (num_samples == 0) {
                    continue;
                }

                // Extract measurement arrays utilizing the existing vectorize method
                sysid::SampleVector vec = sampleLog.vectorize();

                std::vector<double> timestamps(num_samples);
                for (size_t j = 0; j < num_samples; ++j) {
                    timestamps[j] = static_cast<double>(msToSeconds(sampleLog.samples[j].timestamp));
                }

                std::string prefix = std::format("Step {} ", i);

                ImPlot::PlotLine((prefix + "u").c_str(), timestamps.data(), vec.u.data(), static_cast<int>(num_samples));
                ImPlot::PlotLine((prefix + "y_meas").c_str(), timestamps.data(), vec.y_meas.data(), static_cast<int>(num_samples));
                ImPlot::PlotLine((prefix + "dydt_meas").c_str(), timestamps.data(), vec.dydt_meas.data(), static_cast<int>(num_samples));
                ImPlot::PlotLine((prefix + "dydt2_meas").c_str(), timestamps.data(), vec.dydt2_meas.data(), static_cast<int>(num_samples));
            }
        }catch (const std::exception& e) {
            ImPlot::EndPlot();
            ImGui::End();
            throw;
        }
        ImPlot::EndPlot();
    }
}

void sysid::gui::generate_log_view() {
    ImGui::Begin("Log Viewer");

    if (LogStorage::hasLog()) {
        const LogFile& logFile = LogStorage::getLog();
        generate_plot(logFile);
    }else {
        ImGui::TextDisabled("No Log Currently Loaded!");
    }

    ImGui::End();
}
