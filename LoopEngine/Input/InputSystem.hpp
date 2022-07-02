#pragma once

#include <set>
#include <array>
#include <unordered_map>

#include "LoopEngine/Core/Singleton.hpp"

#include "glm/vec2.hpp"

namespace LoopEngine::Input {
    enum class MouseButton {};

    enum class ButtonState {
        Released,
        Pressed
    };

    struct Axis2D {
        int positive_key{};
        int negative_key{};

        float positive_value = 0.0f;
        float negative_value = 0.0f;
    };

    struct ButtonPressEvent {
        std::string_view button;
    };

    struct ButtonReleaseEvent {
        std::string_view button;
    };

    struct AxisEvent {
        std::string_view axis;
        float value;
    };

    struct InputSystem final : LoopEngine::Core::Singleton<InputSystem> {
        void save_config(const std::string &filename);
        void load_config(const std::string &filename);
//        void add_listener(InputController *controller);
//        void remove_listener(InputController *controller);
        auto get_axis(const std::string &name) const -> float;
        auto get_button(const std::string &name) const -> bool;
        auto get_mouse_delta() const -> const glm::vec2 &;
        void update(float dt);

    private:
        glm::vec2 mouse_delta{0.0f, 0.0f};
        glm::vec2 mouse_position{0.0f, 0.0f};
        glm::vec2 prev_mouse_position{0.0f, 0.0f};

        std::unordered_map<std::string, int> button_bindings{};
        std::unordered_map<std::string, Axis2D> axis_bindings{};

        // button state
        std::array<ButtonState, 512> button_states{};
    };
}