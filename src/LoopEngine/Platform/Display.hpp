#pragma once

#include "LoopEngine/Core/Singleton.hpp"
#include "GLFW/glfw3.h"

namespace LoopEngine::Platform {
    struct Display final : LoopEngine::Core::Singleton<Display> {
        Display(int width, int height, const char *title);
        ~Display();

        auto should_close() -> bool;
        void poll_events();

        static auto get_native_window_handle() -> GLFWwindow*;

    private:
        GLFWwindow* handle;
    };
}