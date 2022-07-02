#pragma once

namespace LoopEngine::Core {
    struct DisableCopyAndMove {
        DisableCopyAndMove() = default;
        ~DisableCopyAndMove() = default;

        DisableCopyAndMove(const DisableCopyAndMove&) = delete;
        DisableCopyAndMove& operator=(const DisableCopyAndMove&) = delete;

        DisableCopyAndMove(DisableCopyAndMove&&) = delete;
        DisableCopyAndMove& operator=(DisableCopyAndMove&&) = delete;
    };
}