#include "GJRotationControl.hpp"
#include "../nodes/InputNode.hpp"

bool __fastcall GJRotationControl::ccTouchBeganHook(
    GJRotationControl* _self, edx_t, cocos2d::CCTouch* _touch, cocos2d::CCEvent* _event
) {
    if (ccTouchBegan_(_self, _touch, _event))
        return true;
    
    // return _self->ccTouchBegan(_touch, _event);
    return false;
}

bool __fastcall GJRotationControl::initHook(GJRotationControl* _self) {
    if (!init(_self))
        return false;
    
    auto angleInput = gd::CCTextInputNode::create("Angle", _self, "bigFont.fnt", 120.0f, 30.0f);

    angleInput->setScale(.7f);
    angleInput->setPosition(-10, -75);
    angleInput->setAllowedChars("0123456789.");
    angleInput->setLabelPlaceholderColor({ 170, 170, 170 });
    angleInput->setLabelPlaceholerScale(.5f);
    angleInput->setMaxLabelLength(5);
    angleInput->setMaxLabelScale(1.0f);
    angleInput->setString("0");
    angleInput->setAnchorPoint({ .5f, .5f });

    _self->addChild(angleInput, 1500);

    _self->registerWithTouchDispatcher();
    cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()->incrementForcePrio(2443);
    angleInput->setKeypadEnabled(true);
    angleInput->setKeyboardEnabled(true);
    angleInput->setTouchEnabled(true);
    angleInput->setMouseEnabled(true);
    _self->setTouchEnabled(true);
    _self->setKeyboardEnabled(true);

    return true;
}
