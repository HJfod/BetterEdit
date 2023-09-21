#include <Geode/utils/cocos.hpp>
#include <other/BEShared.hpp>

#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/FMODAudioEngine.hpp>

using namespace geode::prelude;

void _UpdatePulsesInArray(CCArray* array, float pulse) {
    CCObject* _obj;
    CCARRAY_FOREACH(array, _obj) {
        auto obj = typeinfo_cast<GameObject*>(_obj);

        if(obj != nullptr && obj->m_useAudioScale)
            obj->setRScale(pulse);
    }
}
#define UpdatePulsesInArray(arr) _UpdatePulsesInArray(arr, pulse)

class $modify(EditorUIPulse, EditorUI) {
    bool m_hasResetObjectsScale = true; 

    void updateObjectsPulse(float dt) {
        auto fmod = FMODAudioEngine::sharedEngine();
        auto volume = fmod->m_backgroundMusicVolume;

        if((m_isPlayingMusic || m_editorLayer->m_playbackMode == PlaybackMode::Playing) && volume && Mod::get()->getSettingValue<bool>("editor-pulse")) {
            m_fields->m_hasResetObjectsScale = false;

            auto pulse = fmod->m_pulse1 + fmod->m_pulse2 + fmod->m_pulse3;
            pulse /= 3.f;

            UpdatePulsesInArray(m_editorLayer->m_batchNodeAddBottom->getChildren());
            UpdatePulsesInArray(m_editorLayer->m_batchNodeBottom2->getChildren());
            UpdatePulsesInArray(m_editorLayer->m_batchNodeBottom3->getChildren());
            UpdatePulsesInArray(m_editorLayer->m_batchNodeBottom4->getChildren());
            UpdatePulsesInArray(m_editorLayer->m_batchNodeBottom->getChildren());
            UpdatePulsesInArray(m_editorLayer->m_batchNodeAddBottom2->getChildren());
            UpdatePulsesInArray(m_editorLayer->m_batchNodeAddBottom3->getChildren());
            UpdatePulsesInArray(m_editorLayer->m_batchNodeAddBottom4->getChildren());
            UpdatePulsesInArray(m_editorLayer->m_batchNode->getChildren());
            UpdatePulsesInArray(m_editorLayer->m_batchNodeAddTop2->getChildren());
            UpdatePulsesInArray(m_editorLayer->m_batchNodeAddTop3->getChildren());
            UpdatePulsesInArray(m_editorLayer->m_batchNodeTop2->getChildren());
            UpdatePulsesInArray(m_editorLayer->m_batchNodeTop3->getChildren());
        }
        else if(!m_fields->m_hasResetObjectsScale) {
            m_fields->m_hasResetObjectsScale = true;

            _UpdatePulsesInArray(m_editorLayer->getAllObjects(), 1);
        }
    }

    bool init(LevelEditorLayer* editor) {
        if(!EditorUI::init(editor))
            return false;

        // allow pulses in every layer
        GameSoundManager::sharedManager()->enableMetering();

        schedule(schedule_selector(EditorUIPulse::updateObjectsPulse));

        return true;
    }
};

BE_EDITOREXIT() {
    GameSoundManager::sharedManager()->disableMetering();
}

// allow pulses in every layer
/*class $modify(FMODAudioEngine) {
    void update(float dt) {
        bool meteringOrig = m_metering;

        if(!meteringOrig)
            m_metering = Mod::get()->getSettingValue<bool>("editor-pulse");

        FMODAudioEngine::update(dt);

        m_metering = meteringOrig;
    }
};*/