#pragma once

#include "Camera.hpp"
#include "LoopEngine/Core/Singleton.hpp"

namespace LoopEngine::Camera {
    struct CameraSystem final : LoopEngine::Core::Singleton<CameraSystem> {
        friend auto get_default_camera() -> Camera*;
    private:
        Camera camera;
    };

    extern auto get_default_camera() -> Camera*;
}
