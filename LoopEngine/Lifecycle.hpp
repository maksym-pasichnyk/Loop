#pragma once

#include "Event/EventSystem.hpp"

namespace LoopEngine {
    template<typename Self>
    struct Lifecycle {
        Lifecycle() {
            if constexpr(requires(Self self) { self.on_create(); }) {
                init_event_handler.connect(this, +[](Lifecycle* self, const LoopEngine::Event::InitEvent& event) {
                    static_cast<Self*>(self)->on_create();
                });
                LoopEngine::Event::EventSystem::get_global_event_queue()->add_event_handler(&init_event_handler);
            }
            if constexpr(requires(Self self) { self.on_destroy(); }) {
                quit_event_handler.connect(this, +[](Lifecycle* self, const LoopEngine::Event::QuitEvent& event) {
                    static_cast<Self*>(self)->on_destroy();
                });
                LoopEngine::Event::EventSystem::get_global_event_queue()->add_event_handler(&quit_event_handler);
            }
            if constexpr(requires(Self self, float dt) { self.on_update(dt); }) {
                update_event_handler.connect(this, +[](Lifecycle* self, const LoopEngine::Event::UpdateEvent& event) {
                    static_cast<Self*>(self)->on_update(event.dt);
                });
                LoopEngine::Event::EventSystem::get_global_event_queue()->add_event_handler(&update_event_handler);
            }
            if constexpr(requires(Self self, vk::CommandBuffer cmd) { self.on_before_draw(cmd); }) {
                before_draw_event_handler.connect(this, +[](Lifecycle* self, const LoopEngine::Event::BeforeDrawEvent& event) {
                    static_cast<Self*>(self)->on_before_draw(event.cmd);
                });
                LoopEngine::Event::EventSystem::get_global_event_queue()->add_event_handler(&before_draw_event_handler);
            }
            if constexpr(requires(Self self, vk::CommandBuffer cmd) { self.on_draw(cmd); }) {
                draw_event_handler.connect(this, +[](Lifecycle* self, const LoopEngine::Event::DrawEvent& event) {
                    static_cast<Self*>(self)->on_draw(event.cmd);
                });
                LoopEngine::Event::EventSystem::get_global_event_queue()->add_event_handler(&draw_event_handler);
            }
            if constexpr(requires(Self self, vk::CommandBuffer cmd) { self.on_after_draw(cmd); }) {
                after_draw_event_handler.connect(this, +[](Lifecycle* self, const LoopEngine::Event::AfterDrawEvent& event) {
                    static_cast<Self*>(self)->on_after_draw(event.cmd);
                });
                LoopEngine::Event::EventSystem::get_global_event_queue()->add_event_handler(&after_draw_event_handler);
            }
        }
        ~Lifecycle() {
            if constexpr(requires(Self self) { self.on_create(); }) {
                LoopEngine::Event::EventSystem::get_global_event_queue()->remove_event_handler(&init_event_handler);
            }
            if constexpr(requires(Self self, float dt) { self.on_update(dt); }) {
                LoopEngine::Event::EventSystem::get_global_event_queue()->remove_event_handler(&quit_event_handler);
            }
            if constexpr(requires(Self self, vk::CommandBuffer cmd) { self.on_before_draw(cmd); }) {
                LoopEngine::Event::EventSystem::get_global_event_queue()->remove_event_handler(&before_draw_event_handler);
            }
            if constexpr(requires(Self self, vk::CommandBuffer cmd) { self.on_draw(cmd); }) {
                LoopEngine::Event::EventSystem::get_global_event_queue()->remove_event_handler(&draw_event_handler);
            }
            if constexpr(requires(Self self, vk::CommandBuffer cmd) { self.on_after_draw(cmd); }) {
                LoopEngine::Event::EventSystem::get_global_event_queue()->remove_event_handler(&after_draw_event_handler);
            }
            if constexpr(requires(Self self) { self.on_destroy(); }) {
                LoopEngine::Event::EventSystem::get_global_event_queue()->remove_event_handler(&update_event_handler);
            }
        }

    private:
        LoopEngine::Event::EventHandler<LoopEngine::Event::InitEvent> init_event_handler{};
        LoopEngine::Event::EventHandler<LoopEngine::Event::QuitEvent> quit_event_handler{};
        LoopEngine::Event::EventHandler<LoopEngine::Event::BeforeDrawEvent> before_draw_event_handler{};
        LoopEngine::Event::EventHandler<LoopEngine::Event::DrawEvent> draw_event_handler{};
        LoopEngine::Event::EventHandler<LoopEngine::Event::AfterDrawEvent> after_draw_event_handler{};
        LoopEngine::Event::EventHandler<LoopEngine::Event::UpdateEvent> update_event_handler{};
    };
}