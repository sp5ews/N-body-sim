#include "sim_queue.h"

#include <string>
#include "../nlohmann/json.hpp"

void simQueue::generateSim(int steps)
{
    std::thread t([this, steps]()
                  {
                    int tms = timesteps + steps;
                    d.resize(3 * (points_c + 1) * tms);
                    for (int i = 0; i < steps; i++)
                    {
                        mtx.lock();
                        q.push_back(sim.points);
                        com.push_back(sim.centerOfMass());
                        sim.updatePoints(dt, G);
                        mtx.unlock();
                    }

                    for (int s = 0; s < tms; s++){
                        for (int n = 0; n < 3; n++){
                            for (int p = 0; p < points_c; p++)
                                d[n * (points_c+1) * tms + (p+1) * tms + s] = q[s][p].position[n];
                            d[n * (points_c+1) * tms + s] = com[s][n];
                    }}
                    mtx.lock();
                    disp.clear();
                    std::copy(d.begin(), d.end(), std::back_inserter(disp));
                    timesteps = tms;
                    mtx.unlock();
                });
    t.detach();
}

void simQueue::loadJson(std::ifstream *stream)
{
    nlohmann::json json = nlohmann::json::parse(*stream);
    if (json.contains("plot"))
    {
        if (json["plot"].contains("size"))
            plot_size = json["plot"]["size"].get<std::vector<float>>();
        if (json["plot"].contains("colormap"))
            sel_colormap = json["plot"]["colormap"].get<int>();
        if (json["plot"].contains("show_com"))
            show_com = json["plot"]["show_com"].get<bool>();
    }

    if (json.contains("simulation"))
    {
        if (json["simulation"].contains("G"))
            G = json["simulation"]["G"].get<double>();
        if (json["simulation"].contains("dt"))
            dt = json["simulation"]["dt"].get<double>();
    }

    for (nlohmann::json j : json["bodies"])
    {
        addPoint({j["mass"].get<double>(), j["position"].get<std::vector<double>>(), j["velocity"].get<std::vector<double>>()});
    }
}

void simQueue::drawUI(GLFWwindow *window)
{
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    ImGui::SetNextWindowPos({0, 0});
    ImGui::SetNextWindowSize({(float)width, (float)height});
    if (ImGui::Begin("main", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBringToFrontOnFocus))
    {
        if (ImPlot3D::BeginPlot("##main", ImVec2(-1, -1)))
        {
            ImPlot3DAxisFlags flags = 0;
            ImPlot3D::SetupAxes("X", "Y", "Z", flags, flags, flags);
            ImPlot3D::SetupAxesLimits(plot_size[0], plot_size[1], plot_size[2], plot_size[3], plot_size[4], plot_size[5]);
            ImPlot3D::SetupBoxScale(1.2, 1.2, 1.2);

            mtx.lock();
            int rcd = (int)round(curr_disp);
            float pos_p[3 * points_c];
        
            for (int i = 0; i < points_c; i++)
            {
                for (int n = 0; n < 3; n++)
                {
                    pos_p[n * points_c + i] = disp.data()[(i+1) * timesteps + n * disp.size() / 3 + rcd];
                }
            }
            ImPlot3D::PlotScatter("Bodies", &pos_p[0], &pos_p[points_c], &pos_p[2 * points_c], points_c);

            int col = 1;
            int offset = round(curr_disp);
            offset += (offset & 1);
            ImPlot3D::PushStyleVar(ImPlot3DStyleVar_LineWeight, lw);


            if (show_com)
            {
                ImVec4 color = {1, 1, 1, 1};
                ImPlot3D::PushStyleColor(ImPlot3DCol_Line, color);
                ImPlot3D::SetNextMarkerStyle(ImPlot3DMarker_Cross, -1, color, -1, color);
                ImPlot3D::PlotScatter("Center of Mass", &disp.data()[rcd], &disp.data()[disp.size() / 3 + rcd], &disp.data()[(2 * disp.size() / 3) + rcd], 1);
                ImPlot3D::PlotLine("##com_tr", &disp.data()[0], &disp.data()[disp.size() / 3], &disp.data()[2 * disp.size() / 3], offset);
                ImPlot3D::PopStyleColor();
                color.w = 0.5;
                ImPlot3D::PushStyleColor(ImPlot3DCol_Line, color);
                ImPlot3D::PlotLine("##pred_com", &disp.data()[offset], &disp.data()[disp.size() / 3 + offset], &disp.data()[2 * disp.size() / 3 + offset], timesteps - offset, ImPlot3DLineFlags_Segments | ImGuiColorEditFlags_AlphaPreviewHalf);
                ImPlot3D::PopStyleColor();
            }

            for (int i = 1; i < points_c + 1; i++)
            {
                // ImPlot3D::SetNextLineStyle(ImPlot3DMarker_Circle, 0.5);
                ImPlot3D::PushColormap(colormaps[sel_colormap]);
                ImVec4 color = ImPlot3D::GetColormapColor(2 * (col++));
                ImPlot3D::PushStyleColor(ImPlot3DCol_Line, color);
                ImPlot3D::PlotLine(("Trail of body " + std::to_string(i)).c_str(), &disp.data()[i * timesteps], &disp.data()[i * timesteps + disp.size() / 3], &disp.data()[i * timesteps + 2 * disp.size() / 3], offset);
                ImPlot3D::PopStyleColor();
                color.w = 0.5;
                ImPlot3D::PushStyleColor(ImPlot3DCol_Line, color);
                ImPlot3D::PlotLine(("##pred" + std::to_string(i)).c_str(), &disp.data()[i * timesteps + offset], &disp.data()[i * timesteps + disp.size() / 3 + offset], &disp.data()[i * timesteps + 2 * disp.size() / 3 + offset], timesteps - offset, ImPlot3DLineFlags_Segments | ImGuiColorEditFlags_AlphaPreviewHalf);
                ImPlot3D::PopStyleColor();
                ImPlot3D::PopColormap();
            }

            mtx.unlock();
            ImPlot3D::PopStyleVar();
            ImPlot3D::EndPlot();
        }
        ImGui::End();

        if (ImGui::Begin("Options", nullptr, ImGuiWindowFlags_NoCollapse))
        {
            ImGui::Checkbox("Show Center of Mass", &show_com);
            ImGui::Combo("##Colormap", &sel_colormap, colormaps, IM_ARRAYSIZE(colormaps));
            ImGui::SliderFloat("Line Weight", &lw, 1, 5, "%.1f");
            ImGui::SliderFloat("Frame", &curr_disp, 1, timesteps - 1);
            ImGui::SliderFloat("Speed", &play_speed, -5, 5, "x%.1f");

            ImGui::Separator();

            ImGui::Checkbox("Auto generate", &autogen);

            if (autogen)
                ImGui::BeginDisabled();
            if (ImGui::Button("Generate"))
            {
                generateSim(gen_num);
            }
            ImGui::SameLine();
            ImGui::InputInt("##nb", &gen_num, 100, 1000);
            if (autogen)
                ImGui::EndDisabled();

            if (ImGui::Button("Clear"))
            {
                q.resize(1);
                sim.points = t0;
                timesteps = 1;
            }

            if (autogen && timesteps == 1)
            {
                generateSim(100);
            }

            if (autogen && curr_disp > 0.75 * timesteps && play_speed != 0)
            {
                generateSim(timesteps * 1.5);
            }

            ImGui::End();
        }
    }
}