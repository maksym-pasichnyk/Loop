#include "Context.hpp"
#include "Graphics.hpp"
#include "Material.hpp"
#include "glm/vec3.hpp"
#include "spdlog/spdlog.h"
#include "yaml-cpp/yaml.h"

#include <fstream>

auto LoopEngine::Graphics::get_module_from_assets(const std::string &path) -> vk::ShaderModule {
    std::ifstream file(path, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error(fmt::format("Failed to open file: {}", path));
    }

    auto file_size = size_t(file.tellg());
    std::vector<char> buffer(file_size);

    file.seekg(0);
    file.read(buffer.data(), std::streamsize(file_size));
    file.close();

    vk::ShaderModuleCreateInfo create_info{};
    create_info.setCodeSize(file_size);
    create_info.setPCode(reinterpret_cast<uint32_t *>(buffer.data()));

    return context().device.createShaderModule(create_info);
}

auto LoopEngine::Graphics::get_material_from_assets(const std::string &filename) -> std::shared_ptr<Material> {
    std::ifstream file(filename);
    if (!file.is_open()) {
        spdlog::error("Failed to open file {}", filename);
        return nullptr;
    }

    YAML::Node config = YAML::Load(file);
    if (!config.IsMap()) {
        spdlog::error("Failed to parse file {}", filename);
        return nullptr;
    }

    auto vs = get_module_from_assets(config["vert"].as<std::string>());
    auto fs = get_module_from_assets(config["frag"].as<std::string>());

    vk::PipelineShaderStageCreateInfo vertex_shader_stage_create_info{};
    vertex_shader_stage_create_info.setStage(vk::ShaderStageFlagBits::eVertex);
    vertex_shader_stage_create_info.setModule(vs);
    vertex_shader_stage_create_info.setPName("main");

    vk::PipelineShaderStageCreateInfo fragment_shader_stage_create_info{};
    fragment_shader_stage_create_info.setStage(vk::ShaderStageFlagBits::eFragment);
    fragment_shader_stage_create_info.setModule(fs);
    fragment_shader_stage_create_info.setPName("main");

    vk::PipelineShaderStageCreateInfo shader_stages[] = {
        vertex_shader_stage_create_info,
        fragment_shader_stage_create_info
    };

    std::vector<vk::VertexInputBindingDescription> bindings{};
    std::vector<vk::VertexInputAttributeDescription> attributes{};

    for (auto&& node : config["bindings"]) {
        vk::VertexInputBindingDescription description{};
        description.setBinding(node["binding"].as<int>());
        description.setStride(node["stride"].as<int>());
        if (node["instanced"].as<bool>()) {
            description.setInputRate(vk::VertexInputRate::eInstance);
        } else {
            description.setInputRate(vk::VertexInputRate::eVertex);
        }
        bindings.emplace_back(description);
    }

    for (auto&& node : config["attributes"]) {
        vk::VertexInputAttributeDescription description{};
        description.setLocation(node["location"].as<int>());
        description.setBinding(node["binding"].as<int>());
        description.setFormat(vk::Format::eR32G32B32Sfloat);
        description.setOffset(node["offset"].as<int>());
        attributes.emplace_back(description);
    }

    vk::PipelineVertexInputStateCreateInfo vertex_input_create_info{};
    vertex_input_create_info.setVertexBindingDescriptions(bindings);
    vertex_input_create_info.setVertexAttributeDescriptions(attributes);

    vk::PipelineInputAssemblyStateCreateInfo input_assembly_create_info{};
    input_assembly_create_info.setTopology(vk::PrimitiveTopology::eTriangleList);
    input_assembly_create_info.setPrimitiveRestartEnable(false);

    vk::PipelineViewportStateCreateInfo viewport_state_create_info{};
    viewport_state_create_info.setViewportCount(1);
    viewport_state_create_info.setPViewports(nullptr);
    viewport_state_create_info.setScissorCount(1);
    viewport_state_create_info.setPScissors(nullptr);

    vk::PipelineRasterizationStateCreateInfo rasterization_state_create_info{};
    rasterization_state_create_info.setDepthClampEnable(false);
    rasterization_state_create_info.setRasterizerDiscardEnable(false);
    rasterization_state_create_info.setPolygonMode(vk::PolygonMode::eFill);
    rasterization_state_create_info.setLineWidth(1.0f);
    rasterization_state_create_info.setCullMode(vk::CullModeFlagBits::eNone);
    rasterization_state_create_info.setFrontFace(vk::FrontFace::eCounterClockwise);
    rasterization_state_create_info.setDepthBiasEnable(false);

    vk::PipelineMultisampleStateCreateInfo multisample_state_create_info{};
    multisample_state_create_info.setRasterizationSamples(vk::SampleCountFlagBits::e1);
    multisample_state_create_info.setSampleShadingEnable(false);
    multisample_state_create_info.setMinSampleShading(1.0f);
    multisample_state_create_info.setPSampleMask(nullptr);
    multisample_state_create_info.setAlphaToCoverageEnable(false);
    multisample_state_create_info.setAlphaToOneEnable(false);

    vk::PipelineColorBlendAttachmentState color_blend_attachment{};
    color_blend_attachment.setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);
    color_blend_attachment.setBlendEnable(true);
    color_blend_attachment.setSrcColorBlendFactor(vk::BlendFactor::eSrcAlpha);
    color_blend_attachment.setDstColorBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha);
    color_blend_attachment.setColorBlendOp(vk::BlendOp::eAdd);
    color_blend_attachment.setSrcAlphaBlendFactor(vk::BlendFactor::eOne);
    color_blend_attachment.setDstAlphaBlendFactor(vk::BlendFactor::eZero);
    color_blend_attachment.setAlphaBlendOp(vk::BlendOp::eAdd);

    vk::PipelineColorBlendStateCreateInfo color_blend_state_create_info{};
    color_blend_state_create_info.setLogicOpEnable(false);
    color_blend_state_create_info.setLogicOp(vk::LogicOp::eCopy);
    color_blend_state_create_info.setAttachmentCount(1);
    color_blend_state_create_info.setPAttachments(&color_blend_attachment);
    color_blend_state_create_info.setBlendConstants({0.0f, 0.0f, 0.0f, 0.0f});

    std::vector<vk::DescriptorSetLayout> descriptor_set_layouts{};
    descriptor_set_layouts.emplace_back(Graphics::get_instance()->get_global_descriptor_set_layout());

    // descriptor set layouts array
