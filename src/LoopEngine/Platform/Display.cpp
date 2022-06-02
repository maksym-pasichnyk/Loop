#include "Display.hpp"

using LoopEngine::Core::Singleton;
using LoopEngine::Platform::Display;

template<> Display* Singleton<Display>::instance = nullptr;

Display::Display(int width, int height, const char *title) {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    handle = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (handle == nullptr) {
        throw std::runtime_error("Failed to create window");
    }
}

Display::~Display() {
    glfwDestroyWindow((GLFWwindow*) handle);
    glfwTerminate();
}

auto Display::should_close() -> bool {
    return glfwWindowShouldClose((GLFWwindow*) handle);
}

void Display::poll_events() {
    glfwPollEvents();
}

auto Display::get_native_window_handle() -> GLFWwindow* {
    return get_instance()->handle;
}
