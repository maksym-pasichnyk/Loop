#include "LoopEngine/Event/EventSystem.hpp"
#include "LoopEngine/Camera/CameraSystem.hpp"
#include "LoopEngine/Graphics/Material.hpp"
#include "LoopEngine/Graphics/Context.hpp"
#include "LoopEngine/Graphics/Graphics.hpp"
#include "spdlog/spdlog.h"
#include "glm/vec3.hpp"

#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.h>
#include <random>

using LoopEngine::Event::InitEvent;
using LoopEngine::Event::DrawEvent;
using LoopEngine::Event::QuitEvent;
using LoopEngine::Event::UpdateEvent;
using LoopEngine::Event::EventHandler;
using LoopEngine::Graphics::Graphics;
using LoopEngine::Graphics::Material;
using LoopEngine::Graphics::context;
using LoopEngine::Graphics::get_material_from_assets;
using LoopEngine::Camera::get_default_camera;

struct VertexBuffer {
    vk::Buffer handle{};
    VmaAllocation allocation{};
    VmaAllocationInfo info{};

    explicit VertexBuffer(vk::DeviceSize size) {
        vk::BufferUsageFlags usage;
        usage |= vk::BufferUsageFlagBits::eVertexBuffer;
        vk::BufferCreateInfo buffer_info{};
        buffer_info.setSize(size);
        buffer_info.setUsage(usage);

        VmaAllocationCreateInfo alloc_info{};
        alloc_info.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

        vmaCreateBuffer(context().allocator, reinterpret_cast<const VkBufferCreateInfo *>(&buffer_info), &alloc_info, reinterpret_cast<VkBuffer *>(&handle), &allocation, &info);
    }

    ~VertexBuffer() {
        vmaDestroyBuffer(context().allocator, handle, allocation);
    }

    [[nodiscard]] auto map() const -> void* {
        void* data;
        vmaMapMemory(context().allocator, allocation, &data);
        return data;
    }

    void unmap() const {
        vmaUnmapMemory(context().allocator, allocation);
    }

    void update(const void* data, size_t size) const {
        std::memcpy(map(), data, size);
        unmap();
    }
};

struct IndexBuffer {
    vk::Buffer handle{};
    VmaAllocation allocation{};
    VmaAllocationInfo info{};

    explicit IndexBuffer(vk::DeviceSize size) {
        vk::BufferUsageFlags usage;
        usage |= vk::BufferUsageFlagBits::eIndexBuffer;
        vk::BufferCreateInfo buffer_info{};
        buffer_info.setSize(size);
        buffer_info.setUsage(usage);

        VmaAllocationCreateInfo alloc_info{};
        alloc_info.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

        vmaCreateBuffer(context().allocator, reinterpret_cast<const VkBufferCreateInfo *>(&buffer_info), &alloc_info, reinterpret_cast<VkBuffer *>(&handle), &allocation, &info);
    }

    ~IndexBuffer() {
        vmaDestroyBuffer(context().allocator, handle, allocation);
    }

    [[nodiscard]] auto map() const -> void* {
        void* data;
        vmaMapMemory(context().allocator, allocation, &data);
        return data;
    }

    void unmap() const {
        vmaUnmapMemory(context().allocator, allocation);
    }

    void update(const void* data, size_t size) const {
        std::memcpy(map(), data, size);
        unmap();
    }
};

std::shared_ptr<IndexBuffer> ibo;
std::shared_ptr<VertexBuffer> vbo[2];
std::shared_ptr<Material> material;

//todo: register event handlers

template<>
void EventHandler<InitEvent>::handle(const Event& event) {
    spdlog::info("ParticleSystem::init()");

    std::vector<float> vertices = {
        -0.05f, -0.05f, 0.0f,
        0.05f, -0.05f, 0.0f,
        0.0f,  0.05f, 0.0f
    };

    vbo[0] = std::make_shared<VertexBuffer>(sizeof(float) * 6);
    vbo[0]->update(vertices.data(), sizeof(float) * 6);

    std::vector<uint32_t> indices = {
        0, 1, 2
    };

    ibo = std::make_shared<IndexBuffer>(sizeof(uint32_t) * 3);
    ibo->update(indices.data(), sizeof(uint32_t) * 3);

    std::default_random_engine generator{};
    std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);

    std::vector<glm::vec3> positions(1000);
    for (auto& particle : positions) {
        particle.x = distribution(generator);
        particle.y = distribution(generator);
        particle.z = distribution(generator);
    }
    vbo[1] = std::make_shared<VertexBuffer>(sizeof(glm::vec3) * 1000);
    vbo[1]->update(positions.data(), sizeof(glm::vec3) * 1000);

    material = get_material_from_assets("assets/materials/particles.yaml");
}

template<>
void EventHandler<UpdateEvent>::handle(const Event& event) {
//    spdlog::info("ParticleSystem::update({})", event.deltaTime);

    auto camera = get_default_camera();
}

template<>
void EventHandler<DrawEvent>::handle(const Event& event) {
//    spdlog::info("ParticleSystem::draw()");

    auto cmd = ::Graphics::get_instance()->get_current_frame_command_buffer();

    cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, material->pipeline);
    cmd.bindVertexBuffers(0, {vbo[0]->handle, vbo[1]->handle}, {0, 0});
    cmd.bindIndexBuffer(ibo->handle, 0, vk::IndexType::eUint32);
    cmd.drawIndexed(3, 1000, 0, 0, 0);
}

template<>
void EventHandler<QuitEvent>::handle(const Event& event) {
    spdlog::info("ParticleSystem::quit()");

    vbo[0].reset();
    vbo[1].reset();
    ibo.reset();

    context().device.destroyPipelineLayout(material->pipeline_layout);
    context().device.destroyPipeline(material->pipeline);
    material.reset();
}