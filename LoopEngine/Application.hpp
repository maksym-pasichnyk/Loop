#pragma once

#include "Platform/Window.hpp"
#include "Graphics/Context.hpp"
#include "Graphics/Graphics.hpp"
#include "Input/InputSystem.hpp"
#include "Event/EventSystem.hpp"
#include "Camera/CameraSystem.hpp"
#include "Asset/AssetSystem.hpp"

namespace LoopEngine {
    struct Application final : public LoopEngine::Core::Singleton<Application> {
        Application(const char *title, int width, int height);
        ~Application();

        void run();
        auto get_window() -> LoopEngine::Platform::Window&;

    private:
        LoopEngine::Platform::Window window;
        LoopEngine::Graphics::Context context{};
        LoopEngine::Graphics::Graphics graphics{context};
        LoopEngine::Asset::AssetSystem asset_system{};
        LoopEngine::Input::InputSystem input_system{};
        LoopEngine::Event::EventSystem event_system{};
        LoopEngine::Camera::CameraSystem camera_system{};
    };
}