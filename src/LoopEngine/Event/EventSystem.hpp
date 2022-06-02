#pragma once

#include "LoopEngine/Core/Singleton.hpp"

namespace LoopEngine::Event {
    struct InitEvent {};
    struct UpdateEvent {
        float deltaTime;
    };

    struct DrawEvent {};
    struct QuitEvent {};

    template<typename T>
    struct EventHandler {
        using Event = T;
        static void handle(const Event& event);
    };

    struct EventSystem final : LoopEngine::Core::Singleton<EventSystem> {
        template<typename T>
        void send_event(const T& event) {
            static_cast<void(*)(const T& event)>(EventHandler<T>::handle)(event);
        }
    };
}