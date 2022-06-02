#pragma once

#ifndef VMA_DEBUG_LOG
#define VMA_DEBUG_LOG(format, ...) do { printf(format "\n" __VA_OPT__(,) __VA_ARGS__); } while(false)
#endif

#include <cstdio>
#include <cstdlib>
#include "vk_mem_alloc.h"
#include <vulkan/vulkan.hpp>

#include "LoopEngine/Core/Singleton.hpp"

namespace LoopEngine::Graphics {
    struct Context final : LoopEngine::Core::Singleton<Context> {
        vk::Device device{};
        vk::Instance instance{};
        vk::PhysicalDevice physical_device{};
        vk::DebugUtilsMessengerEXT debug_utils{};

        uint32_t graphics_queue_family_index{};
        uint32_t present_queue_family_index{};
        uint32_t compute_queue_family_index{};

        vk::Queue graphics_queue{};
        vk::Queue present_queue{};
        vk::Queue compute_queue{};

        VmaAllocator allocator{};

        vk::Format depth_format{};

        Context();
        ~Context();

    private:
        void create_instance();
        void create_debug_utils();
        void select_physical_device();
        void create_logical_device();
        void create_memory_allocator();
    };

    inline auto context() -> Context& {
        return *Context::get_instance();
    }

    inline static void check(vk::Result result) {
        if (result != vk::Result::eSuccess) {
            throw std::runtime_error(vk::to_string(result));
        }
    }
}