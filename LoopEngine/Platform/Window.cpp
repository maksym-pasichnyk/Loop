#include "Window.hpp"

using LoopEngine::Core::Singleton;
using LoopEngine::Platform::Window;

Window::Window(int width, int height, const char *title) {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    handle = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (handle == nullptr) {
        throw std::runtime_error("Failed to create window");
    }
}

Window::~Window() {
    glfwDestroyWindow((GLFWwindow*) handle);
    glfwTerminate();
}

auto Window::should_close() -> bool {
    return glfwWindowShouldClose((GLFWwindow*) handle);
}

void Window::poll_events() {
    glfwPollEvents();
}

auto Window::get_native_handle() -> GLFWwindow * {
    return (GLFWwindow*) handle;
}