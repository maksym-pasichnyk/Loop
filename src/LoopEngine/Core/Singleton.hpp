#pragma once

#include <stdexcept>
#include "spdlog/fmt/fmt.h"

#include "DisableCopyAndMove.hpp"

namespace LoopEngine::Core {
    template<typename T>
    struct Singleton : private DisableCopyAndMove {
        Singleton() {
            if (instance != nullptr) {
                throw std::runtime_error(fmt::format("Singleton of type {} already initialized!", typeid(T).name()));
            }
            instance = static_cast<T *>(this);
        }

        ~Singleton() {
            instance = nullptr;
        }

        static auto get_instance() -> T * {
            return instance;
        }

    private:
        static T *instance;
    };
}