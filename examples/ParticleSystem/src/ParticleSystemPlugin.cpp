#include "ParticleSystemPlugin.hpp"
#include "ParticleSystem.hpp"
#include "ImGuiPlugin.hpp"

#include "LoopEngine/Camera/CameraSystem.hpp"
#include "LoopEngine/Input/InputSystem.hpp"
#include "LoopEngine/Input/InputController.hpp"
#include "LoopEngine/Platform/Display.hpp"
#include "LoopEngine/Application.hpp"
#include "spdlog/spdlog.h"
#include "glm/vec3.hpp"
#include "imgui.h"

using LoopEngine::Application;
using LoopEngine::Platform::Display;
using LoopEngine::Input::InputSystem;
using LoopEngine::Event::EventSystem;
using LoopEngine::Event::get_global_event_queue;
using LoopEngine::Graphics::check;
using LoopEngine::Graphics::context;
using LoopEngine::Graphics::Graphics;
using LoopEngine::Camera::get_default_camera;

struct FireworkParticleSystem {
    FireworkParticleSystem() {
        rocket_particle_system = std::make_shared<ParticleSystem>(1000);
        rocket_particle_system->add_event_handler(&rocket_particle_death_handler);
        rocket_particle_system->add_event_handler(&rocket_particle_system_update_handler);
        rocket_particle_death_handler.connect<&FireworkParticleSystem::on_rocket_particle_death>(this);
        rocket_particle_system_update_handler.connect<&FireworkParticleSystem::on_rocket_particle_system_update>(this);

        sparkle_particle_system = std::make_shared<ParticleSystem>(1000);
        sparkle_particle_system->add_event_handler(&sparkle_particle_system_update_handler);
        sparkle_particle_system_update_handler.connect<&FireworkParticleSystem::on_sparkle_particle_system_update>(this);

        explosion_particle_system = std::make_shared<ParticleSystem>(1000);
        explosion_particle_system->add_event_handler(&explosion_particle_system_update_handler);
        explosion_particle_system_update_handler.connect<&FireworkParticleSystem::on_explosion_particle_system_update>(this);
    }

    ~FireworkParticleSystem() {
        rocket_particle_system->remove_event_handler(&rocket_particle_system_update_handler);
        rocket_particle_system->remove_event_handler(&rocket_particle_death_handler);
        sparkle_particle_system->remove_event_handler(&sparkle_particle_system_update_handler);
        explosion_particle_system->remove_event_handler(&explosion_particle_system_update_handler);
    }

    void update(const UpdateEvent& event) {
        rocket_particle_system->update(event);
        sparkle_particle_system->update(event);
        explosion_particle_system->update(event);
    }

    void draw(const DrawEvent& event) {
        rocket_particle_system->draw(event);
        sparkle_particle_system->draw(event);
        explosion_particle_system->draw(event);
    }

private:
    void on_rocket_particle_system_update(const ParticleSystemUpdateEvent& event) {
        emit_delay += event.dt;
        while (emit_delay >= emit_rate) {
            emit_delay -= emit_rate;

            glm::vec3 direction{};
            direction.x = std::uniform_real_distribution(-1.0f, 1.0f)(generator);
            direction.y = 2.0f;//std::uniform_real_distribution(-1.0f, 1.0f)(generator);
            direction.z = std::uniform_real_distribution(-1.0f, 1.0f)(generator);

            glm::vec4 color{};
            color.x = std::uniform_real_distribution(0.0f, 1.0f)(generator);
            color.y = std::uniform_real_distribution(0.0f, 1.0f)(generator);
            color.z = std::uniform_real_distribution(0.0f, 1.0f)(generator);
            color.w = 1.0f;

            auto lifetime = std::uniform_real_distribution(1.0f, 2.0f)(generator);

            rocket_particle_system->emit(glm::vec3{}, color, direction * 5.0f, lifetime);
        }

        for (auto& particle : rocket_particle_system->get_particles()) {
            if (particle.lifetime <= 0.0f) {
                continue;
            }
            particle.color.w = std::clamp(particle.lifetime / particle.time, 0.0f, 1.0f);

            glm::vec3 direction{};
            direction.x = std::uniform_real_distribution(-1.0f, 1.0f)(generator) * 0.5f;
            direction.y = 0.0f;
            direction.z = std::uniform_real_distribution(-1.0f, 1.0f)(generator) * 0.5f;

            auto lifetime = std::uniform_real_distribution(0.0f, 1.0f)(generator);

            sparkle_particle_system->emit(particle.position, particle.color, direction, lifetime);
        }
    }

    void on_sparkle_particle_system_update(const ParticleSystemUpdateEvent &event) {
        for (auto& particle : sparkle_particle_system->get_particles()) {
            if (particle.lifetime <= 0.0f) {
                continue;
            }
            particle.color.w = std::clamp(particle.lifetime / particle.time, 0.0f, 1.0f);
            particle.velocity.y -= 9.8f * event.dt;
            particle.velocity.y = std::max(particle.velocity.y, -1.0f);
        }
    }

