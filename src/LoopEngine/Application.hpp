#pragma once

#include "LoopEngine/Platform/Display.hpp"
#include "LoopEngine/Graphics/Context.hpp"
#include "LoopEngine/Graphics/Graphics.hpp"
#include "LoopEngine/Input/InputSystem.hpp"
#include "LoopEngine/Event/EventSystem.hpp"
#include "LoopEngine/Camera/CameraSystem.hpp"

namespace LoopEngine {
    struct Application {
        Application(const char *title, int width, int height);
        void run();

    private:
        LoopEngine::Platform::Display display;
        LoopEngine::Graphics::Context context{};
        LoopEngine::Graphics::Graphics graphics{};
        LoopEngine::Input::InputSystem input_system{};
        LoopEngine::Event::EventSystem event_system{};
        LoopEngine::Camera::CameraSystem camera_system{};
    };
}