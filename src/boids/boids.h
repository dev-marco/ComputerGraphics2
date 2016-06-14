#ifndef SRC_BOIDS_BOIDS_H_
#define SRC_BOIDS_BOIDS_H_

#include "firefly.h"
#include "flock.h"
#include "../engine/engine.h"

namespace Boids {
    class Boids : Engine::Object {

        Engine::Window &window;
        Obstacles obstacles;
        Flock flock;
        unsigned start_pause_context, debug_timeout = 0;
        bool debug_mode = false, debug_last_status = false, debug_status = false, fog_enabled = false, light_enabled = false;

        static std::mt19937 random_generator;
        static std::uniform_int_distribution<unsigned> total_obstacles;

    public:
        Boids (Engine::Window &_window) :
            window(_window), obstacles(total_obstacles(random_generator)), flock(this->window, 20, this->obstacles)
        {

            this->addChild(&this->obstacles);
            this->addChild(&this->flock);

            this->window.addObject(this);

            this->window.event<Engine::Event::MouseClick>([ this ] (GLFWwindow *window, int key, int action, int mods) {
                if (GLFW_PRESS == action) {
                    if (GLFW_MOUSE_BUTTON_LEFT == key) {
                        if (this->window.isPaused()) {
                            this->window.unpause(this->start_pause_context);
                            this->debug_mode = false;
                            this->debug_last_status = false;
                            this->debug_status = false;
                            this->window.clearTimeout(this->debug_timeout);
                            this->debug_timeout = 0;
                        } else {
                            this->window.pause(this->start_pause_context);
                        }
                    } else if (GLFW_MOUSE_BUTTON_RIGHT == key) {

                        if (!this->window.isPaused()) {
                            this->window.pause(this->start_pause_context);
                        }

                        this->debug_mode = true;
                        this->debug_status = !this->debug_status;
                    }
                }
            }, "mouseclick.pause");


            window.event<Engine::Event::Keyboard>([ this ] (GLFWwindow *window, int key, int code, int action, int mods) {
                if (GLFW_PRESS == action) {
                    if (GLFW_KEY_F == key) {
                        this->fog_enabled = !this->fog_enabled;
                        if (this->fog_enabled) {
                            float fog_color[4] = { 0.0, 0.0, 0.0, 1.0 };
                            glFogi(GL_FOG_MODE, GL_EXP);
                            glFogfv(GL_FOG_COLOR, fog_color);
                            glFogf(GL_FOG_DENSITY, 0.05);
                            glHint(GL_FOG_HINT, GL_NICEST);
                            glFogf(GL_FOG_START, 10.0);
                            glFogf(GL_FOG_END, 50.0);
                            glEnable(GL_FOG);
                            glClearColor(fog_color[0], fog_color[1], fog_color[2], fog_color[3]);
                        } else {
                            glDisable(GL_FOG);
                        }
                    } else if (GLFW_KEY_L == key) {
                        this->light_enabled = !this->light_enabled;
                        if (this->light_enabled) {
                            glEnable(GL_LIGHTING);
                            glEnable(GL_LIGHT0);

                            constexpr float
                                light_model_ambient[4] = { 0.0, 0.0, 0.0, 0.0 },
                                light_ambient[4] = { 0.0, 0.0, 0.0, 1.0 },
                                light_diffuse[4] = { 0.2, 0.2, 0.2, 1.0 },
                                light_specular[4] = { 0.2, 0.2, 0.2, 1.0 };

                            Engine::Vec<4, float> light_position = { 0.0, 0.0, 5.0, 1.0 };

                            glLightModelfv(GL_LIGHT_MODEL_AMBIENT, light_model_ambient);

                            glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, 1.0);
                            glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 0.0);

                            glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, (-light_position).data());
                            glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 0.0);
                            glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 180.0);

                            glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0);
                            glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.0);
                            glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.0);

                            glLightfv(GL_LIGHT0, GL_POSITION, light_position.data());
                            glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
                            glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
                            glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

                            glEnable(GL_COLOR_MATERIAL);
                            glColorMaterial(GL_FRONT, GL_DIFFUSE);
                            glColorMaterial(GL_FRONT, GL_SPECULAR);
                        } else {
                            glDisable(GL_LIGHTING);
                        }
                    }
                }
            });
        }

        inline void beforeAlwaysUpdate (float_max_t now, float_max_t delta_time, unsigned tick) {
            int width, height;

            window.getFramebufferSize(width, height);

            glViewport(0, 0, width, height);

            Engine::Draw::perspective(90.0, static_cast<float_max_t>(width) / height);
            Engine::Draw::lookAt(this->flock.eyePos(), this->flock.averagePosition(), { 0.0, 0.0, 1.0 });

            if (this->debug_mode) {
                if (this->debug_last_status != this->debug_status) {
                    this->debug_last_status = this->debug_status;
                    this->debugInfo(std::cout);
                    this->window.unpause(this->start_pause_context);
                } else if (!this->window.isPaused()) {
                    this->window.pause(this->start_pause_context);
                }
            }
        }

    };
};

#endif
