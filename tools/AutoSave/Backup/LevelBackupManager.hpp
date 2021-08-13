#pragma once

#include "../../../BetterEdit.hpp"
#include <chrono>

struct LevelBackup : public CCObject {
    using LevelBackupTime = std::chrono::time_point<std::chrono::system_clock>;

    LevelBackupTime savetime;
    std::string name;
    int objectCount;
    std::string data;

    inline long getTime() {
        return std::chrono::time_point_cast<std::chrono::seconds>(savetime)
            .time_since_epoch().count();
    }

    inline LevelBackup(DS_Dictionary* dict) {
        name = dict->getStringForKey("custom-name");
    }

    inline LevelBackup(GJGameLevel* level, std::string const& cname) {
        savetime = std::chrono::system_clock::now();;
        name = cname;
        objectCount = level->objectCount;
        data = level->levelString;

        this->autorelease();
    }
};

class LevelBackupManager {
    protected:
        CCDictionary* m_pLevels;

        bool init();

    public:
        void encodeDataTo(DS_Dictionary* data);
        void dataLoaded(DS_Dictionary* data);

        static bool initGlobal();
        static LevelBackupManager* get();

        bool levelHasBackup(GJGameLevel* level);
        bool levelHasBackup(std::string const& level);
        CCArray* getBackupsForLevel(GJGameLevel* level);
        CCArray* getBackupsForLevel(std::string const& level);
        void createBackupForLevel(GJGameLevel* level);
};
