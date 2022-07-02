#pragma once

#include "LoopEngine/Core/Singleton.hpp"
#include "GLFW/glfw3.h"

namespace LoopEngine::Platform {
    struct Window final {
        Window(int width, int height, const char *title);
        ~Window();

        auto get_native_handle() -> GLFWwindow*;
        auto should_close() -> bool;
        void poll_events();

    private:
        GLFWwindow* handle;
    };
}