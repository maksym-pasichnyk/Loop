#include "EventSystem.hpp"

using LoopEngine::Core::Singleton;
using LoopEngine::Event::EventSystem;

template<> EventSystem* Singleton<EventSystem>::instance = nullptr;
