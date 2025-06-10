#pragma once

#include <vector>
#include "point_particle.h"
#include "../imgui/implot3d.h"

#include <GLFW/glfw3.h>

#define THRESHOLD 0.001

class trail
{
    int length;

public:
    std::vector<std::vector<double>> points = std::vector<std::vector<double>>(3);
    trail(int len = 1000)
    {
        length = len;
        points[0] = std::vector<double>(0);
        points[1] = std::vector<double>(0);
        points[2] = std::vector<double>(0);
    }

    void addPoint(std::vector<double> pos)
    {
        for (int i = 0; i < pos.size(); i++)
        {
            points[i].push_back(pos[i]);
        }
        for (int i = 0; i < 3; i++)
        {
            if (points[i].size() > length)
            {
                points[i].erase(points[i].begin());
            }
        }
    }
};

class NBodySimulator
{
public:
    std::vector<PP> points;
    //std::vector<trail> trails;
    //trail com_trail = trail(10000);

    NBodySimulator(){};

    NBodySimulator(std::vector<PP> *points){
        this->points = *points;
    };

    void updatePoints(double dt = 1, double G = 1);

    std::vector<std::vector<double>> calcForces(double G);

    std::vector<double> centerOfMass();
};
