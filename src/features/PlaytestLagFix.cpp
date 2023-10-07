#include <Geode/Geode.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>

using namespace geode::prelude;

class $modify(LevelEditorLayer) {
    bool m_playtestStarted = false;

    void update(float dt) {
        if(m_fields->m_playtestStarted) {
            dt = CCDirector::sharedDirector()->getAnimationInterval();
            m_fields->m_playtestStarted = false;
        }

        LevelEditorLayer::update(dt);
    }

    void onPlaytest() {
        m_fields->m_playtestStarted = Mod::get()->getSettingValue<bool>("playtest-lag-fix");

        LevelEditorLayer::onPlaytest();
    }
};

