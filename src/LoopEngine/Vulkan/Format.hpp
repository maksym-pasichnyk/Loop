#pragma once

#include <vulkan/vulkan.hpp>

namespace LoopEngine::Vulkan {
    extern auto get_format_from_string(const std::string& format) -> vk::Format;
}