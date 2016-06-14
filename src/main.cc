#include <string>
#include <iostream>
#include <chrono>
#include <random>
#include "engine/engine.h"
#include "boids/boids.h"

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

        Boids::Boids boids(window);

        window.makeCurrentContext();

        glewInit();

        glShadeModel(GL_SMOOTH);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        glEnable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GREATER, 0.0);

        glEnable(GL_NORMALIZE);

        // Engine::Shader::Program program;
        //
        // try {
        //     program.attachVertexShaderFile("src/engine/shaders/lighting.vert");
        //     program.attachFragmentShaderFile("src/engine/shaders/lighting.frag");
        //     program.link();
        // } catch (std::string e) {
        //     std::cerr << e << std::endl;
        //     return -1;
        // }

        // window.setShader(&program);

        // program.onAfterUse([] (Engine::Shader::Program *program) {
        //     GLfloat mp[16], mv[16];
        //     glGetFloatv(GL_PROJECTION_MATRIX, mp);
        //     glGetFloatv(GL_MODELVIEW_MATRIX, mv);
        //     glUniformMatrix4fv(program->getUniformLocationARB("scriptProj"), 1, GL_FALSE, &mp[0]);
        //     glUniformMatrix4fv(program->getUniformLocationARB("scriptModelView"), 1, GL_FALSE, &mv[0]);
        //     glUniform1fARB(program->getUniformLocationARB("scriptTime"), glfwGetTime());
        // });

        // GLint v;
        //
        // glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &v);
        // std::cout << v << std::endl;
        // glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &v);
        // std::cout << v << std::endl;
        // glGetIntegerv(GL_MAX_VERTEX_UNIFORM_BLOCKS, &v);
        // std::cout << v << std::endl;
        // glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_BLOCKS, &v);
        // std::cout << v << std::endl;
        // glGetIntegerv(GL_MAX_GEOMETRY_UNIFORM_BLOCKS, &v);
        // std::cout << v << std::endl;
        // glGetIntegerv(GL_MAX_LIGHTS, &v);
        // std::cout << v << std::endl;

        while (!window.shouldClose()) {

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ACCUM_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

            window.update();
            window.draw();
            window.swapBuffers();

            glfwPollEvents();

            unsigned fps = window.sync(WINDOW_FPS);
            if (fps != WINDOW_FPS) {
                std::cout << fps << " FPS" << std::endl;
            }
        }

        window.update();
    } else {
        std::cerr << "ERROR: Could not initialize window" << std::endl;
    }

    glfwTerminate();

    return 0;
}
