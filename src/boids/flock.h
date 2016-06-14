#ifndef SRC_BOIDS_FLOCK_H_
#define SRC_BOIDS_FLOCK_H_

#include <set>
#include <random>
#include <chrono>
#include "obstacles.h"
#include "../engine/engine.h"

namespace Boids {

    constexpr float_max_t
        leader_max_angle_y =  Engine::DEG180,
        leader_min_angle_y = -Engine::DEG180,
        leader_max_angle_z =  Engine::DEG45,
        leader_min_angle_z = -Engine::DEG45,
        leader_min_speed = 0.5,
        leader_max_speed = 2.0,
        leader_start_speed = (leader_min_speed + leader_min_speed) / 2.0;

    class Flock : public Engine::Object {

    public:

        enum ViewMode : int {
            VIEW_TOWER, VIEW_BACK, VIEW_SIDE
        };

    private:

        static constexpr unsigned logs_maxsize = 50;
        ViewMode view_mode = ViewMode::VIEW_TOWER;
        Engine::Window &window;
        Obstacles &obstacles;
        Engine::Object *leader = nullptr;
        std::set<Engine::Object *> fireflies;
        bool values_set = false, smooth_values_set = false;
        Engine::Vec<3> avg_position, avg_speed, avg_acceleration, smooth_avg_position, smooth_avg_speed;
        std::list<Engine::Vec<3>> position_log, speed_log;
        float_max_t min_x, min_y, min_z, max_x, max_y, max_z, leader_angle_z = 0.0, leader_angle_y = 0.0, leader_speed = leader_start_speed;
        std::mt19937 random_generator = std::mt19937(std::chrono::system_clock::now().time_since_epoch().count());

        void setAverages (bool force = false) {
            if (!this->values_set || force) {
                this->avg_position = this->avg_speed = this->avg_acceleration = Engine::Vec<3>::zero;
                this->min_x = this->min_y = this->min_z =  std::numeric_limits<float_max_t>::infinity();
                this->max_x = this->max_y = this->max_z = -std::numeric_limits<float_max_t>::infinity();

                if (this->fireflies.size() > 1) {

                    unsigned computed = this->fireflies.size() - 1;

                    for (const auto &firefly : this->fireflies) {
                        if (firefly != this->getLeader()) {
                            static Engine::Vec<3> position;
                            position = firefly->getPosition();
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
                    }

                    this->avg_position /= computed;
                    this->avg_speed /= computed;
                    this->avg_acceleration /= computed;

                    this->position_log.push_back(avg_position);
                    this->speed_log.push_back(avg_speed);

                    if (this->position_log.size() > logs_maxsize) {
                        this->position_log.pop_front();
                        this->speed_log.pop_front();
                    }

                    this->smooth_values_set = false;
                    this->values_set = true;
                }
            }
        }

        void setSmoothAverages (bool force = false) {
            this->setAverages();
            if (!this->smooth_values_set || force) {
                this->smooth_avg_position = Engine::Vec<3>::average(this->position_log);
                this->smooth_avg_speed = Engine::Vec<3>::average(this->speed_log);
                this->smooth_values_set = true;
            }
        }

    public:

        Flock (Engine::Window &_window, unsigned number, Obstacles &_obstacles, const Engine::Vec<3> &_position = Engine::Vec<3>::zero);

        const std::set<Engine::Object *> &getBoids (void) const { return this->fireflies; }
        const std::list<Engine::Object *> &getObstacles (void) const { return this->obstacles.getChildren(); }

        void afterUpdate (float_max_t now, float_max_t delta_time, unsigned tick) override {
            Engine::Object *leader = this->getLeader();
            this->values_set = false;
            if (leader) {
                leader->setSpeed(Engine::Quaternion::eulerZYX(this->leader_angle_z, this->leader_angle_y, 0.0).rotated(Engine::Vec<3>::axisX) * this->leader_speed);
            }
        }

        void onAddChild (Engine::Object *child) override {
            if (child->getType() == "firefly") {
                if (this->leader == nullptr) {
                    this->leader = child;
                    child->setSpeed(Engine::Vec<3>({ 1.0, 1.0, 1.0 }).normalized());
                }
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

        Engine::Vec<3> eyePos (void) {

            if (this->view_mode == ViewMode::VIEW_TOWER) {
                return { 0.0, 0.0, this->obstacles.getTowerHeight() };
            } else {

                const Engine::Vec<3>
                    &flock_center = this->smoothAveragePosition(),
                    &flock_speed = this->smoothAverageSpeed();
                Engine::Vec<3> eye_pos;

                if (this->view_mode == ViewMode::VIEW_BACK) {
                    Engine::Mesh::intersectionRayBox(
                        flock_center,
                        flock_center + flock_speed,
                        { this->minX(), this->minY(), this->minZ() },
                        { this->maxX(), this->maxY(), this->maxZ() },
                        eye_pos
                    );
                } else {
                    Engine::Mesh::intersectionRayBox(
                        flock_center,
                        flock_center + flock_speed.cross(Engine::Vec<3>::axisZ),
                        { this->minX(), this->minY(), this->minZ() },
                        { this->maxX(), this->maxY(), this->maxZ() },
                        eye_pos
                    );
                }

                return eye_pos + (eye_pos - flock_center).normalized();
            }
        }

        Engine::Object *getLeader (void) const { return this->leader; }

        Engine::Vec<3> averagePosition (void) {
            this->setAverages();
            return this->avg_position;
        }

        Engine::Vec<3> smoothAveragePosition (void) {
            this->setSmoothAverages();
            return this->smooth_avg_position;
        }

        Engine::Vec<3> averageSpeed (void) {
            this->setSmoothAverages();
            return this->smooth_avg_speed;
        }

        Engine::Vec<3> smoothAverageSpeed (void) {
            this->setAverages();
            return Engine::Vec<3>::average(this->speed_log);
        }

        Engine::Vec<3> averageAcceleration (void) {
            this->setAverages();
            return this->avg_acceleration;
        }

        float_max_t minX (void) {
            this->setAverages();
            return this->min_x;
        }

        float_max_t minY (void) {
            this->setAverages();
            return this->min_y;
        }

        float_max_t minZ (void) {
            this->setAverages();
            return this->min_z;
        }

        float_max_t maxX (void) {
            this->setAverages();
            return this->max_x;
        }

        float_max_t maxY (void) {
            this->setAverages();
            return this->max_y;
        }

        float_max_t maxZ (void) {
            this->setAverages();
            return this->max_z;
        }

        inline std::string getType () const override { return "flock"; }

    };

}

#endif
