#include <GDMake.h>
#include <GUI/CCControlExtension/CCScale9Sprite.h>
#include "../tools/SliderImprovement/passTouch.hpp"

using namespace gdmake;

class RotateTextDelegate : public cocos2d::CCNode, public gd::TextInputDelegate {
    public:
        gd::GJRotationControl* m_pControl;
        gd::CCTextInputNode* m_pInputNode;
        gd::CCMenuItemSpriteExtra* m_pLockPosBtn;
        gd::CCMenuItemSpriteExtra* m_pLockScaleBtn;
        bool m_bLockPosEnabled;
        bool m_bUnlockScaleEnabled;

        virtual void textChanged(gd::CCTextInputNode* input) override {
            float val = 1.0f;

            if (input->getString() && strlen(input->getString()))
                val = static_cast<float>(std::atof(input->getString()));
        }

        static RotateTextDelegate* create(gd::GJRotationControl* c) {
            auto ret = new RotateTextDelegate();

            if (ret && ret->init()) {
                ret->m_pControl = c;
                ret->autorelease();
                return ret;
            }

            CC_SAFE_DELETE(ret);
            return nullptr;
        }
};

bool pointIntersectsRotateControls(gd::EditorUI* self, cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    auto rtd = reinterpret_cast<RotateTextDelegate*>(
        self->m_pScaleControl->getChildByTag(7777)
    );

    if (!self->m_pScaleControl->isVisible()) {
        if (rtd)
            reinterpret_cast<RotateTextDelegate*>(rtd)->m_pInputNode->getTextField()->detachWithIME();
        
        return false;
    }

    if (rtd) {
        auto inputNodeSize = rtd->m_pInputNode->getScaledContentSize();
        inputNodeSize.width *= 2;
        inputNodeSize.height *= 1.5f;

        auto inputRect = cocos2d::CCRect {
            self->m_pScaleControl->getPositionX() + rtd->m_pInputNode->getPositionX() - inputNodeSize.width / 2,
            self->m_pScaleControl->getPositionY() + rtd->m_pInputNode->getPositionY(),
            inputNodeSize.width,
            inputNodeSize.height
        };

        auto pos = gd::GameManager::sharedState()
            ->getEditorLayer()
            ->getObjectLayer()
            ->convertTouchToNodeSpace(touch);
        
        if (inputRect.containsPoint(pos))
            rtd->m_pInputNode->getTextField()->attachWithIME();
        else
            rtd->m_pInputNode->getTextField()->detachWithIME();

        return inputRect.containsPoint(pos);
    }

    return false;
}

// GDMAKE_HOOK(0x94020, "_ZN17GJRotationControl20updateSliderPositionEN7cocos2d7CCPointE")
// void __fastcall GJRotationControl_updateSliderPosition(gd::GJRotationControl* self, edx_t edx, cocos2d::CCPoint pos) {
//     GDMAKE_ORIG_V(self, edx, pos);

//     auto t = self->getChildByTag(6978);

//     std::stringstream stream;
//     stream << std::fixed << std::setprecision(2) << self->m_fAngle;
//     std::string s = stream.str();

//     if (t)
//         reinterpret_cast<gd::CCTextInputNode*>(t)->setString(s.c_str());
// }

// GDMAKE_HOOK(0x8fe70, "_ZN8EditorUI23activateRotationControlEPN7cocos2d8CCObjectE")
// void __fastcall EditorUI_activateRotationControl(gd::EditorUI* self, edx_t edx, cocos2d::CCObject* pSender) {
//     GDMAKE_ORIG_V(self, edx, pSender);

//     auto sel = self->getSelectedObjects();

//     float angle = 0.0f;
//     for (auto ix = 0u; ix < sel->count(); ix++) {
//         auto obj = reinterpret_cast<gd::GameObject*>(sel->objectAtIndex(ix));

//         if (ix) {
//             if (obj->getRotation() != angle)
//                 angle = 0.0f;
//         } else
//             angle = obj->getRotation();
//     }

//     while (angle < 0.0f)
//         angle += 360.0f;
    
//     while (angle >= 360.0f)
//         angle -= 360.0f;

//     std::cout << "setting angle to " << angle << "\n";
//     std::cout << "obj angle " << self->m_pRotationControl->m_fObjAngle << "\n";

//     self->m_pRotationControl->setAngle(angle);
// }

// GDMAKE_HOOK(0x93f10, "_ZN17GJRotationControl4initEv")
// bool __fastcall GJRotationControl_init(gd::GJRotationControl* self) {
//     if (!GDMAKE_ORIG(self))
//         return false;

//     constexpr float offset_w = 0.0f;
//     constexpr float offset_h = -80.0f;

//     auto ed = RotateTextDelegate::create(self);

//     auto spr = cocos2d::extension::CCScale9Sprite::create(
//         "square02b_001.png", { 0.0f, 0.0f, 80.0f, 80.0f }
//     );

//     spr->setScale(.3f);
//     spr->setColor({ 0, 0, 0 });
//     spr->setOpacity(100);
//     spr->setContentSize({ 115.0f, 75.0f });
//     spr->setPosition(self->getPosition() + cocos2d::CCPoint { offset_w, offset_h });

//     auto eLayerInput = gd::CCTextInputNode::create("0", ed, "bigFont.fnt", 40.0f, 30.0f);

//     eLayerInput->setPosition(self->getPosition() + cocos2d::CCPoint { offset_w, offset_h });
//     eLayerInput->setLabelPlaceholderColor({ 120, 120, 120 });
//     eLayerInput->setAllowedChars(".0123456789");
//     eLayerInput->setAnchorPoint({ 0, 0 });
//     eLayerInput->setScale(.7f);
//     eLayerInput->setDelegate(ed);
//     eLayerInput->setTag(6978);
//     eLayerInput->setString("0");

//     self->addChild(spr);
//     self->addChild(eLayerInput, 999);
//     self->addChild(ed);

//     auto menu = cocos2d::CCMenu::create();

//     /////////////////////////////////

//     auto unlockScaleBtn = gd::CCMenuItemSpriteExtra::create(
//         cocos2d::CCSprite::create("GJ_button_01.png"),
//         self,
//         nullptr
//     );

//     unlockScaleBtn->setNormalImage(
//         createLockSprite("GJ_button_01.png", "GJ_lock_001.png")
//     );
//     unlockScaleBtn->setPosition({ 30.0f, 0.0f });

//     menu->addChild(unlockScaleBtn);

//     /////////////////////////////////

//     auto lockPositionBtn = gd::CCMenuItemSpriteExtra::create(
//         cocos2d::CCSprite::create("GJ_button_01.png"),
//         self,
//         nullptr
//     );

//     lockPositionBtn->setNormalImage(
//         createLockSprite("GJ_button_04.png", "GJ_lock_open_001.png")
//     );
//     lockPositionBtn->setPosition({ -30.0f, 0.0f });

//     menu->addChild(lockPositionBtn);

//     /////////////////////////////////

//     menu->setPosition(self->getPosition() + cocos2d::CCPoint { offset_w, offset_h });
//     self->addChild(menu);

//     ed->m_pInputNode = eLayerInput;
//     ed->m_pLockPosBtn = lockPositionBtn;
//     ed->m_pLockScaleBtn = unlockScaleBtn;
//     ed->setTag(7777);

//     return true;
// }
