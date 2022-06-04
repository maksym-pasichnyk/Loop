#include "EventSystem.hpp"
#include "LoopEngine/Application.hpp"

using LoopEngine::Application;
using LoopEngine::Core::Singleton;

auto LoopEngine::Event::get_global_event_queue() -> EventSystem* {
    return Application::get_instance()->get_event_system();
}
