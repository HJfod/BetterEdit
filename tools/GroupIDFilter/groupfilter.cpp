#include "groupfilter.hpp"
#include "AdvancedFilterLayer.hpp"

using namespace gd;
using namespace cocos2d;
using namespace gdmake;
using namespace gdmake::extra;

class EditorUI_CB : public EditorUI {
    public:
        void onShowGroupFilter(CCObject* pSender) {
            AdvancedFilterLayer::create(as<CCMenuItemSpriteExtra*>(pSender))->show();
        }
};

void setupGroupFilterButton(gd::EditorUI* self) {
    // center the menu to satisfy OCD
    self->m_pDeleteMenu->setPositionY(43);

    if (self->m_pDeleteMenu->getChildByTag(0x80085))
        // Figment's GroupIDFilter is installed
        return;

    auto btn = CCMenuItemSpriteExtra::create(
        CCNodeConstructor()
            .fromNode(ButtonSprite::create(
                CCNodeConstructor()
                    .fromNode(CCSprite::create())
                    .add(CCNodeConstructor<CCLabelBMFont*>()
                        .fromText("ID", "bigFont.fnt")
                        .scale(.8f)
                        .done())
                    .done(),
                0x20, true, 1.0f, 0, "GJ_button_04.png", true, 0x20
            ))
            .scale(.9f)
            .done(),
        self,
        (SEL_MenuHandler)&EditorUI_CB::onShowGroupFilter
    );

    auto winSize = CCDirector::sharedDirector()->getWinSize();
    auto ratio = winSize.width / winSize.height;

    if (ratio > 1.7f)
        btn->setPosition(
            self->m_pDeleteFilterStatic->getPositionX() + 40.0f,
            self->m_pDeleteFilterStatic->getPositionY()
        );
    else
        btn->setPosition(
            self->m_pDeleteFilterDetails->getPositionX() - 40.0f,
            self->m_pDeleteFilterDetails->getPositionY()
        );
    btn->setTag(0x80085);

    self->m_pDeleteMenu->addChild(btn, 100);
}
