#include "remapHook.hpp"
#include "IDRemapPopup.hpp"

class EditorPauseLayer_CB : public EditorPauseLayer {
    public:
        void onRemapIDs(CCObject*) {
            IDRemapPopup::create()->show();
        }
};

void loadRemapHook(EditorPauseLayer* self) {
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    auto menu = as<CCMenu*>(self->m_pButton0->getParent());

    auto btn = CCMenuItemSpriteExtra::create(
        CCNodeConstructor()
            .fromNode(ButtonSprite::create(
                "Remap IDs", 0x32, true, "bigFont.fnt", "GJ_button_04.png", 30.0, .6f
            ))
            .done(),
        self,
        (SEL_MenuHandler)&EditorPauseLayer_CB::onRemapIDs
    );
    auto keysBtn = extra::getChild<CCNode*>(menu, -4);

    btn->setPosition(keysBtn->getPositionX(),
    keysBtn->getPositionY() + 35.0f);

    menu->addChild(btn);
}
