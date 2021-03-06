#include "levelPercent.hpp"
#include <InputPrompt.hpp>

static constexpr const int SLIDERLABEL_TAG = 420;
static constexpr const int EPOSITION_TAG = 421;
float g_lastObjectPosX = 0.0f;
bool g_doUpdate = false;

class EditorUI_CB : public EditorUI {
    public:
        void onGoToPercentage(CCObject* pSender) {
            auto p = InputPrompt::create("Go To %", "%", [this](const char* txt) -> void {
                if (txt && strlen(txt)) {
                    float val = 0.0f;
                    try { val = std::stof(txt); } catch(...) {}

                    auto width = CCDirector::sharedDirector()->getWinSize().width;

                    this->m_pEditorLayer->getObjectLayer()->setPosition({
                        (- g_lastObjectPosX * min(val, 100.0f) / 100.0f + width / 2) *
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
};

void updatePercentLabelPosition(EditorUI* self) {
    auto menu = as<CCMenu*>(self->m_pPositionSlider->getChildByTag(SLIDERLABEL_TAG));

    if (menu) {
        auto label = as<CCLabelBMFont*>(menu->getUserData());

        menu->setPositionX(self->m_pPositionSlider->m_pTouchLogic->m_pThumb->getPositionX());

        float val = 0.0f;
        if (g_lastObjectPosX)
            val =
                self->m_pEditorLayer->getObjectLayer()->convertToNodeSpace(
                    CCDirector::sharedDirector()->getWinSize() / 2
                ).x / g_lastObjectPosX * 100.0f;
        
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

void editorHasBeenTouched(bool up) {
    g_doUpdate = up;
}

// void showPositionLabel(EditorUI* self, bool show) {
//     if (BetterEdit::getDisableEditorPos())
//         CATCH_NULL(as<CCLabelBMFont*>(self->getChildByTag(EPOSITION_TAG)))->setVisible(false);
//     else
//         CATCH_NULL(as<CCLabelBMFont*>(self->getChildByTag(EPOSITION_TAG)))->setVisible(show);
    
//     auto menu = as<CCMenu*>(self->m_pPositionSlider->getChildByTag(SLIDERLABEL_TAG));
//     menu->setVisible(!BetterEdit::getDisablePercentage());
// }

GDMAKE_HOOK(0x162650)
void __fastcall LevelEditorLayer_addSpecial(LevelEditorLayer* self, edx_t edx, GameObject* obj) {
    GDMAKE_ORIG_V(self, edx, obj);

    if (g_doUpdate) {
        g_lastObjectPosX = self->getLastObjectX();
    
        updatePercentLabelPosition(self->m_pEditorUI);
    }
}

GDMAKE_HOOK(0x161cb0)
void __fastcall LevelEditorLayer_removeObject(LevelEditorLayer* self, edx_t edx, GameObject* obj, bool idk) {
    GDMAKE_ORIG_V(self, edx, obj, idk);

    if (g_doUpdate) {
        g_lastObjectPosX = self->getLastObjectX();
    
        updatePercentLabelPosition(self->m_pEditorUI);
    }
}

GDMAKE_HOOK(0x78f10)
void __fastcall EditorUI_updateSlider(EditorUI* self) {
    GDMAKE_ORIG_V(self);

    updatePercentLabelPosition(self);
}

GDMAKE_HOOK(0x78cc0)
void __fastcall EditorUI_sliderChanged(EditorUI* self, edx_t edx, Slider* pSlider) {
    GDMAKE_ORIG_V(self, edx, pSlider);

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

    g_lastObjectPosX = self->m_pEditorLayer->getLastObjectX();

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
