#pragma once

#include <vector>
#include <random>

#include "LoopEngine/Lifecycle.hpp"
#include "LoopEngine/Event/EventSystem.hpp"
#include "LoopEngine/Input/InputSystem.hpp"

#include "ParticleSystem.hpp"
#include "ImGuiPlugin.hpp"

using LoopEngine::Event::InitEvent;
using LoopEngine::Event::DrawEvent;
using LoopEngine::Event::UpdateEvent;
using LoopEngine::Event::EventHandler;
using LoopEngine::Input::ButtonPressEvent;

struct ImGuiDrawEvent;
struct FireworkParticleSystem;
struct ParticleSystemExample : LoopEngine::Lifecycle<ParticleSystemExample> {
    friend Lifecycle;

    ParticleSystemExample();
    ~ParticleSystemExample();

private:
    void on_create();
    void on_update(float dt);
    void on_draw(vk::CommandBuffer cmd);

    void update_camera(float dt) const;

private:
    void on_press_button(const ButtonPressEvent& event);
    void on_imgui_draw(const ImGuiDrawEvent& event);

private:
    bool lock_mouse = false;

    ImGuiPlugin imgui_plugin{};
    EventHandler<ImGuiDrawEvent> imgui_draw_event_handler{};
    EventHandler<ButtonPressEvent> press_button_event_handler{};
    std::shared_ptr<FireworkParticleSystem> firework_particle_system{};
};