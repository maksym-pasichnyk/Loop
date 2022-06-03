#include "LoopEngine/Event/EventSystem.hpp"
#include "LoopEngine/Camera/CameraSystem.hpp"
#include "LoopEngine/Graphics/Material.hpp"
#include "LoopEngine/Graphics/Context.hpp"
#include "LoopEngine/Graphics/Graphics.hpp"
#include "LoopEngine/Graphics/IndexBuffer.hpp"
#include "LoopEngine/Graphics/VertexBuffer.hpp"
#include "LoopEngine/Input/InputSystem.hpp"
#include "LoopEngine/Input/InputController.hpp"
#include "LoopEngine/Platform/Display.hpp"
#include "LoopEngine/Application.hpp"
#include "spdlog/spdlog.h"
#include "glm/vec3.hpp"

#include <random>

using LoopEngine::Event::InitEvent;
using LoopEngine::Event::DrawEvent;
using LoopEngine::Event::QuitEvent;
using LoopEngine::Event::UpdateEvent;
using LoopEngine::Event::EventSystem;
using LoopEngine::Event::EventHandler;
using LoopEngine::Event::create_delegate;
using LoopEngine::Graphics::Graphics;
using LoopEngine::Graphics::Material;
using LoopEngine::Graphics::IndexBuffer;
using LoopEngine::Graphics::VertexBuffer;
using LoopEngine::Graphics::create_index_buffer;
using LoopEngine::Graphics::update_index_buffer;
using LoopEngine::Graphics::release_index_buffer;
using LoopEngine::Graphics::create_vertex_buffer;
using LoopEngine::Graphics::update_vertex_buffer;
using LoopEngine::Graphics::release_vertex_buffer;
using LoopEngine::Graphics::context;
using LoopEngine::Graphics::get_material_from_assets;
using LoopEngine::Graphics::bind_global_descriptor_sets;
using LoopEngine::Camera::get_default_camera;
using LoopEngine::Input::InputSystem;
using LoopEngine::Input::InputController;
using LoopEngine::Platform::Display;
using LoopEngine::Application;

struct Particle {
    glm::vec3 position;
    float lifetime;
};

struct ParticleSystem {
    float totalTime = 0.0f;

    std::vector<Particle> particles{1000};
    std::vector<glm::vec3> positions{1000};
    std::default_random_engine generator{};

    std::shared_ptr<IndexBuffer> ibo{};
    std::shared_ptr<VertexBuffer> vbo[2]{};
    std::shared_ptr<Material> material{};

    EventHandler<InitEvent> init_event_handler;
    EventHandler<DrawEvent> draw_event_handler;
    EventHandler<QuitEvent> quit_event_handler;
    EventHandler<UpdateEvent> update_event_handler;

    ParticleSystem()
    : init_event_handler{create_delegate<&ParticleSystem::init>(this)}
    , draw_event_handler{create_delegate<&ParticleSystem::draw>(this)}
    , quit_event_handler{create_delegate<&ParticleSystem::quit>(this)}
    , update_event_handler{create_delegate<&ParticleSystem::update>(this)} {
        EventSystem::get_instance()->add_event_handler(&init_event_handler);
        EventSystem::get_instance()->add_event_handler(&draw_event_handler);
        EventSystem::get_instance()->add_event_handler(&quit_event_handler);
        EventSystem::get_instance()->add_event_handler(&update_event_handler);
    }

    ~ParticleSystem() {
        EventSystem::get_instance()->remove_event_handler(&init_event_handler);
        EventSystem::get_instance()->remove_event_handler(&draw_event_handler);
        EventSystem::get_instance()->remove_event_handler(&quit_event_handler);
        EventSystem::get_instance()->remove_event_handler(&update_event_handler);
    }

    void init(const InitEvent& event);
    void update(const UpdateEvent& event);
    void draw(const DrawEvent& event);
    void quit(const QuitEvent& event);
};

void ParticleSystem::init(const InitEvent& event) {
    spdlog::info("ParticleSystem::init()");

    std::vector<float> vertices = {
        -0.05f, -0.05f, 0.0f,
        -0.05f,  0.05f, 0.0f,
         0.05f,  0.05f, 0.0f,
         0.05f, -0.05f, 0.0f
    };

    vbo[0] = create_vertex_buffer(sizeof(float) * vertices.size());
    update_vertex_buffer(*vbo[0], vertices.data(), sizeof(float) * vertices.size());

    std::vector<uint32_t> indices = {
        0, 1, 2,
        0, 2, 3
    };

    ibo = create_index_buffer(sizeof(uint32_t) * indices.size());
    update_index_buffer(*ibo, indices.data(), sizeof(uint32_t) * indices.size());

    for (auto& particle : particles) {
        particle.position.x = std::uniform_real_distribution(-1.0f, 1.0f)(generator);
        particle.position.y = std::uniform_real_distribution(-1.0f, 1.0f)(generator);
        particle.position.z = std::uniform_real_distribution(-1.0f, 1.0f)(generator);
        particle.position = glm::normalize(particle.position) * 5.0f;
        particle.lifetime = std::uniform_real_distribution(1.0f, 2.0f)(generator);
    }
    vbo[1] = create_vertex_buffer(sizeof(glm::vec3) * positions.size());

    material = get_material_from_assets("materials/particles.yaml");

    auto camera = get_default_camera();
    camera->set_perspective(60.0f, 16.0f / 12.0f, 0.1f, 1000.0f);
    camera->set_transform(glm::vec3(0.0f, 0.0f, -10.0f), glm::vec3(0.0f, 0.0f, 0.0f));
}

void ParticleSystem::update(const UpdateEvent& event) {
    glfwSetInputMode(Display::get_native_window_handle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    totalTime += event.dt;

    for (int i = 0; i < 1000; i++) {
        particles[i].lifetime -= event.dt;

        if (particles[i].lifetime < 0.0f) {
            particles[i].position.x = std::uniform_real_distribution(-1.0f, 1.0f)(generator);
            particles[i].position.y = std::uniform_real_distribution(-1.0f, 1.0f)(generator);
            particles[i].position.z = std::uniform_real_distribution(-1.0f, 1.0f)(generator);
            particles[i].position = glm::normalize(particles[i].position) * 5.0f;
            particles[i].lifetime = std::uniform_real_distribution(1.0f, 2.0f)(generator);
        }
    }
    for (int i = 0; i < 1000; i++) {
        positions[i] = particles[i].position;
    }
    update_vertex_buffer(*vbo[1], positions.data(), sizeof(glm::vec3) * positions.size());

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

void ParticleSystem::draw(const DrawEvent& event) {
    event.cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, material->pipeline);
    bind_global_descriptor_sets(event.cmd, *material, 0);
    event.cmd.bindVertexBuffers(0, {vbo[0]->handle, vbo[1]->handle}, {0, 0});
    event.cmd.bindIndexBuffer(ibo->handle, 0, vk::IndexType::eUint32);
    event.cmd.drawIndexed(6, 1000, 0, 0, 0);
}

void ParticleSystem::quit(const QuitEvent& event) {
    spdlog::info("ParticleSystem::quit()");

    release_vertex_buffer(*vbo[0]);
    release_vertex_buffer(*vbo[1]);
    release_index_buffer(*ibo);
    release_material(*material);
}

auto main(int argc, char** argv) -> int {
    Application application("Particle System", 800, 600);
    ParticleSystem particle_system{};
    application.run();
    return EXIT_SUCCESS;
}