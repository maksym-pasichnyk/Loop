#include "Graphics.hpp"
#include "Context.hpp"
#include "Material.hpp"
#include "UniformBuffer.hpp"
#include "LoopEngine/Application.hpp"
#include "LoopEngine/Platform/Window.hpp"
#include "LoopEngine/Camera/Camera.hpp"
#include "LoopEngine/Camera/CameraSystem.hpp"

#include <set>
#include "GLFW/glfw3.h"
#include "glm/mat4x4.hpp"
#include "spdlog/spdlog.h"
#include "range/v3/range/conversion.hpp"
#include "LoopEngine/Core/Singleton.hpp"

using LoopEngine::Application;
using LoopEngine::Core::Singleton;
using LoopEngine::Platform::Window;
using LoopEngine::Graphics::Context;
using LoopEngine::Graphics::Graphics;
using LoopEngine::Camera::get_default_camera;

template<> Graphics* Singleton<Graphics>::instance = nullptr;

static auto select_surface_extent(const vk::Extent2D& extent, const vk::SurfaceCapabilitiesKHR &capabilities) -> vk::Extent2D {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    }
    auto width = std::clamp(extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    auto height = std::clamp(extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
    return {width, height};
}

Graphics::Graphics(Context& context) : context(context) {}

void Graphics::initialize() {
    create_surface();
    create_swapchain();
    create_sync_objects();
    create_command_pools();
    create_command_buffers();
    create_default_descriptors();
    create_default_render_pass();
    create_default_framebuffers();
//        create_material_descriptor_pool();
}

void Graphics::terminate() {
    for (size_t i = 0; i < views.size(); i++) {
        context.device.destroyImageView(views[i]);
        context.device.destroyImageView(depth_views[i]);
        context.device.destroyFramebuffer(default_framebuffers[i]);

        vmaDestroyImage(context.allocator, depth_images[i], depth_allocations[i]);
    }

    for (size_t i = 0; i < maxFramesInFlight; i++) {
        context.device.destroyFence(fences[i]);
        context.device.destroySemaphore(image_available_semaphores[i]);
        context.device.destroySemaphore(render_finished_semaphores[i]);

        context.device.freeCommandBuffers(command_pools[i], 1, &command_buffers[i]);
        context.device.destroyCommandPool(command_pools[i]);

        release_uniform_buffer(*global_uniform_buffers[i]);
    }
    context.device.destroyDescriptorSetLayout(global_descriptor_set_layout);
    context.device.destroyDescriptorPool(global_descriptor_pool);
    context.device.destroyRenderPass(default_render_pass);

    context.device.destroySwapchainKHR(swapchain);
    context.instance.destroySurfaceKHR(surface);
}

auto Graphics::begin_single_time_commands() -> vk::CommandBuffer {
    vk::CommandBufferAllocateInfo command_buffer_allocate_info{};
    command_buffer_allocate_info.setCommandBufferCount(1);
    command_buffer_allocate_info.setCommandPool(command_pools[current_frame]);
    command_buffer_allocate_info.setLevel(vk::CommandBufferLevel::ePrimary);
    auto cmd = context.device.allocateCommandBuffers(command_buffer_allocate_info).front();

    vk::CommandBufferBeginInfo begin_info{};
    begin_info.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    cmd.begin(begin_info);
    return cmd;
}

void Graphics::submit_single_time_commands(vk::CommandBuffer cmd) {
    cmd.end();

    // submit
    vk::SubmitInfo submit_info{};
    submit_info.setCommandBufferCount(1);
    submit_info.setPCommandBuffers(&cmd);

    // create fence
    auto fence = context.device.createFence(vk::FenceCreateInfo{});
    check(context.graphics_queue.submit(1, &submit_info, fence));

    // wait for fence
    check(context.device.waitForFences(1, &fence, true, std::numeric_limits<uint64_t>::max()));

    // destroy command buffer
    context.device.freeCommandBuffers(command_pools[current_frame], cmd);

    // destroy fence
    context.device.destroyFence(fence);
}

auto Graphics::setup_frame() -> vk::Result {
    // wait for fence to be signaled
    static constexpr auto timeout = std::numeric_limits<uint64_t>::max();
    check(context.device.waitForFences(1, &fences[current_frame], true, timeout));

    // acquire next image
    image_index = std::numeric_limits<uint32_t>::max();
    auto result = context.device.acquireNextImageKHR(
        swapchain,
        std::numeric_limits<uint64_t>::max(),
        image_available_semaphores[current_frame],
        vk::Fence{},
        &image_index
    );
    if (result == vk::Result::eErrorOutOfDateKHR) {
        recreate_swapchain();
        return result;
    } else if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR) {
        throw std::runtime_error(vk::to_string(result));
    }

    check(context.device.resetFences(1, &fences[current_frame]));

    auto camera = get_default_camera();

    glm::mat4 data[2];
    data[0] = camera->get_projection_matrix();
    data[1] = camera->get_view_matrix();

    update_uniform_buffer(*global_uniform_buffers[current_frame], data, sizeof(data));

    vk::CommandBufferBeginInfo begin_info{};
    begin_info.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    command_buffers[current_frame].begin(begin_info);
    return result;
}

