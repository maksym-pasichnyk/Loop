#pragma once

#include "LoopEngine/Core/Singleton.hpp"

#include <string>
#include <unordered_map>

namespace LoopEngine {
    struct Application;
}

namespace LoopEngine::Asset {
    struct AssetSystem : LoopEngine::Core::Singleton<AssetSystem> {
        friend LoopEngine::Application;

        static auto read_file_from_assets(const std::string &filename) -> std::string;

    private:
        auto initialize() -> bool;
        void terminate();

        struct AssetInfo {
            size_t offset;
            size_t size;
        };

        std::unordered_map<std::string, AssetInfo> assets{};
    };
}