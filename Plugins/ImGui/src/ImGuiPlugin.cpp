#include "ImGuiPlugin.hpp"
#include "LoopEngine/Application.hpp"
#include "LoopEngine/Platform/Window.hpp"
#include "LoopEngine/Graphics/Context.hpp"
#include "LoopEngine/Graphics/Graphics.hpp"

#include "spdlog/spdlog.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"

using LoopEngine::Application;
using LoopEngine::Event::EventSystem;
using LoopEngine::Platform::Window;
using LoopEngine::Graphics::check;
using LoopEngine::Graphics::Context;
using LoopEngine::Graphics::Graphics;

void ImGuiPlugin::on_create() {
    vk::DescriptorPoolSize pool_size{};
    pool_size.setType(vk::DescriptorType::eCombinedImageSampler);
    pool_size.setDescriptorCount(1);

    vk::DescriptorPoolCreateInfo pool_info{};
    pool_info.setMaxSets(1);
    pool_info.setPoolSizeCount(1);
    pool_info.setPPoolSizes(&pool_size);
    imgui_descriptor_pool = Context::get_instance()->device.createDescriptorPool(pool_info);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui_ImplVulkan_LoadFunctions([](const char* function_name, void*) {
        return Context::get_instance()->instance.getProcAddr(function_name);
    });
    ImGui_ImplGlfw_InitForVulkan(Application::get_instance()->get_window().get_native_handle(), true);

    ImGui_ImplVulkan_InitInfo info{};
    info.Instance = Context::get_instance()->instance;
    info.PhysicalDevice = Context::get_instance()->physical_device;
    info.Device = Context::get_instance()->device;
    info.QueueFamily = Context::get_instance()->graphics_queue_family_index;
    info.Queue = Context::get_instance()->graphics_queue;
    info.DescriptorPool = imgui_descriptor_pool;
    info.MinImageCount = Graphics::get_instance()->get_swapchain_min_image_count();
    info.ImageCount = Graphics::get_instance()->get_swapchain_images_count();
    info.CheckVkResultFn = [](VkResult result) {
        check(vk::Result(result));
    };
    ImGui_ImplVulkan_Init(&info, Graphics::get_instance()->get_default_render_pass());

    ImGui::GetIO().Fonts->AddFontDefault();

    auto cmd = Graphics::get_instance()->begin_single_time_commands();
    ImGui_ImplVulkan_CreateFontsTexture(cmd);
    Graphics::get_instance()->submit_single_time_commands(cmd);
}

void ImGuiPlugin::on_update(float dt) {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    EventSystem::get_global_event_queue()->send_event(ImGuiDrawEvent{});

    ImGui::Render();
}

void ImGuiPlugin::on_after_draw(vk::CommandBuffer cmd) {
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd, VK_NULL_HANDLE);
}

void ImGuiPlugin::on_destroy() {
    Context::get_instance()->device.destroyDescriptorPool(imgui_descriptor_pool);

    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}