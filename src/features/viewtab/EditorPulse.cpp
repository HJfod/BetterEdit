#include <Geode/utils/cocos.hpp>
#include <other/BEShared.hpp>

#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/FMODAudioEngine.hpp>

using namespace geode::prelude;

class $modify(EditorUIPulse, EditorUI) {
    bool m_hasResetObjectsScale = true; 

    void updateObjectsPulse(float dt) {
        auto fmod = FMODAudioEngine::sharedEngine();
        auto volume = fmod->m_backgroundMusicVolume;

        if((m_isPlayingMusic || m_editorLayer->m_playbackMode == PlaybackMode::Playing) && volume && Mod::get()->getSettingValue<bool>("editor-pulse")) {
            m_fields->m_hasResetObjectsScale = false;

            auto pulse = fmod->m_pulse1 + fmod->m_pulse2 + fmod->m_pulse3;
            pulse /= 3.f;

            const auto f = [&](GameObject* obj) {
                if (obj != nullptr && obj->m_useAudioScale) {
                    obj->setRScale(pulse);
                }
            };

            for(auto& obj : CCArrayExt<GameObject*>(m_editorLayer->m_batchNodeAddBottom->getChildren()))
                f(obj);

            for(auto& obj : CCArrayExt<GameObject*>(m_editorLayer->m_batchNodeBottom2->getChildren()))
                f(obj);

            for(auto& obj : CCArrayExt<GameObject*>(m_editorLayer->m_batchNodeBottom3->getChildren()))
                f(obj);

            for(auto& obj : CCArrayExt<GameObject*>(m_editorLayer->m_batchNodeBottom4->getChildren()))
                f(obj);

            for(auto& obj : CCArrayExt<GameObject*>(m_editorLayer->m_batchNodeBottom->getChildren()))
                f(obj);

            for(auto& obj : CCArrayExt<GameObject*>(m_editorLayer->m_batchNodeAddBottom2->getChildren()))
                f(obj);

            for(auto& obj : CCArrayExt<GameObject*>(m_editorLayer->m_batchNodeAddBottom3->getChildren()))
                f(obj);

            for(auto& obj : CCArrayExt<GameObject*>(m_editorLayer->m_batchNodeAddBottom4->getChildren()))
                f(obj);

            for(auto& obj : CCArrayExt<GameObject*>(m_editorLayer->m_batchNode->getChildren()))
                f(obj);

            for(auto& obj : CCArrayExt<GameObject*>(m_editorLayer->m_batchNodeAddTop2->getChildren()))
                f(obj);

            for(auto& obj : CCArrayExt<GameObject*>(m_editorLayer->m_batchNodeAddTop3->getChildren()))
                f(obj);

            for(auto& obj : CCArrayExt<GameObject*>(m_editorLayer->m_batchNodeTop2->getChildren()))
                f(obj);

            for(auto& obj : CCArrayExt<GameObject*>(m_editorLayer->m_batchNodeTop3->getChildren()))
                f(obj);
        }
        else if(!m_fields->m_hasResetObjectsScale) {
            m_fields->m_hasResetObjectsScale = true;

            for(auto& obj : CCArrayExt<GameObject*>(m_editorLayer->getAllObjects()))
                if(obj != nullptr && obj->m_useAudioScale)
                    obj->setRScale(1.f);
        }
    }

    bool init(LevelEditorLayer* editor) {
        if(!EditorUI::init(editor))
            return false;

        schedule(schedule_selector(EditorUIPulse::updateObjectsPulse));

        return true;
    }
};

// allow pulses in every layer
class $modify(FMODAudioEngine) {
    void update(float dt) {
        bool meteringOrig = m_metering;

        if(!meteringOrig)
            m_metering = Mod::get()->getSettingValue<bool>("editor-pulse");

        FMODAudioEngine::update(dt);

        m_metering = meteringOrig;
    }
};