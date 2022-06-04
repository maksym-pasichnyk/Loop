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
    struct BeforeDrawEvent {
        vk::CommandBuffer cmd;
    };
    struct DrawEvent {
        vk::CommandBuffer cmd;
    };
    struct AfterDrawEvent {
        vk::CommandBuffer cmd;
    };
    struct QuitEvent {};

    struct EventSystem {
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
            remove_event_handler(handler);
            handlers[typeid(typename Self::Event).hash_code()].emplace_back(handler);
        }

        template<typename Self> requires std::is_base_of_v<IEventHandler, Self>
        void remove_event_handler(Self* handler) {
            auto it = handlers.find(typeid(typename Self::Event).hash_code());
            if (it != handlers.end()) {
                it->second.erase(std::remove(it->second.begin(), it->second.end(), handler), it->second.end());
            }
        }

    private:
        std::unordered_map<size_t, std::vector<IEventHandler*>> handlers;
    };

    extern auto get_global_event_queue() -> EventSystem*;
}