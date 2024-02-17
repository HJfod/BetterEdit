
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/CCTextInputNode.hpp>
#include <utils/EditableBMLabelProxy.hpp>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/binding/GameManager.hpp>

using namespace geode::prelude;

class $modify(TypeInUI, EditorUI) {
    bool init(LevelEditorLayer* lel) {
        if (!EditorUI::init(lel))
            return false;
        
        auto layerMenu = this->getChildByID("layer-menu");
        layerMenu->setContentSize({ 130, layerMenu->getContentSize().height });

        auto layerLockSpr = CCSprite::createWithSpriteFrameName("GJ_lockGray_001.png");
        layerLockSpr->setScale(.75f);
        auto layerLockBtn = CCMenuItemSpriteExtra::create(
            layerLockSpr, this, menu_selector(TypeInUI::onLockLayer)
        );
        layerLockBtn->setID("lock-layer"_spr);
        layerMenu->insertBefore(layerLockBtn, nullptr);
        layerMenu->updateLayout();
        
        m_currentLayerLabel = EditableBMLabelProxy::replace(
            m_currentLayerLabel, this, 40.f, "Z",
            [this](auto str) {
                m_editorLayer->m_currentLayer = numFromString<int>(str).unwrapOr(-1);
            },
            [this](auto) {
                this->updateLockBtn();
            }
        );

        // Delete the existing layer lock button that's just an overlay on the text
        this->getChildByIDRecursive("lock-layer-button")->removeFromParent();

        // setVisible is used by GD but setOpacity is not :-)
        static_cast<CCSprite*>(this->getChildByID("layer-locked-sprite"))->setOpacity(0);
        
        return true;
    }

    void updateLockBtn() {
        auto lockBtn = static_cast<CCMenuItemSpriteExtra*>(
            this->getChildByID("layer-menu")->getChildByID("lock-layer"_spr)
        );
        const char* sprite = "GJ_lock_open_001.png";
        auto onAll = m_editorLayer->m_currentLayer == -1;
        auto layerLocked = !onAll ?
            m_editorLayer->isLayerLocked(m_editorLayer->m_currentLayer) : 
            false;

        if (onAll) {
            sprite = "GJ_lockGray_001.png";
        }
        else if (layerLocked) {
            sprite = "GJ_lock_001.png";
        }

        auto spr = CCSprite::createWithSpriteFrameName(sprite);
        spr->setScale((layerLocked || onAll) ? .68f : .7f);
        spr->setPosition(lockBtn->getNormalImage()->getPosition());
        if (m_editorLayer->m_currentLayer == -1) {
            spr->setOpacity(120);
        }
        lockBtn->setNormalImage(spr);
        spr->setAnchorPoint((layerLocked || onAll) ? ccp(.47f, .48f) : ccp(.5f, .2f));

        lockBtn->setVisible(m_editorLayer->m_layerLockingEnabled);
        m_currentLayerLabel->setColor(layerLocked ? ccc3(255, 150, 0) : ccc3(255, 255, 255));
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
        if (!GameManager::sharedState()->getGameVariable("0046")) {
            toggle = true;
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
