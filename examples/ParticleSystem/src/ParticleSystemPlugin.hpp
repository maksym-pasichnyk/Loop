#pragma once

#include <vector>
#include <random>

#include "LoopEngine/Event/EventSystem.hpp"
#include "LoopEngine/Input/InputSystem.hpp"

#include "ParticleSystem.hpp"

using LoopEngine::Event::InitEvent;
using LoopEngine::Event::DrawEvent;
using LoopEngine::Event::UpdateEvent;
using LoopEngine::Event::EventHandler;
using LoopEngine::Input::ButtonPressEvent;

struct ImGuiDrawEvent;
struct FireworkParticleSystem;
struct ParticleSystemPlugin {
    ParticleSystemPlugin();
    ~ParticleSystemPlugin();

private:
    void init(const InitEvent& event);
    void update(const UpdateEvent& event);
    void draw(const DrawEvent& event);

    void update_camera(const UpdateEvent& event) const;

private:
    void on_press_button(const ButtonPressEvent& event);
    void on_imgui_draw(const ImGuiDrawEvent& event);

private:
    bool lock_mouse = false;

    std::shared_ptr<FireworkParticleSystem> firework_particle_system;

    EventHandler<InitEvent> init_event_handler{};
    EventHandler<DrawEvent> draw_event_handler{};
    EventHandler<UpdateEvent> update_event_handler{};
    EventHandler<ImGuiDrawEvent> imgui_draw_event_handler{};
    EventHandler<ButtonPressEvent> press_button_event_handler{};
};