#include "HandleUIHide.hpp"
#include <Geode/modify/EditorUI.hpp>

class $modify(HideUI, EditorUI) {
    $override
    void showUI(bool show) {
        if (m_editorLayer->m_playbackMode == PlaybackMode::Playing) {
            // Playtest no ui option
            show = !GameManager::sharedState()->getGameVariable("0046");
        }

        EditorUI::showUI(show);
        UIShowEvent(this, show).post();

        m_currentLayerLabel->setVisible(show);

        auto lockBtn = static_cast<CCMenuItemSpriteExtra*>(
            this->getChildByID("layer-menu")->getChildByID("lock-layer"_spr)
        );
        if (lockBtn) {
            lockBtn->setVisible(show);
        }
        m_tabsMenu->setVisible(show && m_selectedMode == 2);
    }
};

UIShowEvent::UIShowEvent(EditorUI* ui, bool show) : ui(ui), show(show) {}

UIShowFilter::UIShowFilter(EditorUI* ui) : m_ui(ui) {}
ListenerResult UIShowFilter::handle(MiniFunction<Callback> fn, UIShowEvent* ev) {
    if (m_ui == ev->ui) {
        fn(ev);
    }
    return ListenerResult::Propagate;
}
