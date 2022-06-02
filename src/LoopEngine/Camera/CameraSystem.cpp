#include "CameraSystem.hpp"

using LoopEngine::Core::Singleton;
using LoopEngine::Camera::Camera;
using LoopEngine::Camera::CameraSystem;

template<> CameraSystem* Singleton<CameraSystem>::instance = nullptr;

auto LoopEngine::Camera::get_default_camera() -> Camera * {
    return std::addressof(CameraSystem::get_instance()->camera);
}
