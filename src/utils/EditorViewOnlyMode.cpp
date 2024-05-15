#include "EditorViewOnlyMode.hpp"
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
#include <Geode/modify/GameManager.hpp>

static Ref<CCLayer> SCENE_TO_RETURN_TO = nullptr;
class $modify(GameManager) {
    $override
    void returnToLastScene(GJGameLevel* level) {
        if (SCENE_TO_RETURN_TO) {
            cocos::switchToScene(SCENE_TO_RETURN_TO);
            this->fadeInMenuMusic();
            SCENE_TO_RETURN_TO = nullptr;
        }
        else {
            GameManager::returnToLastScene(level);
        }
    }
};

class ViewOnlyModeData : public CCObject {
protected:
    utils::MiniFunction<CCLayer*()> m_returnTo;

public:
    static ViewOnlyModeData* create(utils::MiniFunction<CCLayer*()> returnTo) {
        auto ret = new ViewOnlyModeData();
        ret->m_returnTo = returnTo;
        ret->autorelease();
        return ret;
    }

    CCLayer* getLayerToReturnTo() const {
        return m_returnTo();
    }
};

LevelEditorLayer* createViewOnlyEditor(GJGameLevel* level, utils::MiniFunction<CCLayer*()> returnTo) {
    auto editor = LevelEditorLayer::create(level, false);
    editor->setUserObject("view-only-mode"_spr, ViewOnlyModeData::create(returnTo));

    auto viewOnlyLabel = CCLabelBMFont::create("View-Only Mode", "bigFont.fnt");
    viewOnlyLabel->setOpacity(90);
    viewOnlyLabel->setScale(.5f);
    editor->addChildAtPosition(viewOnlyLabel, Anchor::Top, ccp(0, -80), false);

    editor->m_editorUI->showUI(false);
    
    return editor;
}
bool isViewOnlyEditor(LevelEditorLayer* lel) {
    return lel->getUserObject("view-only-mode"_spr);
}

class $modify(EditorUI) {
    static void onModify(auto& self) {
        (void)self.setHookPriority("EditorUI::showUI", -100);
    }

    $override
    void clickOnPosition(CCPoint pos) {
        if (!isViewOnlyEditor(m_editorLayer)) {
            EditorUI::clickOnPosition(pos);
        }
    }
    $override
    void showUI(bool show) {
        if (isViewOnlyEditor(m_editorLayer)) {
            show = false;
            m_tabsMenu->setVisible(false);
        }
        EditorUI::showUI(show);
    }
    $override
    void toggleMode(CCObject* sender) {
        EditorUI::toggleMode(sender);
        if (isViewOnlyEditor(m_editorLayer)) {
            m_editButtonBar->setVisible(false);
            m_createButtonBar->setVisible(false);
            m_deleteMenu->setVisible(false);
            m_tabsMenu->setVisible(false);
        }
    }
};
class $modify(EditorPauseLayer) {
    static void onModify(auto& self) {
        (void)self.setHookPriority("EditorPauseLayer::init", -100);
    }

    static void disableBtn(CCNode* node) {
        if (auto btn = typeinfo_cast<CCMenuItemSpriteExtra*>(node)) {
            btn->setEnabled(false);
            auto spr = typeinfo_cast<CCRGBAProtocol*>(btn->getNormalImage());
            spr->setCascadeColorEnabled(true);
            spr->setCascadeOpacityEnabled(true);
            spr->setColor({ 90, 90, 90 });
            spr->setOpacity(200);
        }
    }
    $override
    bool init(LevelEditorLayer* lel) {
        if (!EditorPauseLayer::init(lel))
            return false;

        if (isViewOnlyEditor(lel)) {
            for (auto node : CCArrayExt<CCNode*>(this->getChildByID("resume-menu")->getChildren())) {
                if (node->getID() == "resume-button" || node->getID() == "exit-button") continue;
                disableBtn(node);
            }
            for (auto node : CCArrayExt<CCNode*>(this->getChildByID("small-actions-menu")->getChildren())) {
                disableBtn(node);
            }
            for (auto node : CCArrayExt<CCNode*>(this->getChildByID("actions-menu")->getChildren())) {
                disableBtn(node);
            }
        }
        
        return true;
    }
    $override
    void onExitNoSave(CCObject* sender) {
        if (auto obj = m_editorLayer->getUserObject("view-only-mode"_spr)) {
            SCENE_TO_RETURN_TO = static_cast<ViewOnlyModeData*>(obj)->getLayerToReturnTo();
            EditorPauseLayer::onExitEditor(sender);
        }
        else {
            return EditorPauseLayer::onExitNoSave(sender);
        }
    }
};
