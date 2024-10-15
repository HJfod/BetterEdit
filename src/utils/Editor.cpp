#include "Editor.hpp"
#include <Geode/modify/GameManager.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
#include <utils/Warn.hpp>

// i hate touch prioi hate touch prioi hate touch prioi hate touch prioi hate 
// touch prioi hate touch prioi hate touch prioi hate touch prioi hate touch 
// aka fix that will make you audibly say "kill yourself"
class $modify(SuperExtraEvenMoreForcePrioUI, EditorUI) {
    struct Fields {
        std::unordered_set<CCTextInputNode*> forceTouchPrio;
    };

    $override
    bool ccTouchBegan(CCTouch* touch, CCEvent* event) {
        for (auto input : m_fields->forceTouchPrio) {
            if (input->isVisible() && CCRect(
                input->getPosition() - input->getScaledContentSize() / 2,
                input->getScaledContentSize()
            ).containsPoint(input->getParent()->convertTouchToNodeSpace(touch))) {
                return input->ccTouchBegan(touch, event);
            }
        }
        return EditorUI::ccTouchBegan(touch, event);
    }
};

//// Free-standing helpers
std::vector<GameObject*> be::getSelectedObjects(EditorUI* ui) {
    if (ui->m_selectedObject) {
        return std::vector<GameObject*>({ ui->m_selectedObject });
    }
    else {
        return ccArrayToVector<GameObject*>(ui->m_selectedObjects);
    }
}

void be::evilForceTouchPrio(EditorUI* ui, CCTextInputNode* input) {
    static_cast<SuperExtraEvenMoreForcePrioUI*>(ui)->m_fields->forceTouchPrio.insert(input);
}

//// Editor exit events
static Ref<CCLayer> SCENE_TO_RETURN_TO = nullptr;
class $modify(GameManager) {
    $override
    void returnToLastScene(GJGameLevel* level) {
        auto editor = LevelEditorLayer::get();
        if (editor && m_sceneEnum == 3) {
            EditorExitEvent().post();
        }
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

//// View-only editor stuff
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

LevelEditorLayer* be::createViewOnlyEditor(GJGameLevel* level, utils::MiniFunction<CCLayer*()> returnTo) {
    auto editor = LevelEditorLayer::create(level, false);
    editor->setUserObject("view-only-mode"_spr, ViewOnlyModeData::create(returnTo));

    auto viewOnlyLabel = CCLabelBMFont::create("View-Only Mode", "bigFont.fnt");
    viewOnlyLabel->setOpacity(90);
    viewOnlyLabel->setScale(.5f);
    editor->addChildAtPosition(viewOnlyLabel, Anchor::Top, ccp(0, -80), false);

    editor->m_editorUI->showUI(false);
    
    return editor;
}
bool be::isViewOnlyEditor(LevelEditorLayer* lel) {
    return lel->getUserObject("view-only-mode"_spr);
}

class $modify(EditorUI) {
    static void onModify(auto& self) {
        (void)self.setHookPriority("EditorUI::showUI", -100);
    }

    $override
    void clickOnPosition(CCPoint pos) {
        if (!be::isViewOnlyEditor(m_editorLayer)) {
            EditorUI::clickOnPosition(pos);
        }
    }
    $override
    void showUI(bool show) {
        if (be::isViewOnlyEditor(m_editorLayer)) {
            show = false;
            m_tabsMenu->setVisible(false);
        }
        EditorUI::showUI(show);
    }
    $override
    void toggleMode(CCObject* sender) {
        EditorUI::toggleMode(sender);
        if (be::isViewOnlyEditor(m_editorLayer)) {
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

        if (be::isViewOnlyEditor(lel)) {
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

//// More EditCommand options
class $modify(EditorUI) {
    BE_ALLOW_START
    BE_ALLOW_FAKE_ENUMS
    CCPoint moveForCommand(EditCommand command) {
        auto gridSize = EditorUI::get()->m_gridSize;
        switch (command) {
            case EditCommandExt::QuarterLeft:   return ccp(-1 / 4.f, 0) * gridSize;
            case EditCommandExt::QuarterRight:  return ccp( 1 / 4.f, 0) * gridSize;
            case EditCommandExt::QuarterUp:     return ccp(0,  1 / 4.f) * gridSize;
            case EditCommandExt::QuarterDown:   return ccp(0, -1 / 4.f) * gridSize;

            case EditCommandExt::EighthLeft:   return ccp(-1 / 8.f, 0) * gridSize;
            case EditCommandExt::EighthRight:  return ccp( 1 / 8.f, 0) * gridSize;
            case EditCommandExt::EighthUp:     return ccp(0,  1 / 8.f) * gridSize;
            case EditCommandExt::EighthDown:   return ccp(0, -1 / 8.f) * gridSize;

            case EditCommandExt::UnitLeft:     return ccp(-.1f,  0);
            case EditCommandExt::UnitRight:    return ccp( .1f,  0);
            case EditCommandExt::UnitUp:       return ccp( 0,  .1f);
            case EditCommandExt::UnitDown:     return ccp( 0, -.1f);

            default: return EditorUI::moveForCommand(command);
        }
    }
    BE_ALLOW_END
};

//// UI events
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
            this->querySelector("layer-menu hjfod.betteredit/lock-layer")
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
