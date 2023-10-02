#include "RotateSaws.hpp"

#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
//#include <Geode/modify/SetupRotatePopup.hpp>

using namespace geode::prelude;

static constexpr const int ROTATEACTION_TAG = 0x42069;

std::map<GameObject*, float> g_startRotations;
std::array<int, 100> g_sawIDs = {
    1705, 1706, 1707,
    1708, 1709, 1710,
    1734, 1735, 1736,
    678, 679, 680,
    186, 187, 188,
    740, 741, 742,
    1619, 1620,
    183, 184, 185,
    85, 86, 87, 97,
    1831, 1832, 1833, 1834,
    1058, 1059, 1060, 1061,
    1752,
    1055, 1056, 1057,
    1019, 1020, 1021,
    997, 998, 999, 1000,
    1521, 1522,
    180, 181, 182,
    154, 155, 156,
    137, 138, 139,
    222, 223, 224,
    1523, 1524, 1525, 1526, 1527, 1528,
    394, 395, 396,
    375, 376, 377, 378,
    // old black 1.9 saws
    675, 676, 677,
    // orbs
    1022, 1330,
};

bool objectIsSaw(GameObject* obj) {
    for (auto id : g_sawIDs)
        if (id == obj->m_objectID)
            return true;
    
    return false;
}

CCAction* recreatedRotateAction(float f, int i) {
    auto ret = CCRepeatForever::create(CCRotateBy::create(f, f));
    return (CCAction*)ret;
}

void beginRotateSaw(GameObject* obj) {
    if (obj->m_myAction)
        return;

    if (obj->m_sawIsDisabled)
        return;

    CCAction* r;
    switch (obj->m_objectID) {
        // the big invisible saw, for some reason
        case 0xba: r = recreatedRotateAction(300.0f, 0); break;
        // the big reaper saw
        case 0x653: r = recreatedRotateAction(720.0f, -1); break;
        // the small reaper saw
        case 0x654: r = recreatedRotateAction(1080.0f, -1); break;
        default: r = recreatedRotateAction(360.0f, 0); break;
    }
    r->setTag(ROTATEACTION_TAG);
    g_startRotations[obj] = obj->getRotation();
    obj->m_myAction = r;
    obj->m_myAction->retain();
    obj->runAction(r);
}

void beginRotations(LevelEditorLayer* self) {
    for (auto obj : CCArrayExt<GameObject*>(self->m_objects)) {
        if (objectIsSaw(obj))
            beginRotateSaw(obj);
    }
}

void stopRotateSaw(GameObject* obj) {
    obj->stopActionByTag(ROTATEACTION_TAG);
    if (g_startRotations.count(obj)) {
        obj->setRotation(g_startRotations[obj]);
        g_startRotations.erase(obj);
    }
    if (obj->m_myAction)
        obj->m_myAction->release();
    obj->m_myAction = nullptr;
}

void stopRotations(LevelEditorLayer* self) {
    for (auto obj : CCArrayExt<GameObject*>(self->m_objects)) {
        if (objectIsSaw(obj))
            stopRotateSaw(obj);
    }

    g_startRotations.clear();
}

void resumeRotations(LevelEditorLayer* self) {
    for (auto obj : CCArrayExt<GameObject*>(self->m_objects)) {
        if (objectIsSaw(obj))
            if (obj->m_myAction)
                obj->runAction(obj->m_myAction);
    }
}

void pauseRotations(LevelEditorLayer* self) {
    for (auto obj : CCArrayExt<GameObject*>(self->m_objects)) {
        if (objectIsSaw(obj))
            if (obj->m_myAction)
                obj->stopActionByTag(ROTATEACTION_TAG);
    }
}

void onRotateSaws(CCObject* pSender) {
    bool toggled = !typeinfo_cast<CCMenuItemToggler*>(pSender)->isToggled();

    geode::Mod::get()->setSettingValue<bool>("rotate-saws-in-editor", toggled);

    if(toggled)  
        beginRotations(LevelEditorLayer::get());

    else 
        stopRotations(LevelEditorLayer::get());
}

bool shouldRotateSaw() {
    return Mod::get()->getSettingValue<bool>("rotate-saws-in-editor");
}

class $modify(LevelEditorLayer) {
    void onPlaytest() {
        LevelEditorLayer::onPlaytest();

        if (!shouldRotateSaw())
            beginRotations(this);
    }

    void onResumePlaytest() {
        LevelEditorLayer::onResumePlaytest();

        if (!shouldRotateSaw())
            resumeRotations(this);
    }

    void onPausePlaytest() {
        LevelEditorLayer::onPausePlaytest();

        if (!shouldRotateSaw())
            pauseRotations(this);
    }

    void onStopPlaytest() {
        LevelEditorLayer::onStopPlaytest();

        if (!shouldRotateSaw())
            stopRotations(this);
    }

    void addSpecial(GameObject* obj) {
        LevelEditorLayer::addSpecial(obj);

        if(shouldRotateSaw() && objectIsSaw(obj))
            beginRotateSaw(obj);
    }

    void removeObject(GameObject* obj, bool p1) {
        LevelEditorLayer::removeObject(obj, p1);

        if(shouldRotateSaw() && objectIsSaw(obj))
            stopRotateSaw(obj);
    }    
};

class $modify(EditorPauseLayer) {
    void saveLevel() {
        if(shouldRotateSaw())
            stopRotations(m_editorLayer);

        EditorPauseLayer::saveLevel();

        if(shouldRotateSaw())
            beginRotations(m_editorLayer);
    }
};

#ifdef GEODE_IS_WINDOWS

class $modify(SetupRotatePopup) {
    void onClose(CCObject* pSender) {
        if(shouldRotateSaw()) {
            stopRotations(LevelEditorLayer::get());
            beginRotations(LevelEditorLayer::get());
        }

        SetupRotatePopup::onClose(pSender);
    }

    void keyBackClicked() {
        if(shouldRotateSaw()) {
            stopRotations(LevelEditorLayer::get());
            beginRotations(LevelEditorLayer::get());
        }

        SetupRotatePopup::keyBackClicked();
    }
};

#endif