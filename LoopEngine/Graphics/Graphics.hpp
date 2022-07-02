#pragma once

#include "LoopEngine/Core/Singleton.hpp"

#include "vk_mem_alloc.h"
#include <vulkan/vulkan.hpp>

namespace LoopEngine::Graphics {
    struct Context;
    struct Material;
    struct UniformBuffer;
    struct Graphics final : LoopEngine::Core::Singleton<Graphics> {
    public:
        Graphics(Context& context);

        void initialize();
        void terminate();

    public:
        [[nodiscard]] auto get_surface_extent() const -> vk::Extent2D {
            return surface_extent;
        }

        [[nodiscard]] auto get_swapchain_images_count() const -> int {
            return static_cast<int>(images.size());
        }

        [[nodiscard]] auto get_swapchain_min_image_count() const -> int {
            return min_image_count;
        }

        [[nodiscard]] auto get_default_render_pass() const -> vk::RenderPass {
            return default_render_pass;
        }

        [[nodiscard]] auto get_current_swapchain_framebuffer() const -> vk::Framebuffer {
            return default_framebuffers[image_index];
        }

        [[nodiscard]] auto get_current_frame_command_buffer() const -> vk::CommandBuffer {
            return command_buffers[current_frame];
        }

        [[nodiscard]] auto get_global_descriptor_set_layout() const -> vk::DescriptorSetLayout {
            return global_descriptor_set_layout;
        }

        [[nodiscard]] auto get_current_frame_global_descriptor_set() const -> vk::DescriptorSet {
            return global_descriptor_sets[current_frame];
        }

        [[nodiscard]] auto begin_single_time_commands() -> vk::CommandBuffer;
        void submit_single_time_commands(vk::CommandBuffer cmd);
        [[nodiscard]] auto setup_frame() -> vk::Result;
        [[nodiscard]] auto submit_frame() -> vk::Result;

    private:
        void create_surface();
        void create_swapchain();
        void recreate_swapchain();
        void create_sync_objects();
        void create_command_pools();
        void create_command_buffers();
        void create_default_descriptors();
        void create_default_render_pass();
        void create_default_framebuffers();

    private:
        Context& context;
        size_t maxFramesInFlight = 3;

        std::vector<vk::Fence> fences{};
        std::vector<vk::Semaphore> image_available_semaphores{};
        std::vector<vk::Semaphore> render_finished_semaphores{};

        uint32_t image_index = 0;
        size_t current_frame = 0;

        uint32_t min_image_count = 0;

        std::vector<vk::CommandPool> command_pools{};
        std::vector<vk::CommandBuffer> command_buffers{};

        vk::SurfaceKHR surface{};
        vk::SwapchainKHR swapchain{};

        std::vector<vk::Image> images{};
        std::vector<vk::ImageView> views{};

        std::vector<vk::Image> depth_images{};
        std::vector<vk::ImageView> depth_views{};
        std::vector<VmaAllocation> depth_allocations{};

        vk::Extent2D surface_extent{};
        vk::RenderPass default_render_pass{};
        std::vector<vk::Framebuffer> default_framebuffers{};

        vk::DescriptorPool global_descriptor_pool{};
        vk::DescriptorSetLayout global_descriptor_set_layout{};
        std::vector<vk::DescriptorSet> global_descriptor_sets{};
        std::vector<std::shared_ptr<UniformBuffer>> global_uniform_buffers;
    };

    extern void bind_global_descriptor_sets(vk::CommandBuffer cmd, const Material& material, int set);
}