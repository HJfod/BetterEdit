#include <Geode/utils/cocos.hpp>
#include <Other/BEShared.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/FMODAudioEngine.hpp>
#include <Geode/binding/GameObject.hpp>
#include <Geode/binding/GameSoundManager.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>

using namespace geode::prelude;

void updatePulsesInArray(CCArray* array, float pulse) {
    CCObject* _obj;
    CCARRAY_FOREACH(array, _obj) {
        auto obj = typeinfo_cast<GameObject*>(_obj);

        if (obj && obj->m_useAudioScale) {
            obj->setRScale(pulse);
        }
    }
}
#define UPDATE_PULSES_IN_ARRAY(arr) updatePulsesInArray(arr, pulse)

class $modify(EditorUIPulse, EditorUI) {
    bool m_hasResetObjectsScale = true; 

    void updateObjectsPulse(float dt) {
        auto fmod = FMODAudioEngine::sharedEngine();
        auto volume = fmod->m_backgroundMusicVolume;

        if (
            (m_isPlayingMusic || m_editorLayer->m_playbackMode == PlaybackMode::Playing) &&
            volume &&
            Mod::get()->getSettingValue<bool>("editor-pulse")
        ) {
            m_fields->m_hasResetObjectsScale = false;

            auto pulse = fmod->m_pulse1 + fmod->m_pulse2 + fmod->m_pulse3;
            pulse /= 3.f;

            UPDATE_PULSES_IN_ARRAY(m_editorLayer->m_batchNodeAddBottom->getChildren());
            UPDATE_PULSES_IN_ARRAY(m_editorLayer->m_batchNodeBottom2->getChildren());
            UPDATE_PULSES_IN_ARRAY(m_editorLayer->m_batchNodeBottom3->getChildren());
            UPDATE_PULSES_IN_ARRAY(m_editorLayer->m_batchNodeBottom4->getChildren());
            UPDATE_PULSES_IN_ARRAY(m_editorLayer->m_batchNodeBottom->getChildren());
            UPDATE_PULSES_IN_ARRAY(m_editorLayer->m_batchNodeAddBottom2->getChildren());
            UPDATE_PULSES_IN_ARRAY(m_editorLayer->m_batchNodeAddBottom3->getChildren());
            UPDATE_PULSES_IN_ARRAY(m_editorLayer->m_batchNodeAddBottom4->getChildren());
            UPDATE_PULSES_IN_ARRAY(m_editorLayer->m_batchNode->getChildren());
            UPDATE_PULSES_IN_ARRAY(m_editorLayer->m_batchNodeAddTop2->getChildren());
            UPDATE_PULSES_IN_ARRAY(m_editorLayer->m_batchNodeAddTop3->getChildren());
            UPDATE_PULSES_IN_ARRAY(m_editorLayer->m_batchNodeTop2->getChildren());
            UPDATE_PULSES_IN_ARRAY(m_editorLayer->m_batchNodeTop3->getChildren());
        }
        else if (!m_fields->m_hasResetObjectsScale) {
            m_fields->m_hasResetObjectsScale = true;
            updatePulsesInArray(m_editorLayer->getAllObjects(), 1);
        }
    }

    bool init(LevelEditorLayer* editor) {
        if (!EditorUI::init(editor))
            return false;

        // allow pulses in every layer
        GameSoundManager::sharedManager()->enableMetering();
        this->schedule(schedule_selector(EditorUIPulse::updateObjectsPulse));

        return true;
    }
};

$onEditorExit {
    GameSoundManager::sharedManager()->disableMetering();
}
