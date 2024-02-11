
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/CCTextInputNode.hpp>
#include <utils/EditableBMLabelProxy.hpp>

using namespace geode::prelude;

class $modify(EditorUI) {
    bool init(LevelEditorLayer* lel) {
        if (!EditorUI::init(lel))
            return false;
        
        log::debug("m_lockedLayers: {0:x}", offsetof(LevelEditorLayer, m_lockedLayers));

        auto layerMenu = this->getChildByID("layer-menu");
        layerMenu->setContentSize({ 130, layerMenu->getContentSize().height });

        auto layerLockSpr = CCSprite::createWithSpriteFrameName("GJ_lockGray_001.png");
        layerLockSpr->setScale(.75f);
        auto layerLockBtn = CCMenuItemSpriteExtra::create(
            layerLockSpr, this, menu_selector(EditorUI::onLockLayer)
        );
        layerLockBtn->setID("lock-layer"_spr);
        layerMenu->insertBefore(layerLockBtn, nullptr);
        layerMenu->updateLayout();
        
        m_currentLayerLabel = EditableBMLabelProxy::replace(
            m_currentLayerLabel, this, 40.f, "Z",
            [this](auto str) {
                try {
                    m_editorLayer->m_currentLayer = std::stoi(str);
                }
                catch(...) {
                    m_editorLayer->m_currentLayer = -1;
                }
            },
            [this](auto) {
                this->updateLockBtn();
            }
        );

        // setVisible is used by GD but setOpacity is not :-)
        static_cast<CCSprite*>(this->getChildByID("layer-locked-sprite"))->setOpacity(0);
        
        return true;
    }

    void updateLockBtn() {
        auto lockBtn = static_cast<CCMenuItemSpriteExtra*>(
            this->getChildByID("layer-menu")->getChildByID("lock-layer"_spr)
        );
        const char* sprite = "GJ_lock_001.png";
        auto layerLocked = m_editorLayer->m_currentLayer >= 0 ?
            m_editorLayer->isLayerLocked(m_editorLayer->m_currentLayer) : 
            false;

        if (m_editorLayer->m_currentLayer == -1) {
            sprite = "GJ_lockGray_001.png";
        }
        else if (layerLocked) {
            sprite = "GJ_lock_open_001.png";
        }

        auto spr = CCSprite::createWithSpriteFrameName(sprite);
        spr->setScale(.7f);
        spr->setPosition(lockBtn->getNormalImage()->getPosition());
        if (m_editorLayer->m_currentLayer == -1) {
            spr->setOpacity(120);
        }
        lockBtn->setNormalImage(spr);
        spr->setAnchorPoint(layerLocked ? ccp(.52f, .2f) : ccp(.5f, .5f));

        m_currentLayerLabel->setColor(layerLocked ? ccc3(255, 205, 55) : ccc3(255, 255, 255));
    }

    $override
    void onLockLayer(CCObject* sender) {
        EditorUI::onLockLayer(sender);
        this->updateLockBtn();
    }

    $override
    void showUI(bool toggle) {
        EditorUI::showUI(toggle);

        // playtest no ui option
        if(!GameManager::sharedState()->getGameVariable("0046")) {
            toggle = true;
        }

        m_currentLayerLabel->setVisible(toggle);
    }
};
