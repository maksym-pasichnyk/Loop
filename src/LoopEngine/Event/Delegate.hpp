#pragma once

namespace LoopEngine::Event {
    template<typename T>
    struct Delegate;

    template<typename R, typename ...T>
    struct Delegate<R(T...)> {
        using FunctionType = R(*)(void *, T...);

        constexpr Delegate() = default;

        template<typename Self>
        constexpr Delegate(Self *object, R(*function)(Self*, T...)) noexcept
        : object(object), function((FunctionType) function) {}

        constexpr Delegate(R(*function)(T...)) noexcept
        : object((void*) function), function(static_invoke) {}

        template<class... Args>
        auto operator()(Args&&... args) -> R {
            return function(object, static_cast<Args&&>(args)...);
        }

    private:
        static auto static_invoke(void *object, T... args) -> R {
            return (*(FunctionType *) object)(object, static_cast<T&&>(args)...);
        }

        void *object = nullptr;
        FunctionType function = nullptr;
    };

    template<typename R, typename ...T>
    Delegate(R(*)(T...)) -> Delegate<R(T...)>;

    template<typename Self, typename R, typename ...T>
    Delegate(Self*, R(*)(Self*, T...)) -> Delegate<R(T...)>;

    template<auto fn, typename Self, typename R, typename... Args>
    inline constexpr auto as_static_function(auto(Self::*)(Args...) -> R) noexcept -> auto(*)(Self*, Args...) -> R {
        return [](Self* self, Args... args) -> R {
            return (self->*fn)(args...);
        };
    }

    template <auto fn>
    inline constexpr auto as_static_function() noexcept {
        return as_static_function<fn>(fn);
    }

    template<auto function, typename Self>
    static auto create_delegate(Self *object) noexcept {
        return Delegate{object, as_static_function<function>()};
    }
    template<typename R, typename ...T>
    static auto create_delegate(R(*function)(T...)) noexcept {
        return Delegate{function};
    }

    template<typename Self, typename R, typename ...T>
    static auto create_delegate(Self *object, R(*function)(Self*, T...)) noexcept {
        return Delegate{object, function};
    }
}