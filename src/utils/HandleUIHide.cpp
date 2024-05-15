#include "HandleUIHide.hpp"
#include <Geode/modify/EditorUI.hpp>

class $modify(HideUI, EditorUI) {
    struct Fields {
        std::vector<Ref<CCNode>> toHide;
    };

    $override
    void showUI(bool toggle) {
        EditorUI::showUI(toggle);

        // playtest no ui option
        if (!GameManager::sharedState()->getGameVariable("0046")) {
            toggle = true;
        }

        for (auto toHide : m_fields->toHide) {
            toHide->setVisible(toggle);
        }

        m_currentLayerLabel->setVisible(toggle);

        auto lockBtn = static_cast<CCMenuItemSpriteExtra*>(
            this->getChildByID("layer-menu")->getChildByID("lock-layer"_spr)
        );
        if (lockBtn) {
            lockBtn->setVisible(toggle);
        }
    }
};

void handleUIHideOnPlaytest(EditorUI* ui, CCNode* target) {
    static_cast<HideUI*>(ui)->m_fields->toHide.push_back(target);
}
