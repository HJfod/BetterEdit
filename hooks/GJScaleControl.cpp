#include <GDMake.h>
#include <GUI/CCControlExtension/CCScale9Sprite.h>
#include "passTouch.hpp"
#include "../BetterEdit.hpp"
#include "ScaleTextDelegate.hpp"

using namespace gdmake;

void ScaleTextDelegate::textChanged(gd::CCTextInputNode* input) {
    float val = 1.0f;

    if (input->getString() && strlen(input->getString()))
        val = static_cast<float>(std::atof(input->getString()));

    m_pControl->m_fValue = val;
    m_pControl->m_pSlider->setValue((val - .5f) / 1.5f);

    auto ui = gd::GameManager::sharedState()->getEditorLayer()->getEditorUI();

    if (ui)
        ui->scaleChanged(val);
}

ScaleTextDelegate* ScaleTextDelegate::create(gd::GJScaleControl* c) {
    auto ret = new ScaleTextDelegate();

    if (ret && ret->init()) {
        ret->m_pControl = c;
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}

class GJScaleControl_CB : public gd::GJScaleControl {
    ScaleTextDelegate* getSTD() {
        return reinterpret_cast<ScaleTextDelegate*>(this->getChildByTag(7777));
    }
    
    public:
        void onLockPosition(cocos2d::CCObject* pSender) {
            auto std = getSTD();

            std->m_bLockPosEnabled = !std->m_bLockPosEnabled;

            reinterpret_cast<gd::CCMenuItemSpriteExtra*>(pSender)
                ->setNormalImage(createLockSprite(
                    std->m_bLockPosEnabled ? "GJ_button_02.png" : "GJ_button_04.png",
                    std->m_bLockPosEnabled ? "GJ_lock_001.png" : "GJ_lock_open_001.png"
                ));
            
            if (std->m_bLockPosEnabled)
                patchBytes(0x8f2f9, { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
            else
                patchBytes(0x8f2f9, { 0x56, 0xe8, 0xb1, 0xea, 0xff, 0xff });
        }

        void onUnlockScale(cocos2d::CCObject* pSender) {
            auto std = getSTD();

            std->m_bUnlockScaleEnabled = !std->m_bUnlockScaleEnabled;

            reinterpret_cast<gd::CCMenuItemSpriteExtra*>(pSender)
                ->setNormalImage(createLockSprite(
                    std->m_bUnlockScaleEnabled ? "GJ_button_02.png" : "GJ_button_01.png",
                    std->m_bUnlockScaleEnabled ? "GJ_lock_open_001.png" : "GJ_lock_001.png"
                ));
        }
};

bool pointIntersectsScaleControls(gd::EditorUI* self, cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    auto std = reinterpret_cast<ScaleTextDelegate*>(
        self->m_pScaleControl->getChildByTag(7777)
    );

    if (!self->m_pScaleControl->isVisible()) {
        if (std)
            reinterpret_cast<ScaleTextDelegate*>(std)->m_pInputNode->getTextField()->detachWithIME();
        
        return false;
    }

    if (std) {
        auto inputNodeSize = std->m_pInputNode->getScaledContentSize();
        inputNodeSize.width *= 2;
        inputNodeSize.height *= 1.5f;

        auto inputRect = cocos2d::CCRect {
            self->m_pScaleControl->getPositionX() + std->m_pInputNode->getPositionX() - inputNodeSize.width / 2,
            self->m_pScaleControl->getPositionY() + std->m_pInputNode->getPositionY() - inputNodeSize.height / 2,
            inputNodeSize.width,
            inputNodeSize.height
        };

        auto pos = gd::GameManager::sharedState()
            ->getEditorLayer()
            ->getObjectLayer()
            ->convertTouchToNodeSpace(touch);
        
        if (inputRect.containsPoint(pos))
            std->m_pInputNode->getTextField()->attachWithIME();
        else
            std->m_pInputNode->getTextField()->detachWithIME();

        return inputRect.containsPoint(pos);
    }

    return false;
}

GDMAKE_HOOK(0x94840)
void __fastcall GJScaleControl_ccTouchMoved(gd::GJScaleControl* self_, edx_t edx, cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    auto self = reinterpret_cast<gd::GJScaleControl*>(reinterpret_cast<uintptr_t>(self_) - 0xEC);

    if (self->m_nTouchID == touch->getID()) {
        self->m_pSlider->ccTouchMoved(touch, event);

        float val = roundf((self->m_pSlider->getValue() * 1.5f + .5f) * 100) / 100;

        auto std = reinterpret_cast<ScaleTextDelegate*>(self->getChildByTag(7777));
        auto snap = BetterEdit::sharedState()->getScaleSnap();
        if (snap < 1) snap = 1;

        if (std && !std->m_bUnlockScaleEnabled) {
            val = roundf(val * snap) / snap;
        
            self->m_pSlider->setValue((val - .5f) / 1.5f);
        }

        if (self->m_pDelegate)
            self->m_pDelegate->scaleChanged(val);

        self->m_fValue = val;
        
        self->updateLabel(val);
    }
}

GDMAKE_HOOK(0x94990)
void __fastcall GJScaleControl_updateLabel(gd::GJScaleControl* self) {
    // note: there's a float in xmm0

    GDMAKE_ORIG_V(self);

    auto t = self->getChildByTag(6978);

    std::stringstream stream;
    stream << std::fixed << std::setprecision(2) << self->m_fValue;
    std::string s = stream.str();

    if (t)
        reinterpret_cast<gd::CCTextInputNode*>(t)->setString(s.c_str());
}

GDMAKE_HOOK(0x94590)
void __fastcall GJScaleControl_loadValues(gd::GJScaleControl* self, edx_t edx, gd::GameObject* obj, cocos2d::CCArray* objs) {
    GDMAKE_ORIG_V(self, edx, obj, objs);

    auto scale = 1.5f * self->m_pSlider->getValue() + .5f;

    auto t = self->getChildByTag(6978);

    std::stringstream stream;
    stream << std::fixed << std::setprecision(2) << scale;
    std::string s = stream.str();

    if (t)
        reinterpret_cast<gd::CCTextInputNode*>(t)->setString(s.c_str());
}

GDMAKE_HOOK(0x94490)
bool __fastcall GJScaleControl_init(gd::GJScaleControl* self) {
    if (!GDMAKE_ORIG(self))
        return false;

    auto ed = ScaleTextDelegate::create(self);

    self->m_pLabel->setVisible(false);

    auto spr = cocos2d::extension::CCScale9Sprite::create(
        "square02b_001.png", { 0.0f, 0.0f, 80.0f, 80.0f }
    );

    spr->setScale(.3f);
    spr->setColor({ 0, 0, 0 });
    spr->setOpacity(100);
    spr->setContentSize({ 115.0f, 75.0f });
    spr->setPosition(self->m_pLabel->getPosition());

    auto eLayerInput = gd::CCTextInputNode::create("1.00", ed, "bigFont.fnt", 40.0f, 30.0f);

    eLayerInput->setPosition(self->m_pLabel->getPosition());
    eLayerInput->setLabelPlaceholderColor({ 120, 120, 120 });
    eLayerInput->setAllowedChars(".0123456789");
    eLayerInput->setAnchorPoint({ 0, 0 });
    eLayerInput->setScale(.7f);
    eLayerInput->setDelegate(ed);
    eLayerInput->setTag(6978);
    eLayerInput->setString("1.00");

    self->addChild(spr);
    self->addChild(eLayerInput, 999);
    self->addChild(ed);

    auto menu = cocos2d::CCMenu::create();

    /////////////////////////////////

    auto unlockScaleBtn = gd::CCMenuItemSpriteExtra::create(
        cocos2d::CCSprite::create("GJ_button_01.png"),
        self,
        (cocos2d::SEL_MenuHandler)&GJScaleControl_CB::onUnlockScale
    );

    unlockScaleBtn->setNormalImage(
        createLockSprite("GJ_button_01.png", "GJ_lock_001.png")
    );
    unlockScaleBtn->setPosition({ 30.0f, 0.0f });

    menu->addChild(unlockScaleBtn);

    /////////////////////////////////

    auto lockPositionBtn = gd::CCMenuItemSpriteExtra::create(
        cocos2d::CCSprite::create("GJ_button_01.png"),
        self,
        (cocos2d::SEL_MenuHandler)&GJScaleControl_CB::onLockPosition
    );

    lockPositionBtn->setNormalImage(
        createLockSprite("GJ_button_04.png", "GJ_lock_open_001.png")
    );
    lockPositionBtn->setPosition({ -30.0f, 0.0f });

    menu->addChild(lockPositionBtn);

    /////////////////////////////////

    menu->setPosition(self->m_pLabel->getPosition());
    self->addChild(menu);

    ed->m_pInputNode = eLayerInput;
    ed->m_pLockPosBtn = lockPositionBtn;
    ed->m_pLockScaleBtn = unlockScaleBtn;
    ed->setTag(7777);

    return true;
}