auto Graphics::submit_frame() -> vk::Result {
    // change swapchain image layout to present
    vk::ImageMemoryBarrier barrier{};
//        barrier.setOldLayout(vk::ImageLayout::eUndefined);
    barrier.setOldLayout(vk::ImageLayout::eColorAttachmentOptimal);
    barrier.setNewLayout(vk::ImageLayout::ePresentSrcKHR);
    barrier.setSrcQueueFamilyIndex(context.graphics_queue_family_index);
    barrier.setDstQueueFamilyIndex(context.present_queue_family_index);
    barrier.setImage(images[image_index]);
    barrier.setSubresourceRange(vk::ImageSubresourceRange{vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});

    command_buffers[current_frame].pipelineBarrier(vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eBottomOfPipe, vk::DependencyFlags{}, nullptr, nullptr, barrier);
    command_buffers[current_frame].end();

    // submit command buffer
    vk::PipelineStageFlags wait_stages[] = {
            vk::PipelineStageFlagBits::eColorAttachmentOutput
    };

    vk::SubmitInfo submit_info{};
    submit_info.setCommandBufferCount(1);
    submit_info.setPCommandBuffers(&command_buffers[current_frame]);
    submit_info.setWaitSemaphoreCount(1);
    submit_info.setPWaitSemaphores(&image_available_semaphores[current_frame]);
    submit_info.setPWaitDstStageMask(wait_stages);
    submit_info.setSignalSemaphoreCount(1);
    submit_info.setPSignalSemaphores(&render_finished_semaphores[current_frame]);

    check(context.graphics_queue.submit(1, &submit_info, fences[current_frame]));

    // present image
    vk::PresentInfoKHR present_info{};
    present_info.setWaitSemaphoreCount(1);
    present_info.setPWaitSemaphores(&render_finished_semaphores[current_frame]);
    present_info.setSwapchainCount(1);
    present_info.setPSwapchains(&swapchain);
    present_info.setPImageIndices(&image_index);

    current_frame = (current_frame + 1) % maxFramesInFlight;

    vk::Result result = context.present_queue.presentKHR(&present_info);
    if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR) {
        recreate_swapchain();
    } else if (result != vk::Result::eSuccess) {
        throw std::runtime_error(vk::to_string(result));
    }
    return result;
}

void Graphics::create_surface() {
    glfwCreateWindowSurface(context.instance, Application::get_instance()->get_window().get_native_handle(), nullptr, reinterpret_cast<VkSurfaceKHR *>(&surface));
}

