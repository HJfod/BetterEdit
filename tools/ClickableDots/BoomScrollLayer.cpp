#include "../../BetterEdit.hpp"

static constexpr const int MENU_TAG = 0x44;

bool isEasedScrollLayer(BoomScrollLayer* self) {
    if (!self->getParent())
        return false;

    // EditButtonBar
    if (vtable_cast(self->getParent(), 0x297658))
        return false;

    // ListButtonBar
    if (vtable_cast(self->getParent(), 0x28b0d4))
        return false;

    return true;
}

class BoomScrollLayer_CB : public BoomScrollLayer {
    public:
        void onGoToPage(CCObject* pSender) {
            auto p = pSender->getTag();
            if (isEasedScrollLayer(this))
                this->moveToPage(p);
            else {
                this->instantMoveToPage(p - 1);
                this->instantMoveToPage(p);
            }
        }
};

GDMAKE_HOOK(0x11ea0)
void __fastcall BoomScrollLayer_updateDots(BoomScrollLayer* self, edx_t edx, float width) {
    GDMAKE_ORIG_V(self, edx, width);

    if (self->m_pDots && self->m_pDots->count()) {
        auto scale = as<CCSprite*>(self->m_pDots->objectAtIndex(0))->getScale();
        auto menu = self->getChildByTag(MENU_TAG);
        if (menu)
            menu->setPosition(
                self->getScaledContentSize().width / 2,
                8.0f * scale
            );
    }
}

GDMAKE_HOOK(0x11780)
bool __fastcall BoomScrollLayer_init(
    BoomScrollLayer* self,
    edx_t edx,
    CCArray* pages,
    int unk,
    bool bunk,
    CCArray* aunk,
    DynamicScrollDelegate* delegate
) {
    // dont move the dots around in updateDots
    patch(0x11fd2, { 0x90, 0x90, 0x90, 0x90 });

    if (!GDMAKE_ORIG(self, edx, pages, unk, bunk, aunk, delegate))
        return false;

    auto menu = CCMenu::create();

    auto i = 0;
    CCARRAY_FOREACH_B_TYPE(self->m_pDots, dot, CCSprite) {
        dot->retain();
        dot->removeFromParent();
        dot->setPosition({ 0.0f, 0.0f });
        auto dotBtn = CCMenuItemSpriteExtra::create(
            dot, self, menu_selector(BoomScrollLayer_CB::onGoToPage)
        );
        dotBtn->setSizeMult(3.0f);
        dotBtn->setTag(i++);
        dotBtn->setUserData(as<void*>(bunk));
        menu->addChild(dotBtn);
        dot->release();
    }

    menu->alignItemsHorizontallyWithPadding((11.f - self->m_pDots->count()) / 2.2f);
    menu->setPosition(self->getScaledContentSize().width / 2, 6.0f);
    menu->setTag(MENU_TAG);
    self->addChild(menu, 5);

    return true;
}
