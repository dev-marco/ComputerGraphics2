#include "flock.h"
#include "firefly.h"

namespace Boids {

    Flock::Flock (Engine::Window &_window, unsigned number, Obstacles &_obstacles, const Spatial::Vec<3> &_position) :
        Engine::Object(_position, Spatial::Quaternion::identity), window(_window), obstacles(_obstacles) {
        for (unsigned i = 0; i < number; ++i) {
            static std::uniform_real_distribution<float_max_t>
                random_angle(-Spatial::DEG180, Spatial::DEG180),
                random_radius(1.0, 5.0);

            const float_max_t
                position_angle = random_angle(this->random_generator),
                position_radius = random_radius(this->random_generator);

            this->addChild(new Firefly({
                std::cos(position_angle) * position_radius,
                std::sin(position_angle) * position_radius,
                random_radius(this->random_generator)
            }));
        }

        window.event<Engine::Event::Keyboard>([ this ] (GLFWwindow *window, int key, int code, int action, int mods) {

            Engine::Object *leader = this->getLeader();

            if (GLFW_PRESS == action) {
                if (GLFW_KEY_Q == key) {
                    this->window.close();
                } else if (GLFW_KEY_V == key) {
                    this->view_mode = static_cast<Flock::ViewMode>((static_cast<int>(view_mode) + 1) % 3);
                } else if (GLFW_KEY_KP_ADD == key || GLFW_KEY_EQUAL == key) {
                    this->addChild(new Firefly(this->averagePosition() + Spatial::Vec<3>::random(-1.0, 1.0)));
                } else if (GLFW_KEY_MINUS == key || GLFW_KEY_KP_SUBTRACT == key) {
                    Engine::Object *last = this->getChildren().back();
                    this->removeChild(last);
                    last->destroy();
                }
            }
            if (GLFW_PRESS == action || GLFW_REPEAT == action) {
                if (leader != nullptr) {
                    if (GLFW_KEY_W == key) {
                        leader_angle_z = std::max(leader_angle_z - 0.02, leader_min_angle_z);
                        leader_angle_z = std::atan2(std::sin(leader_angle_z), std::cos(leader_angle_z));
                    } else if (GLFW_KEY_S == key) {
                        leader_angle_z = std::min(leader_angle_z + 0.02, leader_max_angle_z);
                        leader_angle_z = std::atan2(std::sin(leader_angle_z), std::cos(leader_angle_z));
                    } else if (GLFW_KEY_D == key) {
                        leader_angle_y = std::max(leader_angle_y - 0.02, leader_min_angle_y);
                        leader_angle_y = std::atan2(std::sin(leader_angle_y), std::cos(leader_angle_y));
                    } else if (GLFW_KEY_A == key) {
                        leader_angle_y = std::min(leader_angle_y + 0.02, leader_max_angle_y);
                        leader_angle_y = std::atan2(std::sin(leader_angle_y), std::cos(leader_angle_y));
                    } else if (GLFW_KEY_UP == key) {
                        leader_speed = std::min(leader_speed + 0.02, leader_max_speed);
                    } else if (GLFW_KEY_DOWN == key) {
                        leader_speed = std::max(leader_speed - 0.02, leader_min_speed);
                    }
                }
            }
        }, "keyboard.controls");
    }

};
