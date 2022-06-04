#pragma once

#include "LoopEngine/Platform/Display.hpp"
#include "LoopEngine/Graphics/Context.hpp"
#include "LoopEngine/Graphics/Graphics.hpp"
#include "LoopEngine/Input/InputSystem.hpp"
#include "LoopEngine/Event/EventSystem.hpp"
#include "LoopEngine/Camera/CameraSystem.hpp"
#include "LoopEngine/Asset/AssetSystem.hpp"

namespace LoopEngine {
    struct Application : LoopEngine::Core::Singleton<Application> {
        Application(const char *title, int width, int height);
        void run();

        auto get_event_system() -> LoopEngine::Event::EventSystem* {
            return &event_system;
        }

        template<typename T>
        void add_plugin() {
            plugins.emplace_back(std::make_shared<T>());
        }

    private:
        LoopEngine::Platform::Display display;
        LoopEngine::Graphics::Context context{};
        LoopEngine::Graphics::Graphics graphics{};
        LoopEngine::Asset::AssetSystem asset_system{};
        LoopEngine::Input::InputSystem input_system{};
        LoopEngine::Event::EventSystem event_system{};
        LoopEngine::Camera::CameraSystem camera_system{};
        std::vector<std::shared_ptr<void>> plugins{};
    };
}