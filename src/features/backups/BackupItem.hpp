#pragma once

#include <Geode/DefaultInclude.hpp>
#include "Backup.hpp"

using namespace geode::prelude;

struct UpdateBackupListEvent : public Event {
    bool closeList = false;
};

class BackupItem : public CCNode {
protected:
    BackupPtr m_backup;
    GJGameLevel* m_forLevel;

    bool init(BackupPtr backup, GJGameLevel* forLevel);

    void onView(CCObject*);
    void onRestore(CCObject*);
    void onDelete(CCObject*);

public:
    static BackupItem* create(BackupPtr backup, GJGameLevel* forLevel);
};
