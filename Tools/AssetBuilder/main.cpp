#include <unordered_map>
#include <filesystem>
#include <fstream>
#include <array>

#include "yaml-cpp/yaml.h"
#include "spdlog/spdlog.h"

auto execute(const std::string& cmd, std::string& output) -> int {
    auto pipe = popen(cmd.c_str(), "r");
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    std::array<char, 128> buffer{};
    size_t count;
    while ((count = fread(buffer.data(), 1, 128, pipe)) > 0) {
        output.append(buffer.data(), count);
    }
    return pclose(pipe);
}

struct AssetInfo {
    size_t offset;
    size_t size;
};

auto main(int argc, char** argv) -> int {
    if (argc < 4) {
        spdlog::error("Usage: {} <assets_binary_file> <assets_yaml_file> <assets_dir> <assets_files>", argv[0]);
        return 1;
    }

    std::ofstream bin_file(argv[1], std::ios::binary);

    size_t offset = 0;
    std::unordered_map<std::string, AssetInfo> assets{};

    for (int i = 4; i < argc; ++i) {
        auto file_path = std::filesystem::path(argv[i]);
        if (!std::filesystem::is_regular_file(file_path)) {
            continue;
        }

        auto relative_path = file_path.lexically_relative(argv[3]);

        if (file_path.extension() == ".vert" || file_path.extension() == ".frag") {
            spdlog::info("Compile shader '{}'", relative_path.native());

            std::string output;
            execute(fmt::format("glslc {} -o -", file_path.native()), output);

            bin_file.write(output.data(), output.size());
            assets.emplace(relative_path.native(), AssetInfo{offset, output.size()});
            offset += output.size();
        } else if (file_path.extension() == ".material") {
            spdlog::info("Compile material '{}'", relative_path.native());

            // write file to bin
            std::ifstream file(file_path.native(), std::ios::binary);
            if (!file.is_open()) {
                spdlog::error("Failed to open file {}", file_path.native());
                return 1;
            }
            bin_file << file.rdbuf();
            auto size = size_t(file.tellg());
            assets.emplace(relative_path.native(), AssetInfo{offset, size});
            offset += size;
        } else if (file_path.extension() == ".yaml") {
            spdlog::info("Copy '{}'", relative_path.native());

            // write file to bin
            std::ifstream file(file_path.native(), std::ios::binary);
            if (!file.is_open()) {
                spdlog::error("Failed to open file {}", file_path.native());
                return 1;
            }
            bin_file << file.rdbuf();
            auto size = size_t(file.tellg());
            assets.emplace(relative_path.native(), AssetInfo{offset, size});
            offset += size;
        }
    }
    bin_file.close();

    YAML::Emitter out;
    out << YAML::BeginMap;
    for (auto&& [key, value] : assets) {
        out << YAML::Key << key;
        out << YAML::Value << YAML::BeginMap;
        out << YAML::Key << "offset" << YAML::Value << value.offset;
        out << YAML::Key << "size" << YAML::Value << value.size;
        out << YAML::EndMap;
    }
    out << YAML::EndMap;

    std::ofstream out_file(argv[2]);
    out_file << out.c_str();
    out_file.close();

    return 0;
}