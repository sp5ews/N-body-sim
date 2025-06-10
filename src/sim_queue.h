#include "simulation.h"

#include <thread>
#include <mutex>
#include <fstream>

class simQueue
{
    std::vector<std::vector<PP>> q;
    std::vector<std::vector<double>> com;
    NBodySimulator sim;

    // settings
    int sel_colormap = 10; // Spectral by default

    // consts
    const char *colormaps[12] = {"Viridis", "Plasma", "Hot", "Cool", "Pink", "Jet",
                                 "Twilight", "RdBu", "BrBG", "PiYG", "Spectral", "Greys"};

    std::vector<float> disp;
    std::vector<float> d;
    int points_c = 0;
    int timesteps = 0;
    float curr_disp = 0;
    float play_speed = 0;
    int gen_num = 1000;
    bool show_com = false;
    float lw = 1;
    bool autogen = false;
    std::vector<float> plot_size = {-1, 1, -1, 1, -1, 1};
    double G = 1;
    double dt = 0.01;

    std::vector<PP> t0;

    std::mutex mtx;

public:
    simQueue(std::vector<PP> points)
    {
        q = std::vector<std::vector<PP>>();
        sim = NBodySimulator(&points);
        //disp = new std::vector<float>(0);
        points_c = points.size();
        for (PP p : points)
            t0.push_back(p);
    }

    void generateSim(int steps);

    void drawUI(GLFWwindow *window);

    void adv(){
        curr_disp += play_speed;
        if (curr_disp < 1)
            curr_disp = 1;
        if (curr_disp > q.size()-1)
            curr_disp = q.size()-1;
    }

    void addPoint(PP point){
        t0.push_back(point);
        sim.points.push_back(point);
        points_c++;
    }

    void loadJson(std::ifstream *stream);
};