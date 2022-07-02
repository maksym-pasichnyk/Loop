#pragma once

#include <stdexcept>
#include "spdlog/fmt/fmt.h"

#include "DisableCopyAndMove.hpp"

namespace LoopEngine::Core {
    template<typename T>
    struct Singleton : private DisableCopyAndMove {
        Singleton() {
            static_assert(std::is_base_of<Singleton, T>::value, "T must be a subclass of Singleton");
            if (instance != nullptr) {
                throw std::runtime_error(fmt::format("Singleton of type {} already initialized!", typeid(T).name()));
            }
            instance = static_cast<T *>(this);
        }

        ~Singleton() {
            instance = nullptr;
        }

    public:
        static auto get_instance() -> T * {
            return instance;
        }

    private:
        static T *instance;
    };
}