void Graphics::create_swapchain() {
    auto capabilities = context.physical_device.getSurfaceCapabilitiesKHR(surface);
    surface_extent = select_surface_extent(vk::Extent2D{0, 0}, capabilities);

    min_image_count = 3;
    if (capabilities.maxImageCount > 0 && min_image_count > capabilities.maxImageCount) {
        min_image_count = capabilities.maxImageCount;
    }

    auto queue_family_indices = std::set{
        context.graphics_queue_family_index,
        context.present_queue_family_index
    } | ranges::to_vector;

    vk::SwapchainCreateInfoKHR create_info{};
    create_info.setSurface(surface);
    create_info.setMinImageCount(min_image_count);
    create_info.setImageFormat(vk::Format::eB8G8R8A8Unorm);
    create_info.setImageColorSpace(vk::ColorSpaceKHR::eSrgbNonlinear);
    create_info.setImageExtent(surface_extent);
    create_info.setImageArrayLayers(1);
    create_info.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);
    if (queue_family_indices.size() > 1) {
        create_info.setImageSharingMode(vk::SharingMode::eConcurrent);
        create_info.setQueueFamilyIndices(queue_family_indices);
    } else {
        create_info.setImageSharingMode(vk::SharingMode::eExclusive);
    }
    create_info.setPreTransform(capabilities.currentTransform);
    create_info.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque);
    create_info.setPresentMode(vk::PresentModeKHR::eFifo);
    create_info.setClipped(true);
    create_info.setOldSwapchain(nullptr);

    swapchain = context.device.createSwapchainKHR(create_info);
    images = context.device.getSwapchainImagesKHR(swapchain);

    views.resize(images.size());
    for (size_t i = 0; i < images.size(); i++) {
        vk::ImageViewCreateInfo view_create_info{};
        view_create_info.setImage(images[i]);
        view_create_info.setViewType(vk::ImageViewType::e2D);
        view_create_info.setFormat(vk::Format::eB8G8R8A8Unorm);
        view_create_info.setSubresourceRange({vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});

        views[i] = context.device.createImageView(view_create_info);
    }
}

void Graphics::recreate_swapchain() {
    auto window = Application::get_instance()->get_window().get_native_handle();

    int width = 0, height = 0;
    glfwGetFramebufferSize(window, &width, &height);
    while (width == 0 || height == 0) {
        glfwWaitEvents();
        glfwGetFramebufferSize(window, &width, &height);
    }

    // wait idle
    context.device.waitIdle();

    for (size_t i = 0; i < views.size(); i++) {
        context.device.destroyImageView(views[i]);
        context.device.destroyImageView(depth_views[i]);
        context.device.destroyFramebuffer(default_framebuffers[i]);

        vmaDestroyImage(context.allocator, depth_images[i], depth_allocations[i]);
    }

    context.device.destroySwapchainKHR(swapchain);

    create_swapchain();
    create_default_framebuffers();
}

void Graphics::create_sync_objects() {
    image_available_semaphores.resize(maxFramesInFlight);
    render_finished_semaphores.resize(maxFramesInFlight);
    fences.resize(maxFramesInFlight);

    for (size_t i = 0; i < maxFramesInFlight; i++) {
        vk::SemaphoreCreateInfo semaphore_create_info{};
        image_available_semaphores[i] = context.device.createSemaphore(semaphore_create_info);
        render_finished_semaphores[i] = context.device.createSemaphore(semaphore_create_info);

        vk::FenceCreateInfo fence_create_info{};
        fence_create_info.setFlags(vk::FenceCreateFlagBits::eSignaled);
        fences[i] = context.device.createFence(fence_create_info);
    }
}

void Graphics::create_command_pools() {
    vk::CommandPoolCreateInfo pool_create_info{};
    pool_create_info.setQueueFamilyIndex(context.graphics_queue_family_index);
    pool_create_info.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);

    command_pools.resize(maxFramesInFlight);
    for (size_t i = 0; i < maxFramesInFlight; i++) {
        command_pools[i] = context.device.createCommandPool(pool_create_info);
    }
}

