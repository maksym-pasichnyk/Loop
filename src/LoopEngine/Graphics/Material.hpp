#pragma once

#include <string>
#include <vulkan/vulkan.hpp>

namespace LoopEngine::Graphics {
    struct Material {
        vk::Pipeline pipeline;
        vk::PipelineLayout pipeline_layout;
    };

    extern auto get_module_from_assets(const std::string& path) -> vk::ShaderModule;
    extern auto get_material_from_assets(const std::string& filename) -> std::shared_ptr<Material>;
}