#pragma once

#include "LoopEngine/Core/Singleton.hpp"

#include <string>
#include <unordered_map>

namespace LoopEngine::Asset {
    struct AssetSystem : LoopEngine::Core::Singleton<AssetSystem> {
        AssetSystem();
        auto read(const std::string &filename) -> std::string;

    private:
        struct AssetInfo {
            size_t offset;
            size_t size;
        };

        std::unordered_map<std::string, AssetInfo> assets{};
    };

    extern auto read_file_from_assets(const std::string &filename) -> std::string;
}