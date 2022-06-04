#pragma once

#include <string>
#include <unordered_map>

namespace LoopEngine::Input {
    enum class MouseButton {};

    enum class ButtonState {
        Released,
        Pressed
    };

    struct AxisBinding {
        int positive_key{};
        int negative_key{};

        float positive_value = 0.0f;
        float negative_value = 0.0f;
    };

    struct InputController {
        friend struct InputSystem;

        void BindAction(const std::string& name, ButtonState state, std::function<void()> callback) {
            if (state == ButtonState::Released) {
                release_callbacks[name] = std::move(callback);
            } else {
                press_callbacks[name] = std::move(callback);
            }
        }

        void BindAxis(const std::string& name, std::function<void(float)> callback) {
            axis_callbacks[name] = std::move(callback);
        }

    private:
        void ExecuteAction(const std::string& name, ButtonState state) {
            if (state == ButtonState::Released) {
                auto it = release_callbacks.find(name);
                if (it != release_callbacks.end()) {
                    it->second();
                }
            } else {
                auto it = press_callbacks.find(name);
                if (it != press_callbacks.end()) {
                    it->second();
                }
            }
        }

        void ExecuteAxis(const std::string& name, float value) {
            auto it = axis_callbacks.find(name);
            if (it != axis_callbacks.end()) {
                it->second(value);
            }
        }

    private:
        std::unordered_map<std::string, std::function<void()>> release_callbacks;
        std::unordered_map<std::string, std::function<void()>> press_callbacks;
        std::unordered_map<std::string, std::function<void(float)>> axis_callbacks;
    };
}