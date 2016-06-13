#include "flock.h"
#include "firefly.h"

namespace Boids {

    Flock::Flock (unsigned number, Engine::Object *_obstacles, const Engine::Vec<3> &_position) :
        Engine::Object(_position, Engine::Quaternion::identity), obstacles(_obstacles) {
        for (unsigned i = 0; i < number; ++i) {
            this->addChild(new Firefly());
        }
    }

};
