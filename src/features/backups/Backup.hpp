#pragma once

#include <Geode/DefaultInclude.hpp>
#include <hjfod.gmd-api/include/GMD.hpp>
#include <ghc/filesystem.hpp>

using namespace geode::prelude;

enum BackupInterval {
    NONE,
    ONOPEN,
    DAILY,
    WEEKLY,
    MONTHLY
};

struct Backup {
    static Backup get(std::string path, GJGameLevel* level);
    void startBackup(BackupInterval interval);
    void useBackup(GJGameLevel* level);

    GJGameLevel* m_level;
    ghc::filesystem::path m_folderPath;
    bool m_exists;
};
