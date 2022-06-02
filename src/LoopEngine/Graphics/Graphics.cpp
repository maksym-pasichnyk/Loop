#include "Graphics.hpp"
#include "Context.hpp"
#include "LoopEngine/Platform/Display.hpp"

#include <set>
#include "GLFW/glfw3.h"
#include "spdlog/spdlog.h"
#include "range/v3/range/conversion.hpp"

using LoopEngine::Core::Singleton;
using LoopEngine::Platform::Display;
using LoopEngine::Graphics::Context;
using LoopEngine::Graphics::Graphics;

template<> Graphics* Singleton<Graphics>::instance = nullptr;

static auto select_surface_extent(const vk::Extent2D& extent, const vk::SurfaceCapabilitiesKHR &capabilities) -> vk::Extent2D {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    }
    auto width = std::clamp(extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    auto height = std::clamp(extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
    return {width, height};
}

Graphics::Graphics() {
    create_surface();
    create_swapchain();
    create_sync_objects();
    create_command_pools();
    create_command_buffers();
//        create_global_descriptors();
    create_default_render_pass();
    create_default_framebuffers();
//        create_material_descriptor_pool();
}

Graphics::~Graphics() {
    for (size_t i = 0; i < views.size(); i++) {
        Context::get_instance()->device.destroyImageView(views[i]);
        Context::get_instance()->device.destroyImageView(depth_views[i]);
        Context::get_instance()->device.destroyFramebuffer(default_framebuffers[i]);

        vmaDestroyImage(Context::get_instance()->allocator, depth_images[i], depth_allocations[i]);
    }

    for (size_t i = 0; i < maxFramesInFlight; i++) {
        Context::get_instance()->device.destroyFence(fences[i]);
        Context::get_instance()->device.destroySemaphore(image_available_semaphores[i]);
        Context::get_instance()->device.destroySemaphore(render_finished_semaphores[i]);

        Context::get_instance()->device.freeCommandBuffers(command_pools[i], 1, &command_buffers[i]);
        Context::get_instance()->device.destroyCommandPool(command_pools[i]);
//            destroy_buffer(global_uniform_buffers[i]);
    }

    Context::get_instance()->device.destroyRenderPass(default_render_pass);

//        Context::get_instance()->device.destroyDescriptorPool(per_frame_descriptor_pool);
//        Context::get_instance()->device.destroyDescriptorSetLayout(per_frame_descriptor_set_layout);
//        Context::get_instance()->device.destroyDescriptorPool(per_material_descriptor_pool);
//        Context::get_instance()->device.destroyDescriptorSetLayout(per_material_descriptor_set_layout);

    Context::get_instance()->device.destroySwapchainKHR(swapchain);
    Context::get_instance()->instance.destroySurfaceKHR(surface);
}

auto Graphics::begin_single_time_commands() -> vk::CommandBuffer {
    vk::CommandBufferAllocateInfo command_buffer_allocate_info{};
    command_buffer_allocate_info.setCommandBufferCount(1);
    command_buffer_allocate_info.setCommandPool(command_pools[current_frame]);
    command_buffer_allocate_info.setLevel(vk::CommandBufferLevel::ePrimary);
    auto cmd = Context::get_instance()->device.allocateCommandBuffers(command_buffer_allocate_info).front();

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
    auto fence = Context::get_instance()->device.createFence(vk::FenceCreateInfo{});
    check(Context::get_instance()->graphics_queue.submit(1, &submit_info, fence));

    // wait for fence
    check(Context::get_instance()->device.waitForFences(1, &fence, true, std::numeric_limits<uint64_t>::max()));

    // destroy command buffer
    Context::get_instance()->device.freeCommandBuffers(command_pools[current_frame], cmd);

    // destroy fence
    Context::get_instance()->device.destroyFence(fence);
}

auto Graphics::setup_frame() -> vk::Result {
    // wait for fence to be signaled
    static constexpr auto timeout = std::numeric_limits<uint64_t>::max();
    check(Context::get_instance()->device.waitForFences(1, &fences[current_frame], true, timeout));

    // acquire next image
    image_index = std::numeric_limits<uint32_t>::max();
    auto result = Context::get_instance()->device.acquireNextImageKHR(
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

    check(Context::get_instance()->device.resetFences(1, &fences[current_frame]));

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
    barrier.setSrcQueueFamilyIndex(Context::get_instance()->graphics_queue_family_index);
    barrier.setDstQueueFamilyIndex(Context::get_instance()->present_queue_family_index);
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

    check(Context::get_instance()->graphics_queue.submit(1, &submit_info, fences[current_frame]));

    // present image
    vk::PresentInfoKHR present_info{};
    present_info.setWaitSemaphoreCount(1);
    present_info.setPWaitSemaphores(&render_finished_semaphores[current_frame]);
    present_info.setSwapchainCount(1);
    present_info.setPSwapchains(&swapchain);
    present_info.setPImageIndices(&image_index);

    current_frame = (current_frame + 1) % maxFramesInFlight;

    vk::Result result = Context::get_instance()->present_queue.presentKHR(&present_info);
    if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR) {
        recreate_swapchain();
    } else if (result != vk::Result::eSuccess) {
        throw std::runtime_error(vk::to_string(result));
    }
    return result;
}

void Graphics::create_surface() {
    glfwCreateWindowSurface(Context::get_instance()->instance, Display::get_native_window_handle(), nullptr, reinterpret_cast<VkSurfaceKHR *>(&surface));
}

void Graphics::create_swapchain() {
    auto capabilities = Context::get_instance()->physical_device.getSurfaceCapabilitiesKHR(surface);
    surface_extent = select_surface_extent(vk::Extent2D{0, 0}, capabilities);

    min_image_count = 3;
    if (capabilities.maxImageCount > 0 && min_image_count > capabilities.maxImageCount) {
        min_image_count = capabilities.maxImageCount;
    }

    auto queue_family_indices = std::set{
        Context::get_instance()->graphics_queue_family_index,
        Context::get_instance()->present_queue_family_index
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

    swapchain = Context::get_instance()->device.createSwapchainKHR(create_info);
    images = Context::get_instance()->device.getSwapchainImagesKHR(swapchain);

    views.resize(images.size());
    for (size_t i = 0; i < images.size(); i++) {
        vk::ImageViewCreateInfo view_create_info{};
        view_create_info.setImage(images[i]);
        view_create_info.setViewType(vk::ImageViewType::e2D);
        view_create_info.setFormat(vk::Format::eB8G8R8A8Unorm);
        view_create_info.setSubresourceRange({vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});

        views[i] = Context::get_instance()->device.createImageView(view_create_info);
    }
}

void Graphics::recreate_swapchain() {
    int width = 0, height = 0;
    glfwGetFramebufferSize(Display::get_native_window_handle(), &width, &height);
    while (width == 0 || height == 0) {
        glfwWaitEvents();
        glfwGetFramebufferSize(Display::get_native_window_handle(), &width, &height);
    }

    // wait idle
    Context::get_instance()->device.waitIdle();

    for (size_t i = 0; i < views.size(); i++) {
        Context::get_instance()->device.destroyImageView(views[i]);
        Context::get_instance()->device.destroyImageView(depth_views[i]);
        Context::get_instance()->device.destroyFramebuffer(default_framebuffers[i]);

        vmaDestroyImage(Context::get_instance()->allocator, depth_images[i], depth_allocations[i]);
    }

    Context::get_instance()->device.destroySwapchainKHR(swapchain);

    create_swapchain();
    create_default_framebuffers();
}

void Graphics::create_sync_objects() {
    image_available_semaphores.resize(maxFramesInFlight);
    render_finished_semaphores.resize(maxFramesInFlight);
    fences.resize(maxFramesInFlight);

    for (size_t i = 0; i < maxFramesInFlight; i++) {
        vk::SemaphoreCreateInfo semaphore_create_info{};
        image_available_semaphores[i] = Context::get_instance()->device.createSemaphore(semaphore_create_info);
        render_finished_semaphores[i] = Context::get_instance()->device.createSemaphore(semaphore_create_info);

        vk::FenceCreateInfo fence_create_info{};
        fence_create_info.setFlags(vk::FenceCreateFlagBits::eSignaled);
        fences[i] = Context::get_instance()->device.createFence(fence_create_info);
    }
}

void Graphics::create_command_pools() {
    vk::CommandPoolCreateInfo pool_create_info{};
    pool_create_info.setQueueFamilyIndex(Context::get_instance()->graphics_queue_family_index);
    pool_create_info.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);

    command_pools.resize(maxFramesInFlight);
    for (size_t i = 0; i < maxFramesInFlight; i++) {
        command_pools[i] = Context::get_instance()->device.createCommandPool(pool_create_info);
    }
}

void Graphics::create_command_buffers() {
    for (size_t i = 0; i < maxFramesInFlight; i++) {
        vk::CommandBufferAllocateInfo alloc_info{};
        alloc_info.setCommandPool(command_pools[i]);
        alloc_info.setLevel(vk::CommandBufferLevel::ePrimary);
        alloc_info.setCommandBufferCount(1);
        command_buffers.emplace_back(Context::get_instance()->device.allocateCommandBuffers(alloc_info).front());
    }
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
    depth_attachment.setFormat(Context::get_instance()->depth_format);
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

    default_render_pass = Context::get_instance()->device.createRenderPass(render_pass_create_info);
}

void Graphics::create_default_framebuffers() {
    depth_views.resize(views.size());
    depth_images.resize(views.size());
    depth_allocations.resize(views.size());
    default_framebuffers.resize(views.size());

    for (size_t i = 0; i < views.size(); i++) {
        vk::ImageCreateInfo image_create_info{};
        image_create_info.setImageType(vk::ImageType::e2D);
        image_create_info.setFormat(Context::get_instance()->depth_format);
        image_create_info.setExtent({surface_extent.width, surface_extent.height, 1});
        image_create_info.setMipLevels(1);
        image_create_info.setArrayLayers(1);
        image_create_info.setSamples(vk::SampleCountFlagBits::e1);
        image_create_info.setTiling(vk::ImageTiling::eOptimal);
        image_create_info.setUsage(vk::ImageUsageFlagBits::eDepthStencilAttachment);

        VmaAllocationCreateInfo alloc_info{};
        alloc_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;

        vmaCreateImage(
            Context::get_instance()->allocator,
            reinterpret_cast<const VkImageCreateInfo *>(&image_create_info),
            &alloc_info,
            reinterpret_cast<VkImage *>(&depth_images[i]),
            &depth_allocations[i],
            nullptr
        );

        vk::ImageViewCreateInfo image_view_create_info{};
        image_view_create_info.setImage(depth_images[i]);
        image_view_create_info.setViewType(vk::ImageViewType::e2D);
        image_view_create_info.setFormat(Context::get_instance()->depth_format);
        image_view_create_info.setSubresourceRange(vk::ImageSubresourceRange{vk::ImageAspectFlagBits::eDepth, 0, 1, 0, 1});

        depth_views[i] = Context::get_instance()->device.createImageView(image_view_create_info);

        std::vector<vk::ImageView> attachments{};
        attachments.push_back(views[i]);
        attachments.push_back(depth_views[i]);

        vk::FramebufferCreateInfo framebuffer_create_info{};
        framebuffer_create_info.setRenderPass(default_render_pass);
        framebuffer_create_info.setAttachments(attachments);
        framebuffer_create_info.setWidth(surface_extent.width);
        framebuffer_create_info.setHeight(surface_extent.height);
        framebuffer_create_info.setLayers(1);

        default_framebuffers[i] = Context::get_instance()->device.createFramebuffer(framebuffer_create_info);
    }
}
