#include "levelPercent.hpp"
#include <InputPrompt.hpp>
#include "../RotateSaws/rotateSaws.hpp"
#include "../AutoSave/autoSave.hpp"
#include "../CustomKeybinds/BEKeybinds.hpp"
#include "../EditorLayerInput/editorLayerInput.hpp"
#include "../ContextMenu/loadContextMenu.hpp"
#include "../../utils/moveGameLayer.hpp"

static constexpr const int SLIDERLABEL_TAG = 420;
static constexpr const int EPOSITION_TAG = 421;
GameObject* g_lastObject = nullptr;
int g_bDontUpdateSlider = 0;

void updateLastObjectX(LevelEditorLayer* lel, GameObject* obj = nullptr);

float getLevelLength() {
    // from camden314/gdp/finished_works/PlayLayer/createObjectsFromSetup.cpp

    float screenEnd = CCDirector::sharedDirector()->getScreenRight() + 300.0f;
    auto res = screenEnd;
    
    // g_lastObject will sometimes be a released
    // objects;
    // i think we *might* want to hook undos /
    // redos to account for that
    // or just call retain on it and check the 
    // retain count to see if it should be
    // released lol
    if (g_lastObject) {
        if (g_lastObject->retainCount() == 1u) {
            g_lastObject->release();
            g_lastObject = nullptr;
            updateLastObjectX(LevelEditorLayer::get());
        } else {
            res = g_lastObject->getPositionX() + 340.0f;
        }
    }

    if (res < screenEnd)
        res = screenEnd;
    
    return res;
}

void EditorUI_CB::onGoToPercentage(CCObject* pSender) {
    auto p = InputPrompt::create("Go To %", "%", [this](const char* txt) -> void {
        if (txt && strlen(txt)) {
            float val = 0.0f;
            try { val = std::stof(txt); } catch(...) {}

            auto width = CCDirector::sharedDirector()->getWinSize().width;

            this->m_pEditorLayer->getObjectLayer()->setPosition({
                (- getLevelLength() * min(val, 100.0f) / 100.0f + width / 2) *
                    this->m_pEditorLayer->getObjectLayer()->getScale(),
                this->m_pEditorLayer->getObjectLayer()->getPositionY()
            });

            this->constrainGameLayerPosition();
            this->updateSlider();
        }
    }, "Go");
    p->getInputNode()->getInputNode()->setAllowedChars("0123456789.");
    p->getInputNode()->getInputNode()->setMaxLabelLength(6);
    p->show();
}

void updateLastObjectX(LevelEditorLayer* lel, GameObject* obj) {
    if (obj == nullptr) {
        CCARRAY_FOREACH_B_TYPE(lel->m_pObjects, pObj, GameObject) {
            if (!g_lastObject) {
                g_lastObject = pObj;
                continue;
            }

            if (pObj->getPositionX() > g_lastObject->getPositionX())
                g_lastObject = pObj;
        }
    } else {
        if (!g_lastObject)
            g_lastObject = obj;

        if (obj->getPositionX() > g_lastObject->getPositionX())
            g_lastObject = obj;
    }

    if (g_lastObject)
        g_lastObject->retain();
}

void resetSliderPercent(EditorUI* self) {
    g_lastObject = nullptr;
}

void updatePercentLabelPosition(EditorUI* self) {
    if (!self) return;

    auto menu = as<CCMenu*>(self->m_pPositionSlider->getChildByTag(SLIDERLABEL_TAG));

    if (menu) {
        auto label = as<CCLabelBMFont*>(menu->getUserData());

        if (BetterEdit::getDisablePercentage())
            return menu->setVisible(false);
        menu->setVisible(true);

        menu->setPositionX(self->m_pPositionSlider->m_pTouchLogic->m_pThumb->getPositionX());

        float val = 0.0f;
        if (!BetterEdit::getUseOldProgressBar()) {
            val = self->m_pPositionSlider->getValue() * 100.0f;
        } else if (getLevelLength())
            val =
                self->m_pEditorLayer->getObjectLayer()->convertToNodeSpace(
                    CCDirector::sharedDirector()->getWinSize() / 2
                ).x / getLevelLength() * 100.0f;
        
        val = min(val, 100.0f);
        val = max(val, 0.0f);

        label->setString((BetterEdit::formatToString(val, BetterEdit::getPercentageAccuracy()) + "%").c_str());
        label->setOpacity(255);

        if (BetterEdit::getFadeOutPercentage() && !BetterEdit::getDisablePercentage()) {
            label->stopAllActions();
            label->runAction(CCSequence::create(CCArray::create(
                CCDelayTime::create(.5f),
                CCFadeOut::create(.5f),
                nullptr
            )));
        }
    }

    auto posLabel = as<CCLabelBMFont*>(self->getChildByTag(EPOSITION_TAG));

    if (posLabel) {
        auto pos = self->m_pEditorLayer->getObjectLayer()->getPosition() /
            self->m_pEditorLayer->getObjectLayer()->getScale() -
            CCDirector::sharedDirector()->getWinSize() / 2;

        posLabel->setString(
            (std::to_string(-static_cast<int>(pos.x)) + "," + std::to_string(-static_cast<int>(pos.y))).c_str()
        );
    }
}

