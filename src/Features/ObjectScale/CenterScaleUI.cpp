#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>

using namespace geode::prelude;

class $modify(CenterScaleEditorUI, EditorUI) {
    void activateScaleControl(CCObject* sender) {
        EditorUI::activateScaleControl(sender);
        if (Mod::get()->getSettingValue<bool>("center-scale-on-screen")) {
            log::info("position");
            auto position = CCDirector::sharedDirector()->getWinSize() / 2;
            position.height += 50.0f;
            position = this->m_editorLayer->m_objectLayer->convertToNodeSpace(position);
            m_scaleControl->setPosition(position);
        }
    }
};