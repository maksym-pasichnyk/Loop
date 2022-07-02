#pragma once

#include <vulkan/vulkan.hpp>

namespace LoopEngine::Vulkan {
    extern auto get_format_from_string(const std::string& str) -> vk::Format;
    extern auto get_blend_op_from_string(const std::string& str) -> vk::BlendOp;
    extern auto get_blend_factor_from_string(const std::string& str) -> vk::BlendFactor;
}