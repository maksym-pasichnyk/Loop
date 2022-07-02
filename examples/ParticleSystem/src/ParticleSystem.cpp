#include "ParticleSystem.hpp"

#include "LoopEngine/Graphics/Material.hpp"
#include "LoopEngine/Graphics/Graphics.hpp"
#include "LoopEngine/Graphics/IndexBuffer.hpp"
#include "LoopEngine/Graphics/VertexBuffer.hpp"
#include "spdlog/spdlog.h"
#include "glm/vec3.hpp"

using LoopEngine::Graphics::create_index_buffer;
using LoopEngine::Graphics::update_index_buffer;
using LoopEngine::Graphics::release_index_buffer;
using LoopEngine::Graphics::create_vertex_buffer;
using LoopEngine::Graphics::update_vertex_buffer;
using LoopEngine::Graphics::release_vertex_buffer;
using LoopEngine::Graphics::get_material_from_assets;
using LoopEngine::Graphics::bind_global_descriptor_sets;

ParticleSystem::ParticleSystem(size_t capacity)  {
    positions.resize(capacity);
    particles.resize(capacity);

    std::vector<uint32_t> indices = { 0, 1, 2, 0, 2, 3 };

    ibo = create_index_buffer(sizeof(uint32_t) * indices.size());
    update_index_buffer(*ibo, indices.data(), sizeof(uint32_t) * indices.size());

    std::vector<float> vertices{
        -0.05f, -0.05f, 0.0f,
        -0.05f,  0.05f, 0.0f,
        0.05f,  0.05f, 0.0f,
        0.05f, -0.05f, 0.0f
    };

    vbo[0] = create_vertex_buffer(sizeof(float) * vertices.size());
    vbo[1] = create_vertex_buffer(sizeof(VertexData) * positions.size());

    update_vertex_buffer(*vbo[0], vertices.data(), sizeof(float) * vertices.size());

    material = get_material_from_assets("materials/particles.material");
}

ParticleSystem::~ParticleSystem() {
    release_index_buffer(*ibo);
    release_vertex_buffer(*vbo[0]);
    release_vertex_buffer(*vbo[1]);
    release_material(*material);
}

void ParticleSystem::emit(const glm::vec3 &position, const glm::vec4 &color, const glm::vec3 &velocity, float lifetime) {
    // find an empty slot
    int index = -1;
    for (int i = 0; i < particles.size(); i++) {
        if (particles[i].lifetime <= 0.0f) {
            index = i;
            break;
        }
    }

    if (index != -1) {
        particles[index].position = position;
        particles[index].velocity = velocity;
        particles[index].color = color;
        particles[index].lifetime = lifetime;
        particles[index].time = lifetime;
    }
}

void ParticleSystem::update(float dt) {
    event_queue.send_event(ParticleSystemUpdateEvent{dt });

    count = 0;
    for (auto& particle : particles) {
        if (particle.lifetime <= 0.0f) {
            continue;
        }
        particle.lifetime -= dt;
        particle.position += particle.velocity * dt;
        positions[count].position = particle.position;
        positions[count].color = particle.color;
        count++;
        if (particle.lifetime <= 0.0f) {
            event_queue.send_event(ParticleDeathEvent{particle });
        }
    }
    if (count > 0) {
        update_vertex_buffer(*vbo[1], positions.data(), sizeof(VertexData) * count);
    }
}

void ParticleSystem::draw(vk::CommandBuffer cmd) {
    if (count == 0) {
        return;
    }
    cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, material->pipeline);
    bind_global_descriptor_sets(cmd, *material, 0);
    cmd.bindVertexBuffers(0, {vbo[0]->handle, vbo[1]->handle}, {0, 0});
    cmd.bindIndexBuffer(ibo->handle, 0, vk::IndexType::eUint32);
    cmd.drawIndexed(6, count, 0, 0, 0);
}