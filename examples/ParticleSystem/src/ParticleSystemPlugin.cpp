#include "ParticleSystemPlugin.hpp"
#include "ParticleSystem.hpp"

#include "LoopEngine/Camera/CameraSystem.hpp"
#include "LoopEngine/Input/InputSystem.hpp"
#include "LoopEngine/Input/InputController.hpp"
#include "LoopEngine/Platform/Display.hpp"
#include "LoopEngine/Application.hpp"
#include "spdlog/spdlog.h"
#include "glm/vec3.hpp"

using LoopEngine::Application;
using LoopEngine::Platform::Display;
using LoopEngine::Input::InputSystem;
using LoopEngine::Event::EventSystem;
using LoopEngine::Camera::get_default_camera;

ParticleSystemPlugin::ParticleSystemPlugin() {
    init_event_handler.connect<&ParticleSystemPlugin::init>(this);
    draw_event_handler.connect<&ParticleSystemPlugin::draw>(this);
    update_event_handler.connect<&ParticleSystemPlugin::update>(this);

    EventSystem::get_instance()->add_event_handler(&init_event_handler);
    EventSystem::get_instance()->add_event_handler(&draw_event_handler);
    EventSystem::get_instance()->add_event_handler(&update_event_handler);
}

ParticleSystemPlugin::~ParticleSystemPlugin() {
    EventSystem::get_instance()->remove_event_handler(&init_event_handler);
    EventSystem::get_instance()->remove_event_handler(&draw_event_handler);
    EventSystem::get_instance()->remove_event_handler(&update_event_handler);
}

void ParticleSystemPlugin::init(const InitEvent &event) {
    spdlog::info("ParticleSystemPlugin::init()");

    auto camera = get_default_camera();
    camera->set_perspective(60.0f, 16.0f / 12.0f, 0.1f, 1000.0f);
    camera->set_transform(glm::vec3(0.0f, 0.0f, -10.0f), glm::vec3(0.0f, 0.0f, 0.0f));

    particle_system = std::make_shared<ParticleSystem>(1000);
}

void ParticleSystemPlugin::update(const UpdateEvent &event) {
    glfwSetInputMode(Display::get_native_window_handle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    particle_system->update(event);

    glm::vec3 particle_velocity{};
    particle_velocity.x = std::uniform_real_distribution(-5.0f, 5.0f)(generator);
    particle_velocity.z = std::uniform_real_distribution(-5.0f, 5.0f)(generator);

    auto lifetime = std::uniform_real_distribution(1.0f, 2.0f)(generator);

    particle_system->emit(glm::vec3{0, 0, 0}, particle_velocity, lifetime);

    auto move = InputSystem::get_instance()->get_axis("move");
    auto strafe = InputSystem::get_instance()->get_axis("strafe");

    auto camera = get_default_camera();
    auto position = camera->get_position();
    auto rotation = camera->get_rotation();

    bool flag = false;
    if (move != 0.0f || strafe != 0.0f) {
        flag = true;

        auto speed = 5.f;
        if (InputSystem::get_instance()->get_button("sprint")) {
            speed = 50.f;
        }
        auto orientation = camera->get_orientation();
        auto direction = glm::normalize(glm::vec3(strafe, 0.0f, move));
        auto velocity = glm::mat3(orientation) * direction * speed;

        position += velocity * event.dt;
    }

    auto delta = InputSystem::get_instance()->get_mouse_delta();
    if (delta != glm::vec2(0, 0)) {
        flag = true;

        const auto d4 = 0.5f * 0.6F + 0.2F;
        const auto d5 = d4 * d4 * d4 * 8.0f;

        rotation.y += delta.x * d5 * event.dt * 9.0f;
        rotation.x += delta.y * d5 * event.dt * 9.0f;
        rotation.x = glm::clamp(rotation.x, -90.0f, 90.0f);
    }

    if (flag) {
        camera->set_transform(position, rotation);
    }
}

void ParticleSystemPlugin::draw(const DrawEvent &event) {
    particle_system->draw(event);
}

auto main(int argc, char** argv) -> int {
    Application application("Particle System", 800, 600);
    ParticleSystemPlugin particle_system_plugin{};
    application.run();
    return EXIT_SUCCESS;
}