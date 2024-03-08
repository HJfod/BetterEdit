#pragma once

#include <string>
#include <ghc/filesystem.hpp>
#include <optional>
#include <vector>
#include <fstream>
#include <functional>
#include <fmt/format.h>


struct ScriptInfo
{    
    std::string name;
    std::string dev;
    std::string version;
    ghc::filesystem::path path;

    static std::optional<ScriptInfo> fromLuaFile(const ghc::filesystem::path& filePath)
    {
        if(!ghc::filesystem::exists(filePath)) return {};

        auto getValueFromComment = [](std::string_view line, std::string_view key) -> std::optional<std::string_view>
        {
            constexpr auto npos = std::string_view::npos;

            //return early if not comment (avoid format)
            if(line[0] != '-') return {}; 

            auto start = line.find(fmt::format("--@{}", key).c_str());
            if (start == npos) return {};

            auto startValue = line.find_first_of(' ', start);
            if (startValue == npos) return {};

            return line.substr(startValue + 1, line.size() - startValue);
        };

        std::ifstream f(filePath);
        if (!f.good()) return {};

        std::string line;
        std::string name;
        std::string dev;
        std::string version;

        while (std::getline(f, line))
        {
            if (auto v = getValueFromComment(line, "name")) name = *v;
            else if (auto val = getValueFromComment(line, "developer")) dev = *val;
            else if(auto value = getValueFromComment(line, "version")) version = *value;
        }

        if(name.empty()) return {};
        return ScriptInfo{ name, dev, version, filePath };
    }

    static std::vector<ScriptInfo> getScriptsFromFolder(const ghc::filesystem::path& folder)
    {
        if(!ghc::filesystem::exists(folder) || !ghc::filesystem::is_directory(folder)) return {};

        std::vector<ScriptInfo> ret;
        for(const auto& item : ghc::filesystem::directory_iterator{folder})
        {
            if(!item.is_regular_file()) continue;

            const auto& itemPath = item.path();
            if(!itemPath.has_extension() || itemPath.extension() != ".lua") continue;


            if(auto script = ScriptInfo::fromLuaFile(itemPath)) ret.push_back(*script);
        }
        return ret;
    }

    static std::optional<ghc::filesystem::path> getScriptsPath()
    {
        auto MOD = geode::Mod::get();
        auto scripts_dir = MOD->getConfigDir() / "scripts";

        if(ghc::filesystem::is_directory(scripts_dir))
            return scripts_dir;

        if(ghc::filesystem::is_symlink(scripts_dir))
        {
            auto sympath = ghc::filesystem::read_symlink(scripts_dir);
            if(ghc::filesystem::is_directory(sympath))
                return sympath;
        }

        return ghc::filesystem::create_directory(scripts_dir) ? scripts_dir : ghc::filesystem::path{};
    }

    std::string getUniqueString() const {
        return geode::Mod::get()->expandSpriteName(fmt::format("{}-{}", name, dev).c_str());
    }

    static void forAllOnPath(const std::function<void(const ScriptInfo&)>& callback)
    {
        auto path = getScriptsPath();
        if(!path) return;
        for(const auto& s : getScriptsFromFolder(*path))
        {
            callback(s);
        }
    }

};