#pragma once

#include <Geode/ui/Popup.hpp>
#include <Geode/ui/ScrollLayer.hpp>
#include "BackupItem.hpp"
#include <utils/PopupWithCorners.hpp>

using namespace geode::prelude;

class BackupListPopup : public PopupWithCorners {
protected:
    Ref<GJGameLevel> m_level;
    ScrollLayer* m_scrollLayer;
    CCLabelBMFont* m_statusLabel;
    ListenerHandle m_updateListListener;

    bool init(GJGameLevel* level);
    void updateList();

    void onNewBackup(CCObject*);

public:
    static BackupListPopup* create(GJGameLevel* level);
};
