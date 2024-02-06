#include <Geode/Bindings.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
#include "UIScaling.hpp"

using namespace geode::prelude;

class $modify(EditorPauseLayer) {
    bool init(LevelEditorLayer* p0) {
        if (!EditorPauseLayer::init(p0))
            return false;

        if (!Mod::get()->getSettingValue<bool>("scale-pause"))
            return true;

        float scale = Mod::get()->getSettingValue<double>("scale-factor");
        scaleChild(this, "resume-menu", scale);
        scaleChild(this, "info-menu", scale);
        scaleChild(this, "small-actions-menu", scale, ccp(2, 0));
        scaleChild(this, "actions-menu", scale, ccp(1, 0));
        scaleChild(this, "options-menu", scale, ccp(0, 0));
        scaleChild(this, "settings-menu", scale);
        scaleChild(this, "guidelines-menu", scale);

        return true;
    }
};
