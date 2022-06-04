#pragma once

#include <vector>
#include <random>

#include "LoopEngine/Event/EventSystem.hpp"

using LoopEngine::Event::InitEvent;
using LoopEngine::Event::DrawEvent;
using LoopEngine::Event::UpdateEvent;
using LoopEngine::Event::EventHandler;

struct ParticleSystem;
struct ParticleSystemPlugin {
    ParticleSystemPlugin();
    ~ParticleSystemPlugin();

private:
    void init(const InitEvent& event);
    void update(const UpdateEvent& event);
    void draw(const DrawEvent& event);

private:
    std::default_random_engine generator{};
    std::shared_ptr<ParticleSystem> particle_system{};

    EventHandler<InitEvent> init_event_handler;
    EventHandler<DrawEvent> draw_event_handler;
    EventHandler<UpdateEvent> update_event_handler;
};