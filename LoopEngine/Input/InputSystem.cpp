#include "InputSystem.hpp"
#include "LoopEngine/Application.hpp"
#include "LoopEngine/Platform/Window.hpp"
#include "LoopEngine/Asset/AssetSystem.hpp"
#include "LoopEngine/Event/EventSystem.hpp"

#include "spdlog/spdlog.h"
#include "yaml-cpp/yaml.h"

#include <fstream>

using LoopEngine::Application;
using LoopEngine::Core::Singleton;
using LoopEngine::Platform::Window;
using LoopEngine::Input::InputSystem;
using LoopEngine::Event::EventSystem;
using LoopEngine::Asset::AssetSystem;

template<> InputSystem* Singleton<InputSystem>::instance = nullptr;

void InputSystem::save_config(const std::string &filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        spdlog::error("Failed to open file {}", filename);
        return;
    }

    YAML::Emitter out;
    out << YAML::BeginSeq;
    for (auto&& [binding, button] : button_bindings) {
        out << YAML::BeginMap;
        out << YAML::Key << "type" << YAML::Value << "button";
        out << YAML::Key << "name" << YAML::Value << binding;
        out << YAML::Key << "keycode" << YAML::Value << int(button);
        out << YAML::EndMap;
    }
    for (auto&& [binding, axis] : axis_bindings) {
        out << YAML::BeginMap;
        out << YAML::Key << "type" << YAML::Value << "axis";
        out << YAML::Key << "name" << YAML::Value << binding;
        out << YAML::Key << "positive" << YAML::Value << int(axis.positive_key);
        out << YAML::Key << "negative" << YAML::Value << int(axis.negative_key);
        out << YAML::EndMap;
    }
    out << YAML::EndSeq;

    file << out.c_str();
}

void InputSystem::load_config(const std::string &filename) {
    auto data = AssetSystem::read_file_from_assets(filename);
    if (data.empty()) {
        return;
    }

    auto config = YAML::Load(data);
    if (config.IsNull()) {
        return;
    }
    if (!config.IsSequence()) {
        spdlog::error("Failed to parse file {}", filename);
        return;
    }

    for (auto&& it : config) {
        auto type = it["type"].as<std::string>();
        if (type == "button") {
            auto name = it["name"].as<std::string>();
            auto keycode = it["keycode"].as<int>();
            button_bindings.insert_or_assign(name, keycode);
        } else if (type == "axis") {
            auto name = it["name"].as<std::string>();
            auto positive = it["positive"].as<int>();
            auto negative = it["negative"].as<int>();
            axis_bindings.insert_or_assign(name, Axis2D{positive, negative});
        } else {
            spdlog::error("Unknown binding type {}", type);
        }
    }
}

auto InputSystem::get_axis(const std::string &name) const -> float {
    auto it = axis_bindings.find(name);
    if (it == axis_bindings.end()) {
        return 0.0f;
    }
    return it->second.positive_value - it->second.negative_value;
}

auto InputSystem::get_button(const std::string &name) const -> bool {
    auto it = button_bindings.find(name);
    if (it == button_bindings.end()) {
        return false;
    }
    return button_states[size_t(it->second)] == ButtonState::Pressed;
}

auto InputSystem::get_mouse_delta() const -> const glm::vec2 & {
    return mouse_delta;
}

void InputSystem::update(float dt) {
    auto window = Application::get_instance()->get_window().get_native_handle();

    double x, y;
    glfwGetCursorPos(window, &x, &y);

    prev_mouse_position = mouse_position;
    mouse_position.x = static_cast<float>(x);
    mouse_position.y = static_cast<float>(y);
    mouse_delta = mouse_position - prev_mouse_position;

    for (auto& [binding, button] : button_bindings) {
        if (glfwGetKey(window, button) == GLFW_PRESS) {
            if (button_states[button] == ButtonState::Released) {
                button_states[button] = ButtonState::Pressed;

                EventSystem::get_global_event_queue()->send_event(ButtonPressEvent{binding});
            }
        } else {
            if (button_states[button] == ButtonState::Pressed) {
                button_states[button] = ButtonState::Released;

                EventSystem::get_global_event_queue()->send_event(ButtonReleaseEvent{binding});
            }
        }
    }

    for (auto& [binding, axis] : axis_bindings) {
        bool flag = false;
        if (glfwGetKey(window, int(axis.positive_key)) == GLFW_PRESS) {
            flag = true;
            if (axis.positive_value < 1.0f) {
                axis.positive_value += dt * 10.0f;
                axis.positive_value = std::min(axis.positive_value, 1.0f);
            }
        } else {
            if (axis.positive_value > 0.0f) {
                flag = true;
                axis.positive_value -= dt * 10.0f;
                axis.positive_value = std::max(axis.positive_value, 0.0f);
            }
        }

        if (glfwGetKey(window, int(axis.negative_key)) == GLFW_PRESS) {
            flag = true;
            if (axis.negative_value < 1.0f) {
                axis.negative_value += dt * 10.0f;
                axis.negative_value = std::min(axis.negative_value, 1.0f);
            }
        } else {
            if (axis.negative_value > 0.0f) {
                flag = true;
                axis.negative_value -= dt * 10.0f;
                axis.negative_value = std::max(axis.negative_value, 0.0f);
            }
        }

        if (flag) {
            auto value = axis.positive_value - axis.negative_value;
            EventSystem::get_global_event_queue()->send_event(AxisEvent{binding, value});
        }
    }
}
