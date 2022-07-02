#pragma once

#include <string>
#include <vulkan/vulkan.hpp>

namespace LoopEngine::Graphics {
    struct Context;
    struct Material {
        vk::PipelineBindPoint bind_point = vk::PipelineBindPoint::eGraphics;

        vk::Pipeline pipeline;
        vk::PipelineLayout pipeline_layout;
    };

    extern auto get_module_from_assets(const std::string& filename) -> vk::ShaderModule;
    extern auto get_material_from_assets(const std::string& filename) -> std::shared_ptr<Material>;

    extern void release_material(const Material& material);
}