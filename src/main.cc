#include <string>
#include <iostream>
#include "engine/window.h"

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

        std::cout << std::string((char *) glGetString(GL_VERSION)) << std::endl;
        std::cout << "BLABALBALBLA" << std::endl;

        return 0;

        glShadeModel(GL_SMOOTH);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        glEnable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GREATER, 0.0);

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

        while (!window.shouldClose()) {
            int width, height;

            window.getFramebufferSize(width, height);

            glViewport(0, 0, width, height);

            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluPerspective(90.0, static_cast<double>(width) / height, 1, 100);

            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            glTranslated(0.0, 0.0, -5.0);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ACCUM_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

            glColor3f(0.0f, 0.0f, 0.0f);
            glRectf(-0.75f,0.75f, 0.75f, -0.75f);

            window.draw();
            window.update();

            glFlush();
            window.swapBuffers();

            glfwPollEvents();

            // unsigned fps = window.sync(WINDOW_FPS);
            // if (fps != WINDOW_FPS) {
            //     std::cout << fps << " FPS" << std::endl;
            // }
        }

        window.update();
    } else {
        std::cerr << "ERROR: Could not initialize window" << std::endl;
    }

    glfwTerminate();

    return 0;
}
