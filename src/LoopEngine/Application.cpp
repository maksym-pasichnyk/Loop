#include "Application.hpp"
#include "spdlog/spdlog.h"

using LoopEngine::Application;
using LoopEngine::Core::Singleton;
using LoopEngine::Platform::Display;
using LoopEngine::Graphics::Context;
using LoopEngine::Graphics::Graphics;
using LoopEngine::Input::InputSystem;
using LoopEngine::Event::EventSystem;
using LoopEngine::Camera::CameraSystem;

using LoopEngine::Event::InitEvent;
using LoopEngine::Event::BeforeDrawEvent;
using LoopEngine::Event::DrawEvent;
using LoopEngine::Event::AfterDrawEvent;
using LoopEngine::Event::QuitEvent;
using LoopEngine::Event::UpdateEvent;
using LoopEngine::Event::EventHandler;

template<> Application *Singleton<Application>::instance = nullptr;

Application::Application(const char *title, int width, int height) : display(width, height, title) {
    input_system.load_config("input.yaml");
}

void Application::run() {
    using as_seconds = std::chrono::duration<float, std::chrono::seconds::period>;

    event_system.send_event(InitEvent{});
    input_system.update(0.0f);

    float time_since_start = 0.0f;
    auto start_time = std::chrono::high_resolution_clock::now();
    while (!display.should_close()) {
        auto current_time = std::chrono::high_resolution_clock::now();
        auto delta_time = as_seconds(current_time - start_time).count();
        start_time = current_time;
        time_since_start += delta_time;

        display.poll_events();
        input_system.update(delta_time);
        event_system.send_event(UpdateEvent{delta_time});

        auto result = graphics.setup_frame();
        if (result == vk::Result::eErrorOutOfDateKHR) {
            continue;
        }

        auto cmd = graphics.get_current_frame_command_buffer();

        auto rect = vk::Rect2D{{0, 0}, graphics.get_surface_extent()};

        std::array<vk::ClearValue, 2> clear_values{};
        clear_values[0].setColor(vk::ClearColorValue{}.setFloat32({0.0f, 0.0f, 0.0f, 1.0f}));
        clear_values[1].setDepthStencil(vk::ClearDepthStencilValue{}.setDepth(1.0f).setStencil(0));

        // begin render pass
        vk::RenderPassBeginInfo render_pass_begin_info{};
        render_pass_begin_info.setRenderPass(graphics.get_default_render_pass());
        render_pass_begin_info.setFramebuffer(graphics.get_current_swapchain_framebuffer());
        render_pass_begin_info.setRenderArea(rect);
        render_pass_begin_info.setClearValues(clear_values);
        cmd.beginRenderPass(render_pass_begin_info, vk::SubpassContents::eInline);

        vk::Viewport viewport{};
        viewport.setWidth(static_cast<float>(rect.extent.width));
        viewport.setHeight(static_cast<float>(rect.extent.height));
        viewport.setMinDepth(0.0f);
        viewport.setMaxDepth(1.0f);

        cmd.setScissor(0, rect);
        cmd.setViewport(0, viewport);

        event_system.send_event(BeforeDrawEvent{cmd});
        event_system.send_event(DrawEvent{cmd});
        event_system.send_event(AfterDrawEvent{cmd});

        cmd.endRenderPass();

        result = graphics.submit_frame();
        if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR) {
            continue;
        }
    }

    context.device.waitIdle();

    event_system.send_event(QuitEvent{});
}
