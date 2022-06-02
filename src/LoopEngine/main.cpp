#include "LoopEngine/Platform/Display.hpp"
#include "LoopEngine/Graphics/Context.hpp"
#include "LoopEngine/Graphics/Graphics.hpp"
#include "LoopEngine/Input/InputSystem.hpp"
#include "LoopEngine/Event/EventSystem.hpp"
#include "LoopEngine/Camera/CameraSystem.hpp"

#include "spdlog/spdlog.h"

using LoopEngine::Platform::Display;
using LoopEngine::Graphics::Context;
using LoopEngine::Graphics::Graphics;
using LoopEngine::Input::InputSystem;
using LoopEngine::Event::EventSystem;
using LoopEngine::Camera::CameraSystem;

using LoopEngine::Event::InitEvent;
using LoopEngine::Event::DrawEvent;
using LoopEngine::Event::QuitEvent;
using LoopEngine::Event::UpdateEvent;
using LoopEngine::Event::EventHandler;

// todo: rewrite this
auto main(int argc, char** argv) -> int {
    try {
        // todo: parse command line args
        Display display{800, 600, "Demo"};
        Context context{};
        Graphics graphics{};
        InputSystem input_system{};
        EventSystem event_system{};
        CameraSystem camera_system{};

        input_system.load_config("input.yaml");

        event_system.send_event(InitEvent{});

        float time_since_start = 0.0f;
        auto start_time = std::chrono::high_resolution_clock::now();
        while (!display.should_close()) {
            auto current_time = std::chrono::high_resolution_clock::now();
            auto delta_time = std::chrono::duration<float, std::chrono::seconds::period>(current_time - start_time).count();
            start_time = current_time;
            time_since_start += delta_time;

            display.poll_events();

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

            event_system.send_event(DrawEvent{});

            cmd.endRenderPass();

            result = graphics.submit_frame();
            if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR) {
                continue;
            }
        }

        context.device.waitIdle();

        event_system.send_event(QuitEvent{});

        return EXIT_SUCCESS;
    } catch (const std::exception& e) {
        spdlog::error(e.what());
        return EXIT_FAILURE;
    }
}