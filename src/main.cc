#include <string>
#include <iostream>
#include "engine/engine.h"

#define WINDOW_FPS 60


void recursiveTriangle(
    const Engine::Vec<3> &a,
    const Engine::Vec<3> &b,
    const Engine::Vec<3> &c,
    const float_max_t radius,
    const unsigned steps) {
    if (steps <= 0) {
        Engine::Draw::normal(a);
        Engine::Draw::vertex(a[0] * radius, a[1] * radius, a[2] * radius);

        Engine::Draw::normal(b);
        Engine::Draw::vertex(b[0] * radius, b[1] * radius, b[2] * radius);

        Engine::Draw::normal(c);
        Engine::Draw::vertex(c[0] * radius, c[1] * radius, c[2] * radius);
    } else {
        const unsigned next_step = steps - 1;
        const Engine::Vec<3>
            ab = ((a + b) * 0.5).normalized(),
            ac = ((a + c) * 0.5).normalized(),
            bc = ((b + c) * 0.5).normalized();

        recursiveTriangle( a, ab, ac, radius, next_step);
        recursiveTriangle( b, bc, ab, radius, next_step);
        recursiveTriangle( c, ac, bc, radius, next_step);
        recursiveTriangle(ab, bc, ac, radius, next_step);
    }
}

void drawsphere(unsigned steps, const Engine::Vec<3> &center = Engine::Vec<3>::zero, const float_max_t radius = 1.0) {
    constexpr float_max_t
        X = 0.525731112119133606,
        Z = 0.850650808352039932;

    static const Engine::Vec<3> vdata[12] = {
        {  -X, 0.0,   Z }, {   X, 0.0,   Z }, {  -X, 0.0,  -Z }, {   X, 0.0,  -Z },
        { 0.0,   Z,   X }, { 0.0,   Z,  -X }, { 0.0,  -Z,   X }, { 0.0,  -Z,  -X },
        {   Z,   X, 0.0 }, {  -Z,   X, 0.0 }, {   Z,  -X, 0.0 }, {  -Z,  -X, 0.0 }
    };

    constexpr unsigned tindices[20][3] = {
        { 0,  4,  1 }, { 0, 9,  4 }, { 9,  5, 4 }, {  4, 5, 8 }, { 4, 8,  1 },
        { 8, 10,  1 }, { 8, 3, 10 }, { 5,  3, 8 }, {  5, 2, 3 }, { 2, 7,  3 },
        { 7, 10,  3 }, { 7, 6, 10 }, { 7, 11, 6 }, { 11, 0, 6 }, { 0, 1,  6 },
        { 6,  1, 10 }, { 9, 0, 11 }, { 9, 11, 2 }, {  9, 2, 5 }, { 7, 2, 11 }
    };

    Engine::Draw::push();

    Engine::Draw::translate(center);
    Engine::Draw::begin(GL_TRIANGLES);

    for (const auto &indices : tindices) {
        recursiveTriangle(vdata[indices[0]], vdata[indices[1]], vdata[indices[2]], radius, steps);
    }

    Engine::Draw::end();
    Engine::Draw::pop();

}

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
            std::cout << e << std::endl;
            return -1;
        }

        window.setShader(&program);

        window.addObject(
            new Engine::Object(
                Engine::Vec<3>::zero,
                Engine::Quaternion::identity,
                true,
                new Engine::Rectangle2D({ -1.0, 0.5, 0.0 }, 4.0, 2.0),
                nullptr,
                new Engine::BackgroundColor(Engine::Color::rgba(50, 50, 50, 1.0))
            )
        );

        window.addObject(
            new Engine::Object(
                { 3.0, 3.0, 0.0 },
                Engine::Quaternion::axisAngle({ 1.0, 0.0, 0.0 }, Engine::DEG45),
                true,
                new Engine::Polygon2D(Engine::Vec<3>::zero, 1.0, 5),
                nullptr,
                new Engine::BackgroundColor(Engine::Color::rgba(50, 50, 50, 1.0))
            )
        );

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
            gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ACCUM_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);


            // glColor4d(0.5, 0.5, 0.5, 1.0);
            // drawsphere(3, { 0.0, 0.0, 0.0 }, 2.0);

            window.update();
            window.draw();

            glFlush();
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
