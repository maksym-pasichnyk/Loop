#pragma once

#include "LoopEngine/Lifecycle.hpp"
#include "LoopEngine/Event/EventSystem.hpp"
#include "LoopEngine/Event/EventHandler.hpp"

using LoopEngine::Event::InitEvent;
using LoopEngine::Event::UpdateEvent;
using LoopEngine::Event::AfterDrawEvent;
using LoopEngine::Event::QuitEvent;
using LoopEngine::Event::EventHandler;

struct ImGuiDrawEvent {};

struct ImGuiPlugin : LoopEngine::Lifecycle<ImGuiPlugin> {
    friend Lifecycle;

private:
    void on_create();
    void on_update(float dt);
    void on_after_draw(vk::CommandBuffer cmd);
    void on_destroy();

private:
    vk::DescriptorPool imgui_descriptor_pool{};
};