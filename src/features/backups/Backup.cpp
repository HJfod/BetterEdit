#include "Backup.hpp"
#include <chrono>
/*
Backup Backup::get(std::string path, GJGameLevel* level) {
    Backup bk; // have it your way

    bk.m_folderPath = geode::Mod::get()->getSaveDir().append("Levels").append(path);
    auto GMDpath = bk.m_folderPath.append("data.gmd");

    bk.m_exists = ghc::filesystem::exists(GMDpath);

    uint64_t time = duration_cast<std::chrono::milliseconds>(
        system_clock::now().time_since_epoch()
    ).count();

    // TODO: rewrite the json shit

    json::Value value;

    // read file

    std::ifstream inFile;
    inFile.open(geode::Mod::get()->getSaveDir().append("Levels").append("BEBackupManager.json").c_str());

    std::stringstream strStream;
    strStream << inFile.rdbuf();
    value = json::parse(std::string_view(strStream.str()));

    // do the funny backup management

    if (!value[path].is_null()) {
        value[path]["last_backup"] = time;
        value[path]["backup_frequency"] = 
    }

    if (!bk.m_exists) {
        bk.m_level = level;
    } else {
        auto LVLimport = gmd::importGmdAsLevel(GMDpath);
        bk.m_level = LVLimport.unwrap();
    }

    return bk;
}

void Backup::startBackup(BackupInterval interval) {
    switch (interval) {
        case ONOPEN:

    }
}

*/