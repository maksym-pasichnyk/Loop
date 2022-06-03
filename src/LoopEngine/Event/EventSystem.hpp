#pragma once

#include "EventHandler.hpp"
#include "LoopEngine/Core/Singleton.hpp"

#include <set>
#include <unordered_map>
#include <vulkan/vulkan.hpp>

namespace LoopEngine::Event {
    struct InitEvent {};
    struct UpdateEvent {
        float dt;
    };

    struct DrawEvent {
        vk::CommandBuffer cmd;
    };
    struct QuitEvent {};

    struct EventSystem final : LoopEngine::Core::Singleton<EventSystem> {
        template<typename Event>
        void send_event(const Event& event) {
            auto it = handlers.find(typeid(Event).hash_code());
            if (it != handlers.end()) {
                for (auto& handler : it->second) {
                    handler->handle((void*) std::addressof(event));
                }
            }
        }

        template<typename Self> requires std::is_base_of_v<IEventHandler, Self>
        void add_event_handler(Self* handler) {
            handlers[typeid(typename Self::Event).hash_code()].emplace(handler);
        }

        template<typename Self> requires std::is_base_of_v<IEventHandler, Self>
        void remove_event_handler(Self* handler) {
            handlers.at(typeid(typename Self::Event).hash_code()).erase(handler);
        }

    private:
        std::unordered_map<size_t, std::set<IEventHandler*>> handlers;
    };
}