#pragma once

#include <Geode/binding/GJGameLevel.hpp>

using namespace geode::prelude;

class Backup final {
public:
    using Clock = std::chrono::utc_clock;
    using TimePoint = std::chrono::time_point<Clock>;
    using TimeUnit = std::chrono::minutes;

private:
    ghc::filesystem::path m_directory;
    Ref<GJGameLevel> m_level;
    Ref<GJGameLevel> m_forLevel;
    TimePoint m_createTime;

public:
    static Result<std::shared_ptr<Backup>> load(ghc::filesystem::path const& dir, GJGameLevel* forLevel);
    static std::vector<std::shared_ptr<Backup>> load(GJGameLevel* level);
    static Result<> create(GJGameLevel* level);

    GJGameLevel* getLevel() const;
    TimePoint getCreateTime() const;

    Result<> restoreThis();
    Result<> deleteThis();
};
using BackupPtr = std::shared_ptr<Backup>;