void Graphics::create_command_buffers() {
    for (size_t i = 0; i < maxFramesInFlight; i++) {
        vk::CommandBufferAllocateInfo alloc_info{};
        alloc_info.setCommandPool(command_pools[i]);
        alloc_info.setLevel(vk::CommandBufferLevel::ePrimary);
        alloc_info.setCommandBufferCount(1);
        command_buffers.emplace_back(context.device.allocateCommandBuffers(alloc_info).front());
    }
}

void Graphics::create_default_descriptors() {
    std::array<vk::DescriptorPoolSize, 1> pool_sizes{};
    pool_sizes[0].setType(vk::DescriptorType::eUniformBuffer);
    pool_sizes[0].setDescriptorCount(maxFramesInFlight);

    vk::DescriptorPoolCreateInfo pool_create_info{};
    pool_create_info.setMaxSets(maxFramesInFlight);
    pool_create_info.setPoolSizes(pool_sizes);
    global_descriptor_pool = context.device.createDescriptorPool(pool_create_info);

    std::array<vk::DescriptorSetLayoutBinding, 1> set_layout_bindings{};
    set_layout_bindings[0].setBinding(0);
    set_layout_bindings[0].setDescriptorType(vk::DescriptorType::eUniformBuffer);
    set_layout_bindings[0].setDescriptorCount(1);
    set_layout_bindings[0].setStageFlags(vk::ShaderStageFlagBits::eVertex);

    vk::DescriptorSetLayoutCreateInfo set_layout_create_info{};
    set_layout_create_info.setBindings(set_layout_bindings);
    global_descriptor_set_layout = context.device.createDescriptorSetLayout(set_layout_create_info);

    global_descriptor_sets.resize(maxFramesInFlight);
    global_uniform_buffers.resize(maxFramesInFlight);

    std::vector<vk::WriteDescriptorSet> write_descriptor_sets(maxFramesInFlight);
    std::vector<vk::DescriptorBufferInfo> descriptor_buffer_infos(maxFramesInFlight);

    for (size_t i = 0; i < maxFramesInFlight; ++i) {
        global_uniform_buffers[i] = create_uniform_buffer(sizeof(glm::mat4[2]));

        // create descriptor set
        vk::DescriptorSetAllocateInfo set_alloc_info{};
        set_alloc_info.setDescriptorPool(global_descriptor_pool);
        set_alloc_info.setSetLayouts(global_descriptor_set_layout);
        global_descriptor_sets[i] = context.device.allocateDescriptorSets(set_alloc_info).front();

        // update descriptor set
        descriptor_buffer_infos[i].setBuffer(global_uniform_buffers[i]->handle);
        descriptor_buffer_infos[i].setOffset(0);
        descriptor_buffer_infos[i].setRange(sizeof(glm::mat4[2]));

        write_descriptor_sets[i].setDstSet(global_descriptor_sets[i]);
        write_descriptor_sets[i].setDstBinding(0);
        write_descriptor_sets[i].setDescriptorCount(1);
        write_descriptor_sets[i].setDescriptorType(vk::DescriptorType::eUniformBuffer);
        write_descriptor_sets[i].setPBufferInfo(&descriptor_buffer_infos[i]);
    }
    context.device.updateDescriptorSets(write_descriptor_sets, {});
}

void Graphics::create_default_render_pass() {
    vk::AttachmentDescription color_attachment{};
    color_attachment.setFormat(vk::Format::eB8G8R8A8Unorm);
    color_attachment.setSamples(vk::SampleCountFlagBits::e1);
    color_attachment.setLoadOp(vk::AttachmentLoadOp::eClear);
    color_attachment.setStoreOp(vk::AttachmentStoreOp::eStore);
    color_attachment.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare);
    color_attachment.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);
    color_attachment.setInitialLayout(vk::ImageLayout::eUndefined);
    color_attachment.setFinalLayout(vk::ImageLayout::eColorAttachmentOptimal);

    vk::AttachmentDescription depth_attachment{};
    depth_attachment.setFormat(context.depth_format);
    depth_attachment.setSamples(vk::SampleCountFlagBits::e1);
    depth_attachment.setLoadOp(vk::AttachmentLoadOp::eClear);
    depth_attachment.setStoreOp(vk::AttachmentStoreOp::eDontCare);
    depth_attachment.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare);
    depth_attachment.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);
    depth_attachment.setInitialLayout(vk::ImageLayout::eUndefined);
    depth_attachment.setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

    vk::AttachmentDescription attachments[2] { color_attachment, depth_attachment };

    vk::AttachmentReference color_attachment_ref{};
    color_attachment_ref.setAttachment(0);
    color_attachment_ref.setLayout(vk::ImageLayout::eColorAttachmentOptimal);

    vk::AttachmentReference depth_attachment_ref{};
    depth_attachment_ref.setAttachment(1);
    depth_attachment_ref.setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

    vk::SubpassDescription subpass{};
    subpass.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics);
    subpass.setColorAttachmentCount(1);
    subpass.setPColorAttachments(&color_attachment_ref);
    subpass.setPDepthStencilAttachment(&depth_attachment_ref);

    vk::RenderPassCreateInfo render_pass_create_info{};
    render_pass_create_info.setAttachmentCount(2);
    render_pass_create_info.setPAttachments(attachments);
    render_pass_create_info.setSubpassCount(1);
    render_pass_create_info.setPSubpasses(&subpass);

    default_render_pass = context.device.createRenderPass(render_pass_create_info);
}

void Graphics::create_default_framebuffers() {
    depth_views.resize(views.size());
    depth_images.resize(views.size());
    depth_allocations.resize(views.size());
    default_framebuffers.resize(views.size());

    for (size_t i = 0; i < views.size(); i++) {
        vk::ImageCreateInfo image_create_info{};
        image_create_info.setImageType(vk::ImageType::e2D);
        image_create_info.setFormat(context.depth_format);
        image_create_info.setExtent({surface_extent.width, surface_extent.height, 1});
        image_create_info.setMipLevels(1);
        image_create_info.setArrayLayers(1);
        image_create_info.setSamples(vk::SampleCountFlagBits::e1);
        image_create_info.setTiling(vk::ImageTiling::eOptimal);
        image_create_info.setUsage(vk::ImageUsageFlagBits::eDepthStencilAttachment);

        VmaAllocationCreateInfo alloc_info{};
        alloc_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;

        vmaCreateImage(
            context.allocator,
            reinterpret_cast<const VkImageCreateInfo *>(&image_create_info),
            &alloc_info,
            reinterpret_cast<VkImage *>(&depth_images[i]),
            &depth_allocations[i],
            nullptr
        );

        vk::ImageViewCreateInfo image_view_create_info{};
        image_view_create_info.setImage(depth_images[i]);
        image_view_create_info.setViewType(vk::ImageViewType::e2D);
        image_view_create_info.setFormat(context.depth_format);
        image_view_create_info.setSubresourceRange(vk::ImageSubresourceRange{vk::ImageAspectFlagBits::eDepth, 0, 1, 0, 1});

        depth_views[i] = context.device.createImageView(image_view_create_info);

        std::vector<vk::ImageView> attachments{};
        attachments.push_back(views[i]);
        attachments.push_back(depth_views[i]);

        vk::FramebufferCreateInfo framebuffer_create_info{};
        framebuffer_create_info.setRenderPass(default_render_pass);
        framebuffer_create_info.setAttachments(attachments);
        framebuffer_create_info.setWidth(surface_extent.width);
        framebuffer_create_info.setHeight(surface_extent.height);
        framebuffer_create_info.setLayers(1);

        default_framebuffers[i] = context.device.createFramebuffer(framebuffer_create_info);
    }
}

void LoopEngine::Graphics::bind_global_descriptor_sets(vk::CommandBuffer cmd, const Material& material, int set) {
    auto ds = Graphics::get_instance()->get_current_frame_global_descriptor_set();
    cmd.bindDescriptorSets(material.bind_point, material.pipeline_layout, set, ds, {});
}
