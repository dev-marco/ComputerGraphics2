#ifndef SRC_BOIDS_BIRD_H_
#define SRC_BOIDS_BIRD_H_

#include <iostream>
#include <random>
#include <chrono>
#include "flock.h"
#include "../engine/engine.h"

namespace Boids {

    class Firefly : public Engine::Object {

        Engine::Vec<3> flocking (void) const {

            Flock *flock = this->getFlock();
            constexpr float_max_t
                view_distance = 2.0,
                view_distance2 = view_distance * view_distance,
                separe_distance = 1.0,
                separe_distance2 = separe_distance * separe_distance,
                weight_follow_speed = 3.0,
                weight_follow_position = 3.0,
                weight_alignment = 1.0,
                weight_cohesion = 1.0,
                weight_separation = 5.0;
            Engine::Vec<3>
                follow_speed = Engine::Vec<3>::zero,
                follow_position = Engine::Vec<3>::zero,
                alignment = Engine::Vec<3>::zero,
                cohesion = Engine::Vec<3>::zero,
                separation = Engine::Vec<3>::zero;

            if (flock != nullptr) {
                unsigned
                    found_ali_coe = 0,
                    found_sep = 0;
                const auto boids = flock->getBoids();

                Engine::Object *leader = flock->getLeader();

                if (leader && leader != this) {
                    follow_speed = leader->getSpeed().resized(this->getMaxSpeed());
                    follow_speed -= this->getSpeed();
                    follow_position = (leader->getPosition() - this->getPosition()).resized(this->getMaxSpeed());
                    follow_position -= this->getSpeed();
                }

                for (const auto &other : boids) {
                    if (other != this) {
                        Engine::Vec<3> diff = this->getPosition() - other->getPosition();
                        float_max_t distance2 = diff.length2();
                        if (distance2 > 0) {
                            if (distance2 < view_distance2) {
                                alignment += other->getSpeed();
                                cohesion += other->getPosition();
                                ++found_ali_coe;
                            }
                            if (distance2 < separe_distance2) {
                                separation += diff.normalized() / std::sqrt(distance2);
                                ++found_sep;
                            }
                        }
                    }
                }

                if (found_ali_coe > 0) {

                    if (alignment.length2() > 0) {
                        alignment /= found_ali_coe;
                        alignment.resize(this->getMaxSpeed());
                        alignment -= this->getSpeed();
                    }

                    if (cohesion.length2() > 0) {
                        cohesion /= found_ali_coe;
                        cohesion -= this->getPosition();
                        cohesion.resize(this->getMaxSpeed());
                        cohesion -= this->getSpeed();
                    }
                }

                if (found_sep > 0) {
                    separation /= found_sep;
                    separation.resize(this->getMaxSpeed());
                    separation -= this->getSpeed();
                }
            }

            return
                follow_speed * weight_follow_speed +
                follow_position * weight_follow_position +
                alignment * weight_alignment +
                cohesion * weight_cohesion +
                separation * weight_separation;
        }

        Engine::Vec<3> obstacles (void) {

            constexpr float_max_t separe = 2.0;

            Engine::Vec<3>
                result(0),
                near_point;
            unsigned found = 0;

            float_max_t distance = Engine::Mesh::distancePointPlane(this->getPosition(), Engine::Vec<3>::axisZ, Engine::Vec<3>::origin, near_point);

            if (distance < separe) {
                Engine::Vec<3> diff = this->getPosition() - near_point;
                diff.normalize();
                result += diff / distance;
                ++found;
            }

            for (const auto &other : this->getFlock()->getObstacles()) {

                Engine::Mesh *mesh = other->getMesh();

                if (mesh->getType() == "cylinder" || mesh->getType() == "cone") {

                    Engine::Cone *cone = static_cast<Engine::Cone *>(mesh);

                    const Engine::Vec<3>
                        tower_start = cone->getStart(),
                        tower_end = cone->getEnd();
                    distance = Engine::Mesh::distancePointRay(this->getPosition(), tower_start, tower_end, near_point);

                    if (distance < separe) {

                        Engine::Vec<3> diff = this->getSpeed() - near_point;
                        result += diff;
                        ++found;
                    }
                } else if (mesh->getType() == "sphere3d") {

                    Engine::Sphere3D *sphere = static_cast<Engine::Sphere3D *>(mesh);
                    distance = this->getPosition().distance(sphere->getPosition());

                    if (distance < separe) {
                        Engine::Vec<3> diff = this->getSpeed() - sphere->getPosition();
                        result += diff;
                        ++found;
                    }
                }

            }

            if (found > 0) {
                result /= found;
                result.resize(this->getMaxSpeed());
                result -= this->getSpeed();
                result.clamp(0.0, this->getMaxForce());
            }

            return result;
        }

        bool wing_down = true;
        std::list<Engine::Quaternion> samples;
        float_max_t wing_period, wing_time = 0.0;

        Engine::Mesh drawing,
            right_wing = Engine::Mesh({ 0.06,  0.0, 0.05 }),
            left_wing = Engine::Mesh({ 0.06,  0.0, 0.05 });

        Engine::Sphere3D light = Engine::Sphere3D(Engine::Vec<3>::origin, 0.07, nullptr);

        Flock *flock = nullptr;

        static Engine::Sphere3D head, right_eye_sphere, left_eye_sphere;
        static Engine::Cylinder body;
        static Engine::Ellipse2D right_wing_ellipse, left_wing_ellipse;

        static Engine::BackgroundColor green, blue, black, gray;

        static std::mt19937 random_generator;

    public:
        Firefly (const Engine::Vec<3> &_position = Engine::Vec<3>::zero, const Engine::Quaternion &_orientation = Engine::Quaternion::identity) :
            Engine::Object(
                _position,
                _orientation,
                true,
                nullptr,
                nullptr,
                Engine::Vec<3>::zero,
                Engine::Vec<3>::zero,
                1.0,
                0.0, 3.0
            ) {
                static std::uniform_real_distribution<float_max_t> _wing_period(0.2, 1.0);

                this->wing_period = _wing_period(random_generator);

                this->drawing.addChild(&this->light);
                this->drawing.addChild(&Firefly::body);
                this->drawing.addChild(&Firefly::head);

                this->drawing.addChild(&Firefly::right_eye_sphere);
                this->drawing.addChild(&Firefly::left_eye_sphere);

                this->right_wing.addChild(&Firefly::right_wing_ellipse);
                this->drawing.addChild(&this->right_wing);

                this->left_wing.addChild(&Firefly::left_wing_ellipse);
                this->drawing.addChild(&this->left_wing);

                this->setMesh(&this->drawing);
                this->samples.clear();
                this->samples.push_back(Engine::Quaternion::difference(this->getSpeed(), Engine::Vec<3>::axisX));
            };

        inline void onSetParent (Engine::Object *parent) override {
            this->flock = dynamic_cast<Flock *>(parent);
        }

        inline void onRemoveParent (Engine::Object *parent) override {
            this->flock = nullptr;
        }

        inline void beforeUpdate (float_max_t now, float_max_t delta_time, unsigned tick) override {

            constexpr float_max_t
                min_wing = -Engine::DEG15,
                max_wing = Engine::DEG45,
                delta_wing = max_wing - min_wing;
            const Flock *flock = this->getFlock();
            const Engine::Object *leader = flock->getLeader();

            float_max_t wing_angle;

            if (flock) {
                if (leader == this) {
                    this->light.setBackground(&Firefly::blue);
                } else {
                    this->setAcceleration(this->flocking() + this->obstacles() * 20.0);
                    this->light.setBackground(&Firefly::green);
                }
            }

            if (this->wing_down) {
                if (leader != this) {
                    this->applyForce(this->getOrientation().rotated({ 0.0, 0.0,  3.0 }));
                }
                wing_angle = Engine::Easing::Back::Out(this->wing_time, max_wing, -delta_wing, this->wing_period);
            } else {
                if (leader != this) {
                    this->applyForce(this->getOrientation().rotated({ 0.0, 0.0, -3.0 }));
                }
                wing_angle = Engine::Easing::Back::Out(this->wing_time, min_wing, delta_wing, this->wing_period);
            }

            if (this->wing_time >= this->wing_period) {
                this->wing_time = 0.0;
                this->wing_down = !this->wing_down;
            } else {
                this->wing_time += delta_time;
            }

            this->right_wing.setOrientation(Engine::Quaternion::axisAngle(Engine::Vec<3>::axisX, wing_angle));
            this->left_wing.setOrientation(Engine::Quaternion::axisAngle(Engine::Vec<3>::axisX, -wing_angle));
        }

        inline void afterUpdate (float_max_t now, float_max_t delta_time, unsigned tick) override {
            static constexpr unsigned max_sample_size = 50;
            this->samples.push_back(Engine::Quaternion::difference(Engine::Vec<3>::axisX, this->getSpeed()));
            if (this->samples.size() > max_sample_size) {
                this->samples.pop_front();
            }
            this->setOrientation(this->getOrientation().lerped(Engine::Quaternion::average(this->samples), 0.1));
        }

        inline Flock *getFlock (void) const {
            return this->flock;
        }

        inline std::string getType () const override { return "firefly"; }

    };

}

#endif
