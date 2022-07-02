#pragma once

#include "vk_mem_alloc.h"
#include <vulkan/vulkan.hpp>

#include <memory>

namespace LoopEngine::Graphics {
    struct Context;
    struct IndexBuffer {
        vk::Buffer handle{};
        VmaAllocation allocation{};
    };

    extern auto create_index_buffer(vk::DeviceSize size) -> std::shared_ptr<IndexBuffer>;
    extern void release_index_buffer(const IndexBuffer& buffer);

    extern auto map_index_buffer(const IndexBuffer &buffer) -> void*;
    extern void unmap_index_buffer(const IndexBuffer &buffer);
    extern void update_index_buffer(const IndexBuffer &buffer, const void *data, vk::DeviceSize size);
}

