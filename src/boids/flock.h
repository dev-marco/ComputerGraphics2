#ifndef SRC_BOIDS_FLOCK_H_
#define SRC_BOIDS_FLOCK_H_

#include <set>
#include "../engine/engine.h"

namespace Boids {

    class Flock : public Engine::Object {

        Engine::Object *obstacles;
        std::set<Engine::Object *> fireflies;
        bool values_set = false;
        Engine::Vec<3> avg_position, avg_speed, avg_acceleration;
        float_max_t min_x, min_y, min_z, max_x, max_y, max_z;

        void setAverages (void) {
            this->avg_position = this->avg_speed = this->avg_acceleration = Engine::Vec<3>::zero;
            this->min_x = this->min_y = this->min_z =  std::numeric_limits<float_max_t>::infinity();
            this->max_x = this->max_y = this->max_z = -std::numeric_limits<float_max_t>::infinity();

            if (this->fireflies.size()) {

                for (const auto &firefly : this->fireflies) {
                    static Engine::Vec<3> position = firefly->getPosition();
                    this->min_x = std::min(this->min_x, position[0]);
                    this->min_y = std::min(this->min_y, position[1]);
                    this->min_z = std::min(this->min_z, position[2]);
                    this->max_x = std::max(this->max_x, position[0]);
                    this->max_y = std::max(this->max_y, position[1]);
                    this->max_z = std::max(this->max_z, position[2]);
                    this->avg_position += position;
                    this->avg_speed += firefly->getSpeed();
                    this->avg_acceleration += firefly->getAcceleration();
                }

                this->avg_position /= this->fireflies.size();
                this->avg_speed /= this->fireflies.size();
                this->avg_acceleration /= this->fireflies.size();

                this->values_set = true;
            }
        }

    public:

        Flock (unsigned number, Engine::Object *_obstacles, const Engine::Vec<3> &_position = Engine::Vec<3>::zero);

        const std::set<Engine::Object *> &getBoids (void) const { return this->fireflies; }
        const std::list<Engine::Object *> &getObstacles (void) const { return this->obstacles->getChildren(); }

        void afterUpdate (float_max_t now, float_max_t delta_time, unsigned tick) override {
            this->values_set = false;
        }

        void onAddChild (Engine::Object *child) override {
            if (child->getType() == "firefly") {
                this->fireflies.insert(child);
                this->values_set = false;
            }
        }

        void onRemoveChild (Engine::Object *child) override {
            if (child->getType() == "firefly") {
                this->fireflies.erase(child);
                this->values_set = false;
            }
        }

        Engine::Vec<3> averagePosition (void) {
            if (!this->values_set) {
                this->setAverages();
            }
            return this->avg_position;
        }

        Engine::Vec<3> averageSpeed (void) {
            if (!this->values_set) {
                this->setAverages();
            }
            return this->avg_speed;
        }

        Engine::Vec<3> averageAcceleration (void) {
            if (!this->values_set) {
                this->setAverages();
            }
            return this->avg_acceleration;
        }

        Engine::Vec<3> minXYZ (void) {
            if (!this->values_set) {
                this->setAverages();
            }
            return { this->min_x, this->min_y, this->min_z };
        }

        Engine::Vec<3> maxXYZ (void) {
            if (!this->values_set) {
                this->setAverages();
            }
            return { this->max_x, this->max_y, this->max_z };
        }

    };

}

#endif