/*
void showPositionLabel(EditorUI* self, bool show) {
    if (BetterEdit::getDisableEditorPos())
        CATCH_NULL(as<CCLabelBMFont*>(self->getChildByTag(EPOSITION_TAG)))->setVisible(false);
    else
        CATCH_NULL(as<CCLabelBMFont*>(self->getChildByTag(EPOSITION_TAG)))->setVisible(show);
    
    auto menu = as<CCMenu*>(self->m_pPositionSlider->getChildByTag(SLIDERLABEL_TAG));
    menu->setVisible(!BetterEdit::getDisablePercentage());
} //*/

void setIgnoreNewObjectsForSliderPercent(bool b) {
    if (b) g_bDontUpdateSlider++;
    else if (g_bDontUpdateSlider > 0)
        g_bDontUpdateSlider--;
}

GDMAKE_HOOK(0x78cc0, "_ZN8EditorUI13sliderChangedEPN7cocos2d8CCObjectE")
void __fastcall EditorUI_sliderChanged(EditorUI* self, edx_t edx, Slider* pSlider) {
    if (BetterEdit::getUseOldProgressBar()) {
        GDMAKE_ORIG_V(self, edx, pSlider);
        updatePercentLabelPosition(self);
        return;
    }

    float val = self->m_pPositionSlider->getValue() * 100.0f;

    auto width = CCDirector::sharedDirector()->getWinSize().width;
    
    float posX = 0.0f;
    posX = (- getLevelLength() * min(val, 100.0f) / 100.0f + width / 2) *
        self->m_pEditorLayer->getObjectLayer()->getScale();

    self->m_pEditorLayer->getObjectLayer()->setPosition({
        posX, self->m_pEditorLayer->getObjectLayer()->getPositionY()
    });

    self->constrainGameLayerPosition();

    updatePercentLabelPosition(self);
}

GDMAKE_HOOK(0x78e30, "_ZN8EditorUI13valueFromXPosEf")
void __fastcall EditorUI_valueFromXPos(EditorUI* self) {
    if (BetterEdit::getUseOldProgressBar())
        return GDMAKE_ORIG_V(self);

    float val = 0.0f;
    if (getLevelLength())
        val =
            self->m_pEditorLayer->getObjectLayer()->convertToNodeSpace(
                CCDirector::sharedDirector()->getWinSize() / 2
            ).x / getLevelLength() * 100.0f;
    
    val = min(val, 100.0f);
    val = max(val, 0.0f);

    val /= 100.0f;

    __asm movss xmm0, val

    return;
}

void handleObjectAddForSlider(LevelEditorLayer* self, GameObject* obj) {
    if (g_bDontUpdateSlider || !self) return;

    if (obj) updateLastObjectX(self, obj);

    updatePercentLabelPosition(self->m_pEditorUI);

    if (self->m_pEditorUI)
        self->m_pEditorUI->updateSlider();
}

GDMAKE_HOOK(0x8ddb0, "_ZN8EditorUI10moveObjectEP10GameObjectN7cocos2d7CCPointE")
void __fastcall EditorUI_moveObject(EditorUI* self, edx_t edx, GameObject* obj, CCPoint pos) {
    // because gd sometimes passes a good ol'
    // nullptr to moveObject when you're
    // free moving and then press undo
    // (yes very specific bugfix)

    if (obj == nullptr)
        return;
    
    GDMAKE_ORIG_V(self, edx, obj, pos);

    if (g_bDontUpdateSlider || !self) return;

    updateLastObjectX(self->m_pEditorLayer, obj);

    updateContextMenu();

    updatePercentLabelPosition(self);

    if (BetterEdit::getMoveCameraWhenMovingObjects())
        focusGameLayerToSelection(self);

    self->updateSlider();
}

GDMAKE_HOOK(0x78f10, "_ZN8EditorUI12updateSliderEv")
void __fastcall EditorUI_updateSlider(EditorUI* self) {
    GDMAKE_ORIG_V(self);

    updatePercentLabelPosition(self);
}

void loadSliderPercent(EditorUI* self) {
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    auto menu = CCMenu::create();
    menu->setTag(SLIDERLABEL_TAG);

    auto label = CCLabelBMFont::create("100%", "bigFont.fnt");

    label->setPosition(0, 0);
    label->setScale(.4f);

    auto btn = CCMenuItemSpriteExtra::create(
        label, self, (SEL_MenuHandler)&EditorUI_CB::onGoToPercentage
    );

    menu->setPosition(self->m_pPositionSlider->m_pTouchLogic->m_pThumb->getPositionX(), -25.0f);
    menu->setUserData(label);
    menu->addChild(btn);
    self->m_pPositionSlider->addChild(menu, 999);

    updateLastObjectX(self->m_pEditorLayer);

    // self->addChild(
    //     CCNodeConstructor<CCLabelBMFont*>()
    //         .fromText("x,x", "bigFont.fnt")
    //         .scale(.3f)
    //         .move(winSize / 2 + CCPoint { -100.0f, 140.0f })
    //         .tag(EPOSITION_TAG)
    //         .done()
    // );

    // showPositionLabel(self, true);
    updatePercentLabelPosition(self);
}
