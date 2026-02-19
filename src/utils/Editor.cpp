#include "Editor.hpp"
#include "ObjectIDs.hpp"
#include <Geode/modify/GameManager.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/binding/EditButtonBar.hpp>
#include <Geode/binding/GameObject.hpp>
#include <Geode/binding/EffectGameObject.hpp>
#include <Geode/binding/CCTextInputNode.hpp>
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
            if (nodeIsVisible(input) && CCRect(
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
        // todo: remove this stupid bullshit once we bring back a good replacement for ccArrayToVector
        auto ext = CCArrayExt<GameObject*>(ui->m_selectedObjects);
        return std::vector<GameObject*>(ext.begin(), ext.end());
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
            EditorExitEvent().send();
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
    std::function<CCLayer*()> m_returnTo;

public:
    static ViewOnlyModeData* create(std::function<CCLayer*()> returnTo) {
        auto ret = new ViewOnlyModeData();
        ret->m_returnTo = returnTo;
        ret->autorelease();
        return ret;
    }

    CCLayer* getLayerToReturnTo() const {
        return m_returnTo();
    }
};

LevelEditorLayer* be::createViewOnlyEditor(GJGameLevel* level, std::function<CCLayer*()> returnTo) {
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

    #ifdef GEODE_IS_MACOS // toggleMode is inlined into onPlaytest on macOS
    $override
    void onPlaytest(CCObject* sender) {
        auto playbackMode = m_editorLayer->m_playbackMode;
        EditorUI::onPlaytest(sender);
        if (!m_isPaused && playbackMode != PlaybackMode::Playing && be::isViewOnlyEditor(m_editorLayer)) {
            m_editButtonBar->setVisible(false);
            m_createButtonBar->setVisible(false);
            m_deleteMenu->setVisible(false);
            m_tabsMenu->setVisible(false);
        }
    }
    #endif
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
            spr->setColor(ccc3(90, 90, 90));
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
        UIShowEvent(this).send(show);

        m_tabsMenu->setVisible(show && m_selectedMode == 2);
    }
};

class $modify(TintLayer, LevelEditorLayer) {
    struct Fields {
        std::unordered_map<Ref<GameObject>, ccColor3B> tinted {};
    };

    $override
    void updateVisibility(float dt) {
        LevelEditorLayer::updateVisibility(dt);
        for (auto [obj, color] : m_fields->tinted) {
            obj->setObjectColor(color);
            // if (m_detailSprite) {
            //     m_detailSprite->setColor(color);
            //     m_detailSprite->setChildColor(color);
            // }
        }
    }
};

void be::tintObject(GameObject* obj, std::optional<ccColor3B> const& color) {
    auto lel = static_cast<TintLayer*>(LevelEditorLayer::get());
    if (color) {
        lel->m_fields->tinted.insert({ obj, *color });
        obj->setObjectColor(*color);
    }
    else {
        lel->m_fields->tinted.erase(obj);
    }
}

void be::enableButton(CCMenuItemSpriteExtra* btn, bool enabled, bool visualOnly) {
    btn->setEnabled(enabled || visualOnly);
    if (auto spr = typeinfo_cast<CCRGBAProtocol*>(btn->getNormalImage())) {
        spr->setCascadeColorEnabled(true);
        spr->setCascadeOpacityEnabled(true);
        spr->setColor(enabled ? ccWHITE : ccGRAY);
        spr->setOpacity(enabled ? 255 : 200);
    }
}
void be::enableToggle(CCMenuItemToggler* toggle, bool enabled, bool visualOnly) {
    toggle->setEnabled(enabled || visualOnly);
    be::enableButton(toggle->m_onButton, enabled, visualOnly);
    be::enableButton(toggle->m_offButton, enabled, visualOnly);
}

static CCRect calculateCoverageButActuallyGood(CCArray* nodes) {
    if (nodes->count() == 0) {
        return CCRectZero;
    }
    auto result = static_cast<CCNode*>(nodes->firstObject())->boundingBox();
    for (auto node : CCArrayExt<CCNode*>(nodes)) {
        auto rect = node->boundingBox();
        const auto diffMinX = rect.getMinX() - result.getMinX();
        if (diffMinX < 0.f) {
            result.origin.x += diffMinX;
            result.size.width -= diffMinX;
        }
        const auto diffMinY = rect.getMinY() - result.getMinY();
        if (diffMinY < 0.f) {
            result.origin.y += diffMinY;
            result.size.height -= diffMinY;
        }
        result.size.width += std::max(0.f, rect.getMaxX() - result.getMaxX());
        result.size.height += std::max(0.f, rect.getMaxY() - result.getMaxY());
    }
    return result;
}

