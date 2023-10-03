#include <Geode/Geode.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>

using namespace geode::prelude;

int smoothOut = 0;

class $modify(LevelEditorLayer) {
    void update(float dt) {
        if (!Mod::get()->getSettingValue<bool>("playtest-lag-fix")) return LevelEditorLayer::update(dt);

        float time = cocos2d::CCDirector::sharedDirector()->getAnimationInterval();
        if (smoothOut != 0 && dt - time < 1) {
            smoothOut --;
        }

        LevelEditorLayer::update(time);
    }

    void onPlaytest() {
        if (Mod::get()->getSettingValue<bool>("playtest-lag-fix")) smoothOut = 2;

        LevelEditorLayer::onPlaytest();
    }
};