#pragma once

#include "LoopEngine/Event/EventSystem.hpp"
#include "LoopEngine/Event/EventHandler.hpp"

using LoopEngine::Event::InitEvent;
using LoopEngine::Event::UpdateEvent;
using LoopEngine::Event::AfterDrawEvent;
using LoopEngine::Event::QuitEvent;
using LoopEngine::Event::EventHandler;

struct ImGuiDrawEvent {};

struct ImGuiPlugin {
    ImGuiPlugin();
    ~ImGuiPlugin();

private:
    void init(const InitEvent& event);
    static void update(const UpdateEvent& event);
    static void draw(const AfterDrawEvent& event);
    void quit(const QuitEvent& event);

private:
    EventHandler<InitEvent> init_event_handler{};
    EventHandler<UpdateEvent> update_event_handler{};
    EventHandler<AfterDrawEvent> draw_event_handler{};
    EventHandler<QuitEvent> quit_event_handler{};

    // ImGui
    vk::DescriptorPool descriptor_pool{};
};