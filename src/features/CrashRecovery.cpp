#include <Geode/modify/LevelEditorLayer.hpp>

using namespace geode::prelude;

class $modify(LevelEditorLayer) {
    $override
    bool init(GJGameLevel* level, bool unk) {
        if (!LevelEditorLayer::init(level, unk))
            return false;
        
        if (!Mod::get()->template getSettingValue<bool>("soft-save-enabled")) {
            return true;
        }
        
        return true;
    }
};