//        auto descriptor_set_layouts = std::array{
//                Graphics::Get()->per_frame_descriptor_set_layout,
//                Graphics::Get()->per_material_descriptor_set_layout
//        };
//        default_material.descriptor_set = Graphics::Get()->allocate_material_descriptor_set();

//        vk::PushConstantRange push_constant_range{};
//        push_constant_range.setStageFlags(vk::ShaderStageFlagBits::eVertex);
//        push_constant_range.setSize(sizeof(glm::mat4));
//
    // layout
    vk::PipelineLayoutCreateInfo pipeline_layout_create_info{};
    pipeline_layout_create_info.setSetLayouts(descriptor_set_layouts);
//        pipeline_layout_create_info.setPushConstantRangeCount(1);
//        pipeline_layout_create_info.setPPushConstantRanges(&push_constant_range);

    auto material = std::make_shared<Material>();
    material->pipeline_layout = context().device.createPipelineLayout(pipeline_layout_create_info);

    vk::PipelineDepthStencilStateCreateInfo depth_stencil_state_create_info{};
    depth_stencil_state_create_info.setDepthTestEnable(true);
    depth_stencil_state_create_info.setDepthWriteEnable(true);
    depth_stencil_state_create_info.setDepthCompareOp(vk::CompareOp::eLess);
    depth_stencil_state_create_info.setDepthBoundsTestEnable(false);
    depth_stencil_state_create_info.setStencilTestEnable(false);

    vk::DynamicState dynamic_states[] = {
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor
    };

    vk::PipelineDynamicStateCreateInfo dynamic_state_create_info{};
    dynamic_state_create_info.setDynamicStateCount(2);
    dynamic_state_create_info.setPDynamicStates(dynamic_states);

    vk::GraphicsPipelineCreateInfo pipeline_create_info{};
    pipeline_create_info.setStageCount(2);
    pipeline_create_info.setPStages(shader_stages);
    pipeline_create_info.setPVertexInputState(&vertex_input_create_info);
    pipeline_create_info.setPInputAssemblyState(&input_assembly_create_info);
    pipeline_create_info.setPViewportState(&viewport_state_create_info);
    pipeline_create_info.setPRasterizationState(&rasterization_state_create_info);
    pipeline_create_info.setPMultisampleState(&multisample_state_create_info);
    pipeline_create_info.setPColorBlendState(&color_blend_state_create_info);
    pipeline_create_info.setPDepthStencilState(&depth_stencil_state_create_info);
    pipeline_create_info.setPDynamicState(&dynamic_state_create_info);
    pipeline_create_info.setLayout(material->pipeline_layout);
    pipeline_create_info.setRenderPass(Graphics::get_instance()->get_default_render_pass());
    pipeline_create_info.setSubpass(0);
    pipeline_create_info.setBasePipelineHandle(nullptr);
    pipeline_create_info.setBasePipelineIndex(-1);

    check(context().device.createGraphicsPipelines(nullptr, 1, &pipeline_create_info, nullptr, &material->pipeline));

    context().device.destroyShaderModule(vs);
    context().device.destroyShaderModule(fs);

    return material;
}
