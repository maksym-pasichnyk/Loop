#pragma once

#include "vk_mem_alloc.h"
#include <vulkan/vulkan.hpp>

namespace LoopEngine::Graphics {
    struct Texture {
        vk::Image image{};
        vk::Sampler sampler{};
        vk::ImageView image_view{};
        VmaAllocation allocation{};
    };
    extern auto get_texture_from_assets(const std::string& filename) -> std::shared_ptr<Texture>;
    static void release_texture(const Texture& texture);
}