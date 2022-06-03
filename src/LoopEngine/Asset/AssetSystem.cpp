#include "AssetSystem.hpp"

#include "spdlog/spdlog.h"
#include "yaml-cpp/yaml.h"

#include <fstream>

using LoopEngine::Core::Singleton;
using LoopEngine::Asset::AssetSystem;

template<> AssetSystem* Singleton<AssetSystem>::instance = nullptr;

AssetSystem::AssetSystem() {
    auto config = YAML::LoadFile("assets.yaml");
    for (auto&& node : config) {
        auto offset = node.second["offset"].as<size_t>();
        auto size = node.second["size"].as<size_t>();

        assets.emplace(node.first.as<std::string>(), AssetInfo{offset, size});
    }
}

auto LoopEngine::Asset::AssetSystem::read(const std::string &filename) -> std::string {
    auto it = assets.find(filename);
    if (it == assets.end()) {
        spdlog::error("Failed to find asset {}", filename);
        return "";
    }

    auto& info = it->second;
    std::ifstream file("assets.bin", std::ios::binary);

    file.seekg(std::streamoff(info.offset));
    std::string data(info.size, '\0');
    file.read(data.data(), std::streamsize(info.size));

    return data;
}

auto LoopEngine::Asset::read_file_from_assets(const std::string &filename) -> std::string {
    return AssetSystem::get_instance()->read(filename);
}
