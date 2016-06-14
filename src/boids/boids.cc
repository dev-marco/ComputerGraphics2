#include "boids.h"

namespace Boids {

    std::mt19937 Boids::random_generator(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<unsigned> Boids::total_obstacles(100, 200);

};
