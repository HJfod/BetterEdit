#include "autoCT.hpp"
#include "../../BetterEdit.hpp"
#include "ColorTriggerPopup.hpp"

using namespace gd;
using namespace gdmake;
using namespace gdmake::extra;
using namespace cocos2d;

class EditorPauseLayer_CB : public EditorPauseLayer {
    public:
        void onAutoColorTriggers(CCObject* pSender) {
            ColorTriggerPopup::create(this)->show();
        }
};

void loadColorTriggerButton(EditorPauseLayer* self) {
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    auto menu = as<CCMenu*>(self->m_pButton0->getParent());

    auto btn = CCMenuItemSpriteExtra::create(
        CCNodeConstructor()
            .fromNode(ButtonSprite::create(
                "Create Color\nTriggers", 0x32, true, "bigFont.fnt", "GJ_button_04.png", 30.0, .6f
            ))
            .done(),
        self,
        (SEL_MenuHandler)&EditorPauseLayer_CB::onAutoColorTriggers
    );
    auto buildHelper = extra::getChild<CCNode*>(menu, 8);
    auto keysBtn = extra::getChild<CCNode*>(menu, -3);

    btn->setPosition(
        buildHelper->getPositionX(),
        buildHelper->getPositionY() + 35.0f
    );

    keysBtn->setPosition(
        keysBtn->getPositionX() - 75.0f,
        buildHelper->getPositionY()
    );

    menu->addChild(btn);
}
