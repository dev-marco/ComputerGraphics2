#ifndef SRC_BOIDS_OBSTACLES_H_
#define SRC_BOIDS_OBSTACLES_H_

#include <chrono>
#include <random>
#include "../engine/engine.h"

namespace Boids {
    class Obstacles : public Engine::Object {

        Engine::BackgroundColor
            tower_color =  Engine::BackgroundColor(Engine::Color::rgba( 50,  50,  50, 1.0)),
            ground_color = Engine::BackgroundColor(Engine::Color::rgba(  0,  70,   0, 1.0));
        Engine::Cone tower = Engine::Cone(Spatial::Vec<3>::origin, { 0.0, 0.0, 5.0 }, 0.5, 0.0, &this->tower_color);
        Engine::Rectangle2D ground = Engine::Rectangle2D(Spatial::Vec<3>::origin, 5000.0, 5000.0, Spatial::Quaternion::identity, &this->ground_color);
        Engine::Object
            tower_obj = Engine::Object(Spatial::Vec<3>::origin, Spatial::Quaternion::identity, true, &this->tower),
            ground_obj = Engine::Object({ -2500.0, 2500.0, 0.0 }, Spatial::Quaternion::identity, true, &this->ground);

    public:

        Obstacles (const unsigned &number) :
            Engine::Object(Spatial::Vec<3>::origin, Spatial::Quaternion::identity)
        {
            this->addChild(&this->tower_obj);
            this->addChild(&this->ground_obj);

            std::mt19937 random_generator(std::chrono::system_clock::now().time_since_epoch().count());
            std::uniform_int_distribution<unsigned char> random_rgb(0, 255);
            std::uniform_int_distribution<int> random_type(0, 2);
            std::uniform_real_distribution<float_max_t> random_xy(5.0, 200.0), random_z(0.0, 30.0), random_radius(0.5, 1.5), random_angle(-Spatial::DEG180, Spatial::DEG180);

            for (unsigned i = 0; i < number; ++i) {

                float_max_t
                    pos_angle = random_angle(random_generator),
                    pos_radius = random_xy(random_generator);
                Engine::Mesh *mesh = nullptr;
                Spatial::Vec<3> position = { std::cos(pos_angle) * pos_radius, std::sin(pos_angle) * pos_radius, random_z(random_generator) };
                Engine::Background *bg = new Engine::BackgroundColor(
                    Engine::Color::rgb(random_rgb(random_generator), random_rgb(random_generator), random_rgb(random_generator))
                );

                switch (random_type(random_generator)) {
                    case 0:
                        mesh = new Engine::Sphere3D(position, random_radius(random_generator), bg);
                    break;
                    case 1:
                        mesh = new Engine::Cylinder(position, position + Spatial::Vec<3>::random(0.5, 3.0), random_radius(random_generator), bg);
                    break;
                    default:
                        mesh = new Engine::Cone(position, position + Spatial::Vec<3>::random(0.5, 3.0), random_radius(random_generator), random_radius(random_generator) - random_radius.min(), bg);
                    break;
                }

                this->addChild(new Engine::Object(
                    Spatial::Vec<3>::origin,
                    Spatial::Quaternion::identity,
                    true,
                    mesh
                ));
            }
        }

        const Engine::Cone &getTower (void) const {
            return this->tower;
        }

        float_max_t getTowerHeight (void) const {
            return this->getTower().getHeight();
        }
    };
};

#endif
