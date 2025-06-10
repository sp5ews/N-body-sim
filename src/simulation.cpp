#include "simulation.h"
#include "math.h"


std::vector<std::vector<double>> NBodySimulator::calcForces(double G)
{
    std::vector<std::vector<double>> forces;

    for (int i = 0; i < points.size(); i++)
        forces.push_back({0, 0, 0});

    for (int i = 0; i < points.size(); i++)
    {
        for (int j = i + 1; j < points.size(); j++)
        {
            double d = pow(pow(points[i].position[0] - points[j].position[0], 2) +
                               pow(points[i].position[1] - points[j].position[1], 2) +
                               pow(points[i].position[2] - points[j].position[2], 2),
                           1.5);

            for (int n = 0; n < 3; n++)
            {
                double F = 0;
                if (d >= THRESHOLD){
                    F = (points[i].position[n] - points[j].position[n]) * G * points[i].mass * points[j].mass / d;
                }
                forces[i][n] -= F;
                forces[j][n] += F;
            }
        }
    }
    return forces;
}

// Algorytm Verleta
void NBodySimulator::updatePoints(double dt, double G)
{
    std::vector<std::vector<double>> forces = calcForces(G);

    for (int i = 0; i < points.size(); i++)
    {
        PP *p = &points[i];
        for (int n = 0; n < p->dim; n++)
            p->position[n] += p->velocity[n] * dt + pow(dt, 2.0) / 2.0 * (forces[i][n] / p->mass);
        //trails[i].addPoint(p->position);
    }
    std::vector<std::vector<double>> new_forces = calcForces(G);
    for (int i = 0; i < points.size(); i++)
    {
        PP *p = &points[i];
        for (int n = 0; n < p->dim; n++)
            p->velocity[n] += (forces[i][n] + new_forces[i][n]) / p->mass * dt / 2;
    }
}

std::vector<double> NBodySimulator::centerOfMass()
{
    std::vector<double> com = std::vector<double>(3);
    double M = 0;
    for (int i = 0; i < points.size(); i++)
    {
        PP *p = &points[i];
        for (int n = 0; n < 3; n++)
        {
            com[n] += p->mass * p->position[n];
        }
        M += p->mass;
    }
    for (int n = 0; n < 3; n++)
        com[n] /= M;

    return com;
}