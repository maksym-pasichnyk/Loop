#pragma once

#include "vk_mem_alloc.h"
#include <vulkan/vulkan.hpp>

#include <memory>

namespace LoopEngine::Graphics {
    struct Context;
    struct UniformBuffer {
        vk::Buffer handle{};
        VmaAllocation allocation{};
    };

    extern auto create_uniform_buffer(vk::DeviceSize size) -> std::shared_ptr<UniformBuffer>;
    extern void release_uniform_buffer(const UniformBuffer& buffer);

    extern auto map_uniform_buffer(const UniformBuffer &buffer) -> void*;
    extern void unmap_uniform_buffer(const UniformBuffer &buffer);
    extern void update_uniform_buffer(const UniformBuffer &buffer, const void *data, vk::DeviceSize size);
}