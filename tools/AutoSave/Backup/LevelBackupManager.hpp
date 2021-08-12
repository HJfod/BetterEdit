#pragma once

#include "../../../BetterEdit.hpp"
#include <chrono>

struct LevelBackup {
    using LevelBackupTime = std::chrono::time_point<std::chrono::system_clock>;

    LevelBackupTime savetime;
    std::string name;
    int objectCount;
    std::string data;
};

class LevelBackupManager {
    public:
        using backup_list = std::vector<LevelBackup>;

    protected:
        std::map<std::string, backup_list> m_mLevels;

        bool init();

    public:
        void encodeDataTo(DS_Dictionary* data);
        void dataLoaded(DS_Dictionary* data);

        static bool initGlobal();
        static LevelBackupManager* get();

        bool levelHasBackup(GJGameLevel* level);
        bool levelHasBackup(std::string const& level);
        backup_list* getBackupsForLevel(GJGameLevel* level);
        backup_list* getBackupsForLevel(std::string const& level);
        void createBackupForLevel(GJGameLevel* level);
};
