#pragma once

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>

#include <memory>

namespace LoopEngine::Graphics {
    struct VertexBuffer {
        vk::Buffer handle{};
        VmaAllocation allocation{};
    };

    extern auto create_vertex_buffer(vk::DeviceSize size) -> std::shared_ptr<VertexBuffer>;
    extern void release_vertex_buffer(const VertexBuffer& buffer);

    extern auto map_vertex_buffer(const VertexBuffer &buffer) -> void*;
    extern void unmap_vertex_buffer(const VertexBuffer &buffer);
    extern void update_vertex_buffer(const VertexBuffer &buffer, const void *data, vk::DeviceSize size);
}