#include <string>
#include <iostream>
#include <chrono>
#include <random>
#include "engine/engine.h"
#include "boids/flock.h"

#define WINDOW_FPS 60
int main (int argc, const char *argv[]) {

    if (!glfwInit()) {
        std::cerr << "ERROR: Could not initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 1);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    Engine::Window window(800, 600, "Boids");

    if (window) {
        window.makeCurrentContext();

        glewInit();

        glShadeModel(GL_SMOOTH);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        glEnable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GREATER, 0.0);

        Engine::Shader::Program program;

        try {
            program.attachVertexShaderFile("src/engine/shaders/lighting.vert");
            program.attachFragmentShaderFile("src/engine/shaders/lighting.frag");
            program.link();
        } catch (std::string e) {
            std::cerr << e << std::endl;
            return -1;
        }

        // window.setShader(&program);

        program.onAfterUse([] (Engine::Shader::Program *program) {
            GLfloat mp[16], mv[16];
            glGetFloatv(GL_PROJECTION_MATRIX, mp);
            glGetFloatv(GL_MODELVIEW_MATRIX, mv);
            glUniformMatrix4fv(program->getUniformLocationARB("scriptProj"), 1, GL_FALSE, &mp[0]);
            glUniformMatrix4fv(program->getUniformLocationARB("scriptModelView"), 1, GL_FALSE, &mv[0]);
            glUniform1fARB(program->getUniformLocationARB("scriptTime"), glfwGetTime());
        });

        Engine::Object obstacles(
            { 0.0, 0.0, 0.0 },
            Engine::Quaternion::identity
        );

        Engine::Object tower(
            { 0.0, 0.0, 0.0 },
            Engine::Quaternion::identity,
            true,
            new Engine::Cone(Engine::Vec<3>::zero, { 0.0, 0.0, 5.0 }, 0.5, 0.0, new Engine::BackgroundColor(Engine::Color::rgba(50, 50, 50, 1.0))),
            nullptr
        );

        obstacles.addChild(&tower);

        std::mt19937 random_generator(std::chrono::system_clock::now().time_since_epoch().count());
        std::uniform_int_distribution<unsigned> random_obstacles(50, 80), random_rgb(0, 255);
        std::uniform_int_distribution<int> random_type(0, 2);
        std::uniform_real_distribution<float_max_t> random_xy(-10.0, 10.0), random_z(0.0, 10.0), random_radius(0.5, 1.5), random_angle(-Engine::DEG180, Engine::DEG180);

        for (unsigned i = 0, total = random_obstacles(random_generator); i < total; ++i) {

            Engine::Mesh *mesh = nullptr;
            Engine::Vec<3> position = { random_xy(random_generator), random_xy(random_generator), random_z(random_generator) };
            Engine::Background *bg = new Engine::BackgroundColor(
                Engine::Color::rgb(random_rgb(random_generator), random_rgb(random_generator), random_rgb(random_generator))
            );

            if (position[0] < 0.0) { position[0] -= 5.0; } else { position[0] += 5.0; }
            if (position[1] < 0.0) { position[1] -= 5.0; } else { position[1] += 5.0; }

            switch (random_type(random_generator)) {
                case 0:
                    std::cout << "sphere" << std::endl;
                    mesh = new Engine::Sphere3D(position, random_radius(random_generator), bg);
                break;
                case 1:
                    std::cout << "cylinder" << std::endl;
                    mesh = new Engine::Cylinder(position, position + Engine::Vec<3>::random(0.5, 3.0), random_radius(random_generator), bg);
                break;
                default:
                    std::cout << "cone" << std::endl;
                    mesh = new Engine::Cone(position, position + Engine::Vec<3>::random(0.5, 3.0), random_radius(random_generator), random_radius(random_generator), bg);
                break;
            }

            obstacles.addChild(new Engine::Object(
                Engine::Vec<3>::origin,
                Engine::Quaternion::identity,
                true,
                mesh
            ));
        }

        window.addObject(&obstacles);

        window.addObject(
            new Engine::Object(
                { -250.0, 250.0, 0.0 },
                Engine::Quaternion::identity,
                true,
                new Engine::Rectangle2D(Engine::Vec<3>::zero, 5000.0, 5000.0, Engine::Quaternion::identity, new Engine::BackgroundColor(Engine::Color::rgba(255, 255, 0, 1.0))),
                nullptr
            )
        );

        window.addObject(new Boids::Flock(50, &obstacles));

        // window.addObject(
        //     new Engine::Object(
        //         { 3.0, 3.0, 0.0 },
        //         Engine::Quaternion::axisAngle({ 1.0, 0.0, 0.0 }, Engine::DEG45),
        //         true,
        //         new Engine::Polygon2D(Engine::Vec<3>::zero, 1.0, 5),
        //         nullptr,
        //         new Engine::BackgroundColor(Engine::Color::rgba(50, 50, 50, 1.0))
        //     )
        // );
        //
        // window.addObject(
        //     new Engine::Object(
        //         Engine::Vec<3>::zero,
        //         Engine::Quaternion::identity,
        //         true,
        //         new Engine::Rectangle2D({ -2.0, 1.0, 0.0 }, 4.0, 2.0),
        //         nullptr,
        //         new Engine::BackgroundColor(Engine::Color::rgba(50, 50, 50, 1.0))
        //     )
        // );

        GLint v;

        glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &v);
        std::cout << v << std::endl;
        glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &v);
        std::cout << v << std::endl;
        glGetIntegerv(GL_MAX_VERTEX_UNIFORM_BLOCKS, &v);
        std::cout << v << std::endl;
        glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_BLOCKS, &v);
        std::cout << v << std::endl;
        glGetIntegerv(GL_MAX_GEOMETRY_UNIFORM_BLOCKS, &v);
        std::cout << v << std::endl;

        while (!window.shouldClose()) {
            int width, height;

            window.getFramebufferSize(width, height);

            glViewport(0, 0, width, height);

            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluPerspective(90.0, static_cast<float_max_t>(width) / height, 1, 100);

            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            gluLookAt(15.0, 0.0, 15.0, -1.0, 0.0, -1.0, 0.0, 0.0, 1.0);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ACCUM_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

            window.update();
            window.draw();

            glFlush();
            window.swapBuffers();

            glfwPollEvents();

            unsigned fps = window.sync(WINDOW_FPS);
            if (fps != WINDOW_FPS) {
                // std::cout << fps << " FPS" << std::endl;
            }
        }

        window.update();
    } else {
        std::cerr << "ERROR: Could not initialize window" << std::endl;
    }

    glfwTerminate();

    return 0;
}
