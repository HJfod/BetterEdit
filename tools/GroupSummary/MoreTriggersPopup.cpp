#include "MoreTriggersPopup.hpp"

void MoreTriggersPopup::setup() {
    auto info = this->m_pPopup->m_mGroupInfo[this->m_nGroup];

    constexpr int grid_h = 7;

    auto menu = CCMenu::create();
    menu->setPosition(0, 0);
    this->addChild(menu);

    auto ix = 0;
    for (auto trigger : info.m_vTriggers) {
        auto sprName = ObjectToolbox::sharedState()->intKeyToFrame(trigger->m_nObjectID);

        auto spr = this->m_pPopup->createSpriteForTrigger(trigger, this->m_nGroup);

        auto btn = CCMenuItemSpriteExtra::create(
            spr, this->m_pPopup, menu_selector(GroupSummaryPopup::onViewTrigger)
        );
        btn->setPosition({
            27.5f * (ix % grid_h - (grid_h - 1) / 2.f),
            this->m_obSize.height / 2 - 18.75f - 27.5f * (ix / grid_h)
        });
        btn->setUserObject(trigger);
        menu->addChild(btn);

        ix++;
    }
}

MoreTriggersPopup* MoreTriggersPopup::create(GroupSummaryPopup* popup, CCNode* btn) {
    auto ret = new MoreTriggersPopup;

    auto pos = btn->getParent()->convertToWorldSpace(btn->getPosition());

    auto width = 210.f;
    auto height = 100.f;

    auto info = popup->m_mGroupInfo[btn->getTag()];

    if (info.m_vTriggers.size() > 21) {
        height = 27.5f * info.m_vTriggers.size() / 21 + 100.f;
        width = 27.5f * (info.m_vTriggers.size() - 2) / 7 + 210.f;
    }

    ContextPopupDirection dir = kContextPopupDirectionDown;

    if (pos.y + 120.f > CCDirector::sharedDirector()->getScreenTop()) {
        dir = kContextPopupDirectionUp;
    }

    if (
        ret &&
        (ret->m_pPopup = popup) &&
        (ret->m_nGroup = btn->getTag()) &&
        ret->init(
            pos,
            { 210.f, height },
            dir,
            kContextPopupTypeBrown
        )
    ) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}
