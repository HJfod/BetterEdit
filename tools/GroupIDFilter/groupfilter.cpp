#include "groupfilter.hpp"
#include "GroupIDInputLayer.hpp"

using namespace gd;
using namespace cocos2d;
using namespace gdmake;
using namespace gdmake::extra;

class EditorUI_CB : public EditorUI {
    public:
        void onShowGroupFilter(CCObject* pSender) {
            GroupIDInputLayer::create()->show();
        }
};

void setupGroupFilterButton(gd::EditorUI* self) {
    auto btn = CCMenuItemSpriteExtra::create(
        CCNodeConstructor()
            .fromFile("GJ_button_01.png")
            .scale(.8f)
            .add([](auto spr) -> cocos2d::CCNode* {
                return CCNodeConstructor<CCLabelBMFont*>()
                    .fromText("ID", "bigFont.fnt")
                    .move(spr->getContentSize() / 2)
                    .scale(.8f)
                    .done();
            })
            .done(),
        self,
        (SEL_MenuHandler)&EditorUI_CB::onShowGroupFilter
    );

    btn->setPosition(
        self->m_pCopyBtn->getPositionX() - 75.0f,
        self->m_pCopyBtn->getPositionY()
    );
    btn->setTag(7777);

    self->m_pCopyBtn->getParent()->addChild(btn, 100);
}
