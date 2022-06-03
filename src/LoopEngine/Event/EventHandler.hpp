#pragma once

#include "LoopEngine/Event/Delegate.hpp"

namespace LoopEngine::Event {
    struct IEventHandler {
        virtual void handle(void *payload) = 0;
    };

    template<typename T>
    struct EventHandler : IEventHandler {
        using Event = T;

        constexpr EventHandler() noexcept = default;
        constexpr EventHandler(Delegate<void(const Event&)> delegate) noexcept : delegate(delegate) {}

    private:
        void handle(void *payload) override {
            if (delegate) {
                delegate(static_cast<const Event&>(*static_cast<const T*>(payload)));
            }
        }

        Delegate<void(const Event&)> delegate;
    };
}