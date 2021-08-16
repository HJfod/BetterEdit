#pragma once

#include "../../../BetterEdit.hpp"
#include <chrono>

struct LevelBackup : public CCObject {
    using LevelBackupTime = std::chrono::time_point<std::chrono::system_clock>;

    // normalize time to be better served as an int
    // there's no way a backup can be made before this point
    // and also the int's signed so it should theoretically
    // work anyway
    static constexpr const long time_offset = 1629000000;

    LevelBackupTime savetime;
    std::string name;
    int objectCount;
    std::string data;

    inline int getTime() {
        return static_cast<int>(std::chrono::time_point_cast<std::chrono::seconds>(savetime)
            .time_since_epoch().count() - time_offset);
    }

    inline LevelBackupTime getTime(long time) {
        return LevelBackupTime(std::chrono::seconds(time + time_offset));
    }

    inline void save(DS_Dictionary* dict) {
        dict->setIntegerForKey("save-time", this->getTime());
        dict->setStringForKey("custom-name", this->name);
        dict->setIntegerForKey("object-count", this->objectCount);
        dict->setStringForKey("data", this->data);
    }

    inline LevelBackup(DS_Dictionary* dict) {
        savetime = getTime(dict->getIntegerForKey("save-time"));
        name = dict->getStringForKey("custom-name");
        objectCount = dict->getIntegerForKey("object-count");
        data = dict->getStringForKey("data");

        this->autorelease();
    }

    inline LevelBackup(GJGameLevel* level, std::string const& cname) {
        savetime = std::chrono::system_clock::now();
        name = cname;
        objectCount = level->objectCount;
        data = level->levelString;

        this->autorelease();
    }
};

class LevelBackupManager : public GManager {
    protected:
        CCDictionary* m_pLevels;

        bool init();

    public:
        void encodeDataTo(DS_Dictionary* data) override;
        void dataLoaded(DS_Dictionary* data) override;
        void firstLoad() override;

        static bool initGlobal();
        static LevelBackupManager* get();

        bool levelHasBackup(GJGameLevel* level);
        bool levelHasBackup(std::string const& level);
        CCArray* getBackupsForLevel(GJGameLevel* level);
        CCArray* getBackupsForLevel(std::string const& level);
        void createBackupForLevel(GJGameLevel* level);
        void removeBackupForLevel(GJGameLevel* level, LevelBackup* backup);
};
