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

struct ParticleSystemPlugin {
    ParticleSystemPlugin();
    ~ParticleSystemPlugin();

private:
    void init(const InitEvent& event);
    void update(const UpdateEvent& event);
    void draw(const DrawEvent& event);

    void update_camera(const UpdateEvent& event);

private:
    void create_imgui_context();
    void destroy_imgui_context();

    void on_press_button(const ButtonPressEvent& event);

    void on_particle_system_update(const ParticleSystemUpdateEvent& event);
    void on_sub_particle_system_update(const ParticleSystemUpdateEvent& event);

private:
    bool lock_mouse = false;

    float emit_rate = 2.5f;
    float emit_delay = 0.0f;

    std::default_random_engine generator{};
    std::shared_ptr<ParticleSystem> particle_system{};
    std::shared_ptr<ParticleSystem> sub_particle_system{};
    EventHandler<ParticleSystemUpdateEvent> particle_system_update_handler{};
    EventHandler<ParticleSystemUpdateEvent> sub_particle_system_update_handler{};

    EventHandler<InitEvent> init_event_handler{};
    EventHandler<DrawEvent> draw_event_handler{};
    EventHandler<UpdateEvent> update_event_handler{};

    EventHandler<ButtonPressEvent> press_button_event_handler{};

    // ImGui
    vk::DescriptorPool descriptor_pool{};
};