void be::focusEditor(EditorUI* ui, CCRect const& rect, bool smooth) {
    constexpr int MAGIC_ACTION_TAG = 2348234;

    // todo: all code like this will have to be reworked when i add editor rotation...

    // also i have no clue why these calculations work, 
    // i figured this out through trial and error

    const auto objLayer = ui->m_editorLayer->m_objectLayer;
    const auto winSize = CCDirector::get()->getWinSize();
    const auto viewPortToFitTo = winSize / 3;
    const auto scale = std::clamp(
        std::min(viewPortToFitTo.width / rect.size.width, viewPortToFitTo.height / rect.size.height),
        .25f, 1.5f
    );
    const auto pos = (-ccp(rect.getMidX(), rect.getMidY()) + winSize / 2 / scale) * scale;

    if (smooth) {
        objLayer->stopActionByTag(MAGIC_ACTION_TAG);
        auto action = CCEaseInOut::create(CCSpawn::create(
            CCMoveTo::create(.4f, pos),
            CCScaleTo::create(.4f, scale),
            nullptr
        ), 2);
        action->setTag(MAGIC_ACTION_TAG);
        objLayer->runAction(action);
    }
    else {
        objLayer->setPosition(pos);
        objLayer->setScale(scale);
    }
}
void be::focusEditorOnObjects(EditorUI* ui, CCArray* objs, bool smooth) {
    return focusEditor(ui, calculateCoverageButActuallyGood(objs), smooth);
}

CCArray* be::getObjectsFromGroupDict(CCDictionary* groupDict, int id) {
    // Don't waste time on invalid triggers
    if (id <= 0 || id > 9999) {
        return nullptr;
    }
    auto objs = static_cast<CCArray*>(groupDict->objectForKey(id));
    // Don't waste time on null or empty array
    if (!objs || !objs->count()) {
        return nullptr;
    }
    return objs;
}
static CCPoint doCalculateSlotPosition(
    EffectGameObject* trigger,
    size_t slotCount, size_t slotIndex,
    float sideMultiplier
) {
    constexpr float TRIGGER_NAME_SPACE_OFFSET = 5;
    return trigger->getPosition() + ccp(
        10 * trigger->getScaleX() * sideMultiplier,
        ((slotIndex - static_cast<float>(slotCount - 1) / 2.f) * -10 - TRIGGER_NAME_SPACE_OFFSET) *
            trigger->getScaleY()
    );
}
CCPoint be::InputSlots::calculateSlotPosition(EffectGameObject* trigger, size_t slotIndex) const {
    const size_t slotCount = (targetGroupID.has_value());
    return doCalculateSlotPosition(trigger, slotCount, slotIndex, -1);
}
CCPoint be::OutputSlots::calculateSlotPosition(EffectGameObject* trigger, size_t slotIndex) const {
    const size_t slotCount = (targetGroupID.has_value() + centerGroupID.has_value());
    return doCalculateSlotPosition(trigger, slotCount, slotIndex, 1);
}

std::pair<be::InputSlots, be::OutputSlots> be::getTriggerSlots(EffectGameObject* trigger) {
    using namespace object_ids;
    switch (trigger->m_objectID) {
        case MOVE_TRIGGER: {
            return std::make_pair(
                be::InputSlots {
                    .targetGroupID = SlotType::Generic,
                },
                be::OutputSlots {
                    .targetGroupID = SlotType::Transform,
                    .centerGroupID = std::nullopt,
                }
            );
        } break;

        case ROTATE_TRIGGER: {
            return std::make_pair(
                InputSlots {
                    .targetGroupID = SlotType::Generic,
                },
                OutputSlots {
                    .targetGroupID = SlotType::Transform,
                    .centerGroupID = SlotType::Center,
                }
            );
        } break;

        default: {
            return std::make_pair(
                be::InputSlots {
                    .targetGroupID = SlotType::Generic,
                },
                be::OutputSlots {
                    .targetGroupID = SlotType::Generic,
                    .centerGroupID = SlotType::Generic,
                }
            );
        } break;
    }
}
std::vector<int> be::getTriggerTargetedGroups(EffectGameObject* trigger) {
    auto [_, slots] = getTriggerSlots(trigger);
    std::vector<int> results;
    if (slots.targetGroupID) {
        results.push_back(trigger->m_targetGroupID);
    }
    if (slots.centerGroupID) {
        results.push_back(trigger->m_centerGroupID);
    }
    return results;
}
