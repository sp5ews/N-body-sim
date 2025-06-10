#pragma once

#include <vector>
#include <math.h>

class PP
{
public:
    PP(double mass, int n = 3)
    {
        this->mass = mass;
        dim = n;
    }
    PP(double mass, std::vector<double> position, int n = 3)
    {
        this->mass = mass;
        this->position = position;
        this->dim = n;
    }
    PP(double mass, std::vector<double> position, std::vector<double> velocity, int n = 3)
    {
        this->mass = mass;
        this->position = position;
        this->velocity = velocity;
        this->dim = n;
    }

    std::vector<double> position = {0, 0, 0};
    std::vector<double> velocity = {0, 0, 0};
    double mass;
    int dim;
};

class display_PP : public PP
{
public:
    double color[3];
    double display_size;
};