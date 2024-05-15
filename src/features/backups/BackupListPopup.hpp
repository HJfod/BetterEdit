#pragma once

#include <Geode/ui/Popup.hpp>
#include <Geode/ui/ScrollLayer.hpp>
#include "BackupItem.hpp"
#include <utils/PopupWithCorners.hpp>

using namespace geode::prelude;

class BackupListPopup : public PopupWithCorners<GJGameLevel*> {
protected:
    Ref<GJGameLevel> m_level;
    ScrollLayer* m_scrollLayer;
    CCLabelBMFont* m_statusLabel;
    EventListener<EventFilter<UpdateBackupListEvent>> m_updateListListener;

    bool setup(GJGameLevel* level) override;
    void updateList();

    void onNewBackup(CCObject*);

public:
    static BackupListPopup* create(GJGameLevel* level);
};