    void on_explosion_particle_system_update(const ParticleSystemUpdateEvent &event) {
        for (auto& particle : explosion_particle_system->get_particles()) {
            if (particle.lifetime <= 0.0f) {
                continue;
            }
            particle.color.w = std::clamp(particle.lifetime / particle.time, 0.0f, 1.0f);
        }
    }

    void on_rocket_particle_death(const ParticleDeathEvent& event) {
        for (int i = 0; i < 250; ++i) {
            glm::vec3 velocity{};
            velocity.x = std::uniform_real_distribution(-1.0f, 1.0f)(generator) * 5.f;
            velocity.y = std::uniform_real_distribution(-1.0f, 1.0f)(generator) * 5.f;
            velocity.z = std::uniform_real_distribution(-1.0f, 1.0f)(generator) * 5.f;

            glm::vec4 color = event.particle.color;
//        color.x = std::uniform_real_distribution(0.0f, 1.0f)(generator);
//        color.y = std::uniform_real_distribution(0.0f, 1.0f)(generator);
//        color.z = std::uniform_real_distribution(0.0f, 1.0f)(generator);
            color.w = 1.0f;

            explosion_particle_system->emit(event.particle.position, color, velocity, 1.0f);
        }
    }

private:
    float emit_rate = 2.5f;
    float emit_delay = 0.0f;

    std::default_random_engine generator{};
    std::shared_ptr<ParticleSystem> rocket_particle_system{};
    std::shared_ptr<ParticleSystem> sparkle_particle_system{};
    std::shared_ptr<ParticleSystem> explosion_particle_system{};
    EventHandler<ParticleSystemUpdateEvent> rocket_particle_system_update_handler{};
    EventHandler<ParticleSystemUpdateEvent> sparkle_particle_system_update_handler{};
    EventHandler<ParticleSystemUpdateEvent> explosion_particle_system_update_handler{};
    EventHandler<ParticleDeathEvent> rocket_particle_death_handler{};
};

ParticleSystemPlugin::ParticleSystemPlugin() {
    init_event_handler.connect<&ParticleSystemPlugin::init>(this);
    draw_event_handler.connect<&ParticleSystemPlugin::draw>(this);
    update_event_handler.connect<&ParticleSystemPlugin::update>(this);
    imgui_draw_event_handler.connect<&ParticleSystemPlugin::on_imgui_draw>(this);
    press_button_event_handler.connect<&ParticleSystemPlugin::on_press_button>(this);

    get_global_event_queue()->add_event_handler(&init_event_handler);
    get_global_event_queue()->add_event_handler(&draw_event_handler);
    get_global_event_queue()->add_event_handler(&update_event_handler);
    get_global_event_queue()->add_event_handler(&imgui_draw_event_handler);
    get_global_event_queue()->add_event_handler(&press_button_event_handler);

    firework_particle_system = std::make_shared<FireworkParticleSystem>();
}

ParticleSystemPlugin::~ParticleSystemPlugin() {
    get_global_event_queue()->remove_event_handler(&init_event_handler);
    get_global_event_queue()->remove_event_handler(&draw_event_handler);
    get_global_event_queue()->remove_event_handler(&update_event_handler);
    get_global_event_queue()->remove_event_handler(&imgui_draw_event_handler);
    get_global_event_queue()->remove_event_handler(&press_button_event_handler);
}

void ParticleSystemPlugin::init(const InitEvent &event) {
    auto camera = get_default_camera();
    camera->set_perspective(60.0f, 16.0f / 12.0f, 0.1f, 1000.0f);
    camera->set_transform(glm::vec3(0.0f, 0.0f, -50.0f), glm::vec3(0.0f, 0.0f, 0.0f));
}

void ParticleSystemPlugin::update(const UpdateEvent &event) {
    update_camera(event);

    firework_particle_system->update(event);
}

void ParticleSystemPlugin::draw(const DrawEvent &event) {
    firework_particle_system->draw(event);
}

void ParticleSystemPlugin::update_camera(const UpdateEvent &event) const {
    if (lock_mouse) {
        glfwSetInputMode(Display::get_native_window_handle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } else {
        glfwSetInputMode(Display::get_native_window_handle(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        return;
    }

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

void ParticleSystemPlugin::on_press_button(const ButtonPressEvent& event) {
    if (event.button == "camera") {
        lock_mouse = !lock_mouse;
    }
}

void ParticleSystemPlugin::on_imgui_draw(const ImGuiDrawEvent& event) {
    ImGui::Begin("Particle System", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();
}

auto main(int argc, char** argv) -> int {
    Application application("Particle System", 800, 600);
    application.add_plugin<ImGuiPlugin>();
    application.add_plugin<ParticleSystemPlugin>();
    application.run();
    return EXIT_SUCCESS;
}