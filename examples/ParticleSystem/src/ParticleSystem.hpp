#pragma once

#include <vector>
#include <memory>

#include "glm/glm.hpp"

#include "LoopEngine/Event/EventSystem.hpp"
#include "LoopEngine/Graphics/Material.hpp"
#include "LoopEngine/Graphics/Context.hpp"
#include "LoopEngine/Graphics/IndexBuffer.hpp"
#include "LoopEngine/Graphics/VertexBuffer.hpp"

using LoopEngine::Event::InitEvent;
using LoopEngine::Event::DrawEvent;
using LoopEngine::Event::QuitEvent;
using LoopEngine::Event::UpdateEvent;

using LoopEngine::Graphics::Material;
using LoopEngine::Graphics::IndexBuffer;
using LoopEngine::Graphics::VertexBuffer;

struct Particle {
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec4 color;
    float time;
    float lifetime;
};

struct ParticleSystemUpdateEvent {
    float dt;
};

struct ParticleSpawnEvent {
    Particle& particle;
};

struct ParticleDeathEvent {
    Particle& particle;
};

struct ParticleSystem {
    explicit ParticleSystem(size_t capacity);
    ~ParticleSystem();

    void emit(const glm::vec3& position, const glm::vec4 &color, const glm::vec3 &velocity, float lifetime);
    void update(float dt);
    void draw(vk::CommandBuffer cmd);

    [[nodiscard]] auto get_particles() -> std::span<Particle> {
        return particles;
    }

    [[nodiscard]] auto get_capacity() const -> size_t {
        return particles.size();
    }

    [[nodiscard]] auto get_particles_count() const -> size_t {
        return count;
    }

    template<typename T>
    void add_event_handler(LoopEngine::Event::EventHandler<T>* handler) {
        event_queue.add_event_handler(handler);
    }

    template<typename T>
    void remove_event_handler(LoopEngine::Event::EventHandler<T>* handler) {
        event_queue.remove_event_handler(handler);
    }

private:
    struct VertexData {
        alignas(16) glm::vec3 position;
        alignas(16) glm::vec4 color;
    };
    std::vector<Particle> particles{};
    std::vector<VertexData> positions{};
    size_t count = 0;

    std::shared_ptr<IndexBuffer> ibo{};
    std::shared_ptr<VertexBuffer> vbo[2]{};

    std::shared_ptr<Material> material{};
    LoopEngine::Event::EventQueue event_queue{};
};