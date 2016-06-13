#include "firefly.h"

namespace Boids {

    Engine::Sphere3D
        Firefly::light = Engine::Sphere3D(Engine::Vec<3>::origin, 0.07, &Firefly::green),
        Firefly::head = Engine::Sphere3D({ 0.2, 0.0, 0.0 }, 0.1, &Firefly::black),
        Firefly::right_eye_sphere = Engine::Sphere3D({ 0.265, 0.03, 0.045 }, 0.02, &Firefly::green),
        Firefly::left_eye_sphere = Engine::Sphere3D({ 0.265, -0.03, 0.045 }, 0.02, &Firefly::green);
    Engine::Cylinder Firefly::body = Engine::Cylinder(Engine::Vec<3>::origin, { 0.2, 0.0, 0.0 }, 0.05, &black);
    Engine::Ellipse2D
        Firefly::right_wing_ellipse = Engine::Ellipse2D({ 0.0,  0.1, 0.0 }, 0.1, 0.8, 2.0, &Firefly::gray),
        Firefly::left_wing_ellipse = Engine::Ellipse2D({ 0.0, -0.1, 0.0 }, 0.1, 0.8, 2.0, &Firefly::gray);

    Engine::BackgroundColor
        Firefly::green = Engine::BackgroundColor(Engine::Color::rgba(0, 255, 0, 1.0)),
        Firefly::black = Engine::BackgroundColor(Engine::Color::rgba(0, 0, 0, 1.0)),
        Firefly::gray = Engine::BackgroundColor(Engine::Color::rgba(200, 200, 200, 0.8));

    std::default_random_engine Firefly::random_generator = std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count());

};
