#pragma once

#include <Geode/DefaultInclude.hpp>
#include "Backup.hpp"

using namespace geode::prelude;

struct UpdateBackupListEvent : public Event<UpdateBackupListEvent, bool(bool)> {
    using Event::Event;
};

class BackupItem : public CCNode {
protected:
    BackupPtr m_backup;

    bool init(BackupPtr backup);

    void onView(CCObject*);
    void onRestore(CCObject*);
    void onDelete(CCObject*);
    void onConvertAutomated(CCObject*);

public:
    static BackupItem* create(BackupPtr backup);
};
