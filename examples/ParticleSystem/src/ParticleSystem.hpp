#pragma once

#include <vector>
#include <memory>

#include "glm/glm.hpp"

#include "LoopEngine/Event/EventSystem.hpp"
#include "LoopEngine/Graphics/Material.hpp"
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
    float lifetime;
};

struct ParticleSystem {
    explicit ParticleSystem(size_t capacity);
    ~ParticleSystem();

    void emit(const glm::vec3& position, const glm::vec3 &velocity, float lifetime);
    void update(const UpdateEvent& event);
    void draw(const DrawEvent& event);

private:
    std::vector<Particle> particles{};
    std::vector<glm::vec3> positions{};
    size_t count = 0;

    std::shared_ptr<IndexBuffer> ibo{};
    std::shared_ptr<VertexBuffer> vbo[2]{};

    std::shared_ptr<Material> material{};
};