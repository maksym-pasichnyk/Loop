#pragma once

#include "Delegate.hpp"

namespace LoopEngine::Event {
    struct IEventHandler {
        virtual void handle(void *payload) = 0;
    };

    template<typename T>
    struct EventHandler : IEventHandler {
        using Event = T;

        template<auto function, typename Self>
        constexpr auto connect(Self *object) noexcept {
            delegate = Delegate{object, as_static_function<function>()};
        }
        constexpr auto connect(void(*function)(const Event&)) noexcept {
            delegate = Delegate{function};
        }

        template<typename Self>
        constexpr auto connect(Self *object, void(*function)(Self*, const Event&)) noexcept {
            delegate = Delegate{object, function};
        }

        constexpr void reset() noexcept {
            delegate = {};
        }

    private:
        void handle(void *payload) override {
            if (delegate) {
                delegate(*static_cast<const Event*>(payload));
            }
        }

        Delegate<void(const Event&)> delegate;
    };
}