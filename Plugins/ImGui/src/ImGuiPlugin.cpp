#include "ImGuiPlugin.hpp"
#include "LoopEngine/Platform/Display.hpp"
#include "LoopEngine/Graphics/Context.hpp"
#include "LoopEngine/Graphics/Graphics.hpp"

#include "spdlog/spdlog.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"

using LoopEngine::Event::get_global_event_queue;
using LoopEngine::Platform::Display;
using LoopEngine::Graphics::check;
using LoopEngine::Graphics::context;
using LoopEngine::Graphics::Graphics;

ImGuiPlugin::ImGuiPlugin() {
    init_event_handler.connect<&ImGuiPlugin::init>(this);
    update_event_handler.connect(ImGuiPlugin::update);
    draw_event_handler.connect(ImGuiPlugin::draw);
    quit_event_handler.connect<&ImGuiPlugin::quit>(this);

    get_global_event_queue()->add_event_handler(&init_event_handler);
    get_global_event_queue()->add_event_handler(&draw_event_handler);
    get_global_event_queue()->add_event_handler(&update_event_handler);
    get_global_event_queue()->add_event_handler(&quit_event_handler);
}

ImGuiPlugin::~ImGuiPlugin() {
    get_global_event_queue()->remove_event_handler(&init_event_handler);
    get_global_event_queue()->remove_event_handler(&draw_event_handler);
    get_global_event_queue()->remove_event_handler(&update_event_handler);
    get_global_event_queue()->remove_event_handler(&quit_event_handler);
}

void ImGuiPlugin::init(const LoopEngine::Event::InitEvent &event) {
    vk::DescriptorPoolSize pool_size{};
    pool_size.setType(vk::DescriptorType::eCombinedImageSampler);
    pool_size.setDescriptorCount(1);

    vk::DescriptorPoolCreateInfo pool_info{};
    pool_info.setMaxSets(1);
    pool_info.setPoolSizeCount(1);
    pool_info.setPPoolSizes(&pool_size);
    descriptor_pool = context().device.createDescriptorPool(pool_info);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui_ImplVulkan_LoadFunctions([](const char* function_name, void*) {
        return context().instance.getProcAddr(function_name);
    });
    ImGui_ImplGlfw_InitForVulkan(Display::get_native_window_handle(), true);

    ImGui_ImplVulkan_InitInfo info{};
    info.Instance = context().instance;
    info.PhysicalDevice = context().physical_device;
    info.Device = context().device;
    info.QueueFamily = context().graphics_queue_family_index;
    info.Queue = context().graphics_queue;
    info.DescriptorPool = descriptor_pool;
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

void ImGuiPlugin::update(const LoopEngine::Event::UpdateEvent &event) {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    get_global_event_queue()->send_event(ImGuiDrawEvent{});

    ImGui::Render();
}

void ImGuiPlugin::draw(const LoopEngine::Event::AfterDrawEvent &event) {
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), event.cmd, VK_NULL_HANDLE);
}

void ImGuiPlugin::quit(const LoopEngine::Event::QuitEvent &event) {
    context().device.destroyDescriptorPool(descriptor_pool);

    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}