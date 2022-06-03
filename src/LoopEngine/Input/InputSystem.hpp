#pragma once

#include <set>
#include <array>

#include "LoopEngine/Core/Singleton.hpp"
#include "InputController.hpp"

#include "glm/vec2.hpp"

namespace LoopEngine::Input {
    // todo: send events to EventSystem
    struct InputSystem final : LoopEngine::Core::Singleton<InputSystem> {
        void save_config(const std::string &filename);
        void load_config(const std::string &filename);
        void add_listener(InputController *controller);
        void remove_listener(InputController *controller);
        auto get_axis(const std::string &name) const -> float;
        auto get_button(const std::string &name) const -> bool;
        auto get_mouse_delta() const -> const glm::vec2 &;
        void update(float dt);

    private:
        glm::vec2 mouse_delta{0.0f, 0.0f};
        glm::vec2 mouse_position{0.0f, 0.0f};
        glm::vec2 prev_mouse_position{0.0f, 0.0f};

        std::set<InputController *> controllers{};

        std::unordered_map<std::string, KeyCode> button_bindings{};
        std::unordered_map<std::string, AxisBinding> axis_bindings{};

        // button state
        std::array<ButtonState, 512> button_states{};
    };
}