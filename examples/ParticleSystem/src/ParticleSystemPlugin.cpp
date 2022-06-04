#include "ParticleSystemPlugin.hpp"
#include "ParticleSystem.hpp"

#include "LoopEngine/Camera/CameraSystem.hpp"
#include "LoopEngine/Input/InputSystem.hpp"
#include "LoopEngine/Input/InputController.hpp"
#include "LoopEngine/Platform/Display.hpp"
#include "LoopEngine/Application.hpp"
#include "spdlog/spdlog.h"
#include "glm/vec3.hpp"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"

using LoopEngine::Application;
using LoopEngine::Platform::Display;
using LoopEngine::Input::InputSystem;
using LoopEngine::Event::EventSystem;
using LoopEngine::Event::get_global_event_queue;
using LoopEngine::Graphics::check;
using LoopEngine::Graphics::context;
using LoopEngine::Graphics::Graphics;
using LoopEngine::Camera::get_default_camera;

ParticleSystemPlugin::ParticleSystemPlugin() {
    init_event_handler.connect<&ParticleSystemPlugin::init>(this);
    draw_event_handler.connect<&ParticleSystemPlugin::draw>(this);
    update_event_handler.connect<&ParticleSystemPlugin::update>(this);
    press_button_event_handler.connect<&ParticleSystemPlugin::on_press_button>(this);

    get_global_event_queue()->add_event_handler(&init_event_handler);
    get_global_event_queue()->add_event_handler(&draw_event_handler);
    get_global_event_queue()->add_event_handler(&update_event_handler);
    get_global_event_queue()->add_event_handler(&press_button_event_handler);

    particle_system = std::make_shared<ParticleSystem>(1000);
    particle_system->add_event_handler(&particle_system_update_handler);

    particle_system_update_handler.connect<&ParticleSystemPlugin::on_particle_system_update>(this);

    sub_particle_system = std::make_shared<ParticleSystem>(1000);
    sub_particle_system->add_event_handler(&sub_particle_system_update_handler);

    sub_particle_system_update_handler.connect<&ParticleSystemPlugin::on_sub_particle_system_update>(this);

    create_imgui_context();
}

ParticleSystemPlugin::~ParticleSystemPlugin() {
    destroy_imgui_context();

    particle_system->remove_event_handler(&particle_system_update_handler);
    sub_particle_system->remove_event_handler(&sub_particle_system_update_handler);

    get_global_event_queue()->remove_event_handler(&init_event_handler);
    get_global_event_queue()->remove_event_handler(&draw_event_handler);
    get_global_event_queue()->remove_event_handler(&update_event_handler);
    get_global_event_queue()->remove_event_handler(&press_button_event_handler);
}

void ParticleSystemPlugin::init(const InitEvent &event) {
    spdlog::info("ParticleSystemPlugin::init()");

    auto camera = get_default_camera();
    camera->set_perspective(60.0f, 16.0f / 12.0f, 0.1f, 1000.0f);
    camera->set_transform(glm::vec3(0.0f, 0.0f, -10.0f), glm::vec3(0.0f, 0.0f, 0.0f));
}

void ParticleSystemPlugin::update(const UpdateEvent &event) {
    update_camera(event);

    particle_system->update(event);
    sub_particle_system->update(event);
}

void ParticleSystemPlugin::create_imgui_context() {
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

    ImGuiIO& io = ImGui::GetIO();
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

    io.Fonts->AddFontDefault();

    auto cmd = Graphics::get_instance()->begin_single_time_commands();
    ImGui_ImplVulkan_CreateFontsTexture(cmd);
    Graphics::get_instance()->submit_single_time_commands(cmd);
}

void ParticleSystemPlugin::destroy_imgui_context() {
    context().device.destroyDescriptorPool(descriptor_pool);

    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void ParticleSystemPlugin::on_particle_system_update(const ParticleSystemUpdateEvent& event) {
    emit_delay += event.dt;
    while (emit_delay >= emit_rate) {
        emit_delay -= emit_rate;

        glm::vec3 direction{};
        direction.x = 0.0f;//std::uniform_real_distribution(-1.0f, 1.0f)(generator);
        direction.y = 1.0f;//std::uniform_real_distribution(-1.0f, 1.0f)(generator);
        direction.z = 0.0f;//std::uniform_real_distribution(-1.0f, 1.0f)(generator);

        glm::vec4 color{};
        color.x = std::uniform_real_distribution(0.0f, 1.0f)(generator);
        color.y = std::uniform_real_distribution(0.0f, 1.0f)(generator);
        color.z = std::uniform_real_distribution(0.0f, 1.0f)(generator);
        color.w = 1.0f;

        auto lifetime = std::uniform_real_distribution(1.0f, 2.0f)(generator);

        particle_system->emit(glm::vec3{}, color, direction * 5.0f, lifetime);
    }

    for (auto& particle : particle_system->get_particles()) {
        if (particle.lifetime <= 0.0f) {
            continue;
        }
        particle.color.w = std::clamp(particle.lifetime / particle.time, 0.0f, 1.0f);

        glm::vec3 direction{};
        direction.x = std::uniform_real_distribution(-1.0f, 1.0f)(generator) * 0.5f;
        direction.y = 0.0f;
        direction.z = std::uniform_real_distribution(-1.0f, 1.0f)(generator) * 0.5f;

        auto lifetime = std::uniform_real_distribution(0.0f, 1.0f)(generator);

        sub_particle_system->emit(particle.position, particle.color, direction, lifetime);
    }
}

void ParticleSystemPlugin::on_sub_particle_system_update(const ParticleSystemUpdateEvent &event) {
    for (auto& particle : sub_particle_system->get_particles()) {
        if (particle.lifetime <= 0.0f) {
            continue;
        }
        particle.color.w = std::clamp(particle.lifetime / particle.time, 0.0f, 1.0f);
        particle.velocity.y -= 9.8f * event.dt;
        particle.velocity.y = std::max(particle.velocity.y, -1.0f);
    }
}

void ParticleSystemPlugin::draw(const DrawEvent &event) {
    particle_system->draw(event);
    sub_particle_system->draw(event);

    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Particle System", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();

    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), event.cmd, VK_NULL_HANDLE);
}

void ParticleSystemPlugin::update_camera(const UpdateEvent &event) {
    if (lock_mouse) {
        glfwSetInputMode(Display::get_native_window_handle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } else {
        glfwSetInputMode(Display::get_native_window_handle(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        return;
    }

    auto move = InputSystem::get_instance()->get_axis("move");
    auto strafe = InputSystem::get_instance()->get_axis("strafe");

    auto camera = get_default_camera();
    auto position = camera->get_position();
    auto rotation = camera->get_rotation();

    bool flag = false;
    if (move != 0.0f || strafe != 0.0f) {
        flag = true;

        auto speed = 5.f;
        if (InputSystem::get_instance()->get_button("sprint")) {
            speed = 50.f;
        }
        auto orientation = camera->get_orientation();
        auto direction = glm::normalize(glm::vec3(strafe, 0.0f, move));
        auto velocity = glm::mat3(orientation) * direction * speed;

        position += velocity * event.dt;
    }

    auto delta = InputSystem::get_instance()->get_mouse_delta();
    if (delta != glm::vec2(0, 0)) {
        flag = true;

        const auto d4 = 0.5f * 0.6F + 0.2F;
        const auto d5 = d4 * d4 * d4 * 8.0f;

        rotation.y += delta.x * d5 * event.dt * 9.0f;
        rotation.x += delta.y * d5 * event.dt * 9.0f;
        rotation.x = glm::clamp(rotation.x, -90.0f, 90.0f);
    }

    if (flag) {
        camera->set_transform(position, rotation);
    }
}

void ParticleSystemPlugin::on_press_button(const ButtonPressEvent& event) {
    if (event.button == "camera") {
        lock_mouse = !lock_mouse;
    }
}

auto main(int argc, char** argv) -> int {
    Application application("Particle System", 800, 600);
    ParticleSystemPlugin particle_system_plugin{};
    application.run();
    return EXIT_SUCCESS;
}