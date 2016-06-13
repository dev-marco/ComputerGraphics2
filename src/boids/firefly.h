#ifndef SRC_BOIDS_BIRD_H_
#define SRC_BOIDS_BIRD_H_

#include <iostream>
#include <random>
#include <chrono>
#include "flock.h"
#include "../engine/engine.h"

namespace Boids {

    class Firefly : public Engine::Object {

        static bool getLeader (void) {
            static bool leader = false;
            bool result = leader;
            leader = true;
            return result;
        }

        float_max_t max_speed;

        Engine::Vec<3> separation (void) const {

            constexpr float_max_t
                separe = 2.0,
                separe2 = separe * separe;
            Engine::Vec<3> result(0);
            unsigned found = 0;

            for (const auto &other : static_cast<Flock *>(this->getParent())->getBoids()) {
                if (other != this) {
                    Engine::Vec<3> diff = this->getPosition() - other->getPosition();
                    float_max_t distance2 = diff.length2();
                    if (distance2 > 0 && distance2 < separe2) {
                        diff.normalize();
                        result += diff / std::sqrt(distance2);
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

            // std::cout << "sep " << result << std::endl;

            return result;
        }

        Engine::Vec<3> alignment (void) const {

            constexpr float_max_t
                min_distance = 10.0,
                min_distance2 = min_distance * min_distance;
            Engine::Vec<3> result(0);
            unsigned found = 0;

            for (const auto &other : static_cast<Flock *>(this->getParent())->getBoids()) {
                if (other != this) {
                    float_max_t distance2 = this->getPosition().distance2(other->getPosition());
                    if (distance2 > 0 && distance2 < min_distance2) {
                        result += other->getSpeed();
                        ++found;
                    }
                }
            }

            if (found > 0 && result.length2() > 0) {
                result /= found;
                result.resize(this->getMaxSpeed());
                result -= this->getSpeed();
                result.clamp(0.0, this->getMaxForce());
            }

            // std::cout << "ali " << result << std::endl;

            return result;
        }

        Engine::Vec<3> cohesion (void) const {

            constexpr float_max_t
                min_distance = 10.0,
                min_distance2 = min_distance * min_distance;
            Engine::Vec<3> result(0);
            unsigned found = 0;

            for (const auto &other : static_cast<Flock *>(this->getParent())->getBoids()) {
                if (other != this) {
                    float_max_t distance2 = this->getPosition().distance2(other->getPosition());
                    if (distance2 > 0 && distance2 < min_distance2) {
                        result += other->getPosition();
                        ++found;
                    }
                }
            }

            if (found > 0 && result.length2() > 0) {
                result /= found;
                result -= this->getPosition();
                result.resize(this->getMaxSpeed());
                result -= this->getSpeed();
                result.clamp(0.0, this->getMaxForce());
            }

            // std::cout << "coh " << result << std::endl;

            return result;
        }

        Engine::Vec<3> obstacles (void) {

            constexpr float_max_t
                separe = 2.0,
                separe2 = separe * separe;

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

            for (const auto &other : static_cast<Flock *>(this->getParent())->getObstacles()) {

                Engine::Mesh *mesh = other->getMesh();

                if (mesh->getType() == "cylinder" || mesh->getType() == "cone") {

                    Engine::Cone *cone = static_cast<Engine::Cone *>(mesh);

                    const Engine::Vec<3>
                        tower_start = cone->getStart(),
                        tower_end = cone->getEnd();
                    distance = Engine::Mesh::distancePointCylinder(this->getPosition(), tower_start, tower_end, std::max(cone->getBaseRadius(), cone->getTopRadius()));

                    if (distance < separe) {

                        std::cout << mesh->getType() << std::endl;
                        Engine::Mesh::distancePointRay(this->getPosition(), tower_start, tower_end, near_point);
                        Engine::Vec<3> diff = this->getPosition() - near_point;
                        diff.normalize();
                        result += diff / distance;
                        ++found;
                    }
                } else if (mesh->getType() == "sphere3d") {

                    Engine::Sphere3D *sphere = static_cast<Engine::Sphere3D *>(mesh);
                    distance = Engine::Mesh::distancePointSphere(this->getPosition(), other->getPosition(), sphere->getRadius());

                    if (distance < separe) {
                        Engine::Vec<3> diff = this->getPosition() - other->getPosition();
                        diff.normalize();
                        result += diff / distance;
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

        bool leader, wing_down = true;
        std::list<Engine::Quaternion> samples;
        float_max_t wing_period, wing_time = 0.0;

        Engine::Mesh drawing,
            right_wing = Engine::Mesh({ 0.06,  0.0, 0.05 }),
            left_wing = Engine::Mesh({ 0.06,  0.0, 0.05 });

        static Engine::Sphere3D light, head, right_eye_sphere, left_eye_sphere;
        static Engine::Cylinder body;
        static Engine::Ellipse2D right_wing_ellipse, left_wing_ellipse;

        static Engine::BackgroundColor green, black, gray;

        static std::default_random_engine random_generator;

// TODO wing add force
// TODO wing period

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
                0.0, 1.0
            ), leader(getLeader()) {

                static std::uniform_real_distribution<float_max_t> _wing_period(0.2, 1.0);

                static int state = 0;
                Engine::Vec<3> position = Engine::Vec<3>::random(2.0, 5.0);
                if (state == 0) {
                    position[0] = -position[0];
                } else if (state == 1) {
                    position[1] = -position[1];
                } else if (state == 2) {
                    position[0] = -position[0];
                    position[1] = -position[1];
                } else {
                    state = -1;
                }
                ++state;

                this->wing_period = _wing_period(random_generator);

                this->drawing.addChild(&Firefly::light);
                this->drawing.addChild(&Firefly::body);
                this->drawing.addChild(&Firefly::head);

                this->drawing.addChild(&Firefly::right_eye_sphere);
                this->drawing.addChild(&Firefly::left_eye_sphere);

                this->right_wing.addChild(&Firefly::right_wing_ellipse);
                this->drawing.addChild(&this->right_wing);

                this->left_wing.addChild(&Firefly::left_wing_ellipse);
                this->drawing.addChild(&this->left_wing);

                this->setMesh(&this->drawing);
                this->setPosition(position);
                this->samples.clear();
                this->samples.push_back(Engine::Quaternion::difference(this->getSpeed(), Engine::Vec<3>::axisX));
            };

        inline void beforeUpdate (float_max_t now, float_max_t delta_time, unsigned tick) override {

            constexpr float_max_t
                min_wing = -Engine::DEG15,
                max_wing = Engine::DEG45,
                delta_wing = max_wing - min_wing;

            float_max_t wing_angle;

            this->setAcceleration(this->separation() * 2.0 + this->alignment() + this->cohesion() + this->obstacles() * 3.0);

            if (this->leader) {
                this->applyForce({ 10.0, 0.0, 20.0 });
            }

            if (this->wing_down) {
                this->applyForce(this->getOrientation().rotated({ 0.0, 0.0,  3.0 }));
                wing_angle = Engine::Easing::Back::Out(this->wing_time, max_wing, -delta_wing, this->wing_period);
            } else {
                this->applyForce(this->getOrientation().rotated({ 0.0, 0.0, -3.0 }));
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

            // std::cout << this->getSpeed() << std::endl << std::endl;
        }

        inline void afterUpdate (float_max_t now, float_max_t delta_time, unsigned tick) override {
            static constexpr unsigned max_sample_size = 50;
            this->samples.push_back(Engine::Quaternion::difference(Engine::Vec<3>::axisX, this->getSpeed()));
            if (this->samples.size() > max_sample_size) {
                this->samples.pop_front();
            }
            this->setOrientation(this->getOrientation().lerped(Engine::Quaternion::average(this->samples), 0.1));
        }

        inline std::string getType () const override { return "firefly"; }

    };

}

#endif
