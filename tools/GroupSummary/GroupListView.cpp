#include "GroupListView.hpp"
#include "MoreTriggersPopup.hpp"

GroupCell::GroupCell(const char* name, CCSize size) :
    TableViewCell(name, size.width, size.height) {}

GroupCell::~GroupCell() {
    CCDirector::sharedDirector()->getTouchDispatcher()->decrementForcePrio(2);
}

void GroupCell::draw() {
    // just call StatsCell::draw, no one will notice
    reinterpret_cast<void(__thiscall*)(GroupCell*)>(
        base + 0x59d40
    )(this);
}

void GroupCell::onShowRestOfTheTriggers(CCObject* pSender) {
    MoreTriggersPopup::create(this->m_pPopup, as<CCNode*>(pSender))->show();
}

void GroupCell::loadFromGroup(int group) {
    this->m_pLayer->setVisible(true);
    this->m_pBGLayer->setOpacity(255);

    bool color = group % 2;

    auto menu = CCMenu::create();
    menu->setPosition(0, 0);
    this->m_pLayer->addChild(menu);
    
    CCDirector::sharedDirector()->getTouchDispatcher()->incrementForcePrio(2);
    this->registerWithTouchDispatcher();

    auto num = CCLabelBMFont::create(std::to_string(group).c_str(), "goldFont.fnt");
    num->setPosition(
        this->m_fHeight / 2,
        this->m_fHeight / 2
    );
    num->setScale(.5f);
    num->setAnchorPoint({ .0f, .5f });
    this->m_pLayer->addChild(num);

    auto info = this->m_pPopup->getGroup(group);

    auto count = CCLabelBMFont::create(
        (
            std::to_string(info.m_vObjects.size()) +
            " obj" +
            (info.m_vObjects.size() == 1 ? "" : "s")
        ).c_str(),
        "bigFont.fnt"
    );
    count->setPosition(
        this->m_fHeight / 2 + 30.f,
        this->m_fHeight / 2
    );
    count->setScale(.375f);
    count->setAnchorPoint({ .0f, .5f });
    this->m_pLayer->addChild(count);

    auto posx = 155.f;
    int showCount = 3;
    int shown = 0;
    for (auto trigger : info.m_vTriggers) {
        if (shown >= showCount) {
            auto moreCount = CCLabelBMFont::create(
                ("+ "_s + std::to_string(info.m_vTriggers.size() - showCount) + " more").c_str(),
                "bigFont.fnt"
            );
            moreCount->setScale(.3f);
            moreCount->setAnchorPoint({ .0f, .5f });

            auto moreBtn = CCMenuItemSpriteExtra::create(
                moreCount, this, menu_selector(GroupCell::onShowRestOfTheTriggers)
            );
            moreBtn->setPosition(posx - 15.f, this->m_fHeight / 2);
            moreBtn->setAnchorPoint({ .0f, .5f });
            moreBtn->setTag(group);
            this->m_pLayer->addChild(moreBtn);

            break;
        }
        auto sprName = ObjectToolbox::sharedState()->intKeyToFrame(trigger->m_nObjectID);
        auto spr = CCSprite::createWithSpriteFrameName(sprName);
        spr->setScale(.65f);
        spr->setPosition({ posx, this->m_fHeight / 2 });
        this->m_pLayer->addChild(spr);
        posx += 30.f;
        shown++;
    }
}

GroupCell* GroupCell::create(const char* key, CCSize size) {
    auto pRet = new GroupCell(key, size);

    if (pRet) {
        return pRet;
    }

    CC_SAFE_DELETE(pRet);
    return nullptr;
}



void GroupListView::setupList() {
    this->m_fItemSeparation = 30.0f;

    if (!this->m_pEntries->count()) return;

    this->m_pTableView->reloadData();

    this->m_pTableView->m_fScrollLimitTop = this->m_fItemSeparation *
        (this->m_pTableView->m_pContentLayer->getScaledContentSize().height / 1500.0f);

    if (this->m_pEntries->count() == 1)
        this->m_pTableView->moveToTopWithOffset(this->m_fItemSeparation);
    
    this->m_pTableView->moveToTop();
}

TableViewCell* GroupListView::getListCell(const char* key) {
    return GroupCell::create(key, { this->m_fWidth, this->m_fItemSeparation });
}

void GroupListView::loadCell(TableViewCell* cell, unsigned int index) {
    as<GroupCell*>(cell)->m_pPopup = this->m_pPopup;
    as<GroupCell*>(cell)->loadFromGroup(
        as<CCInteger*>(this->m_pEntries->objectAtIndex(index))->getValue()
    );
    as<StatsCell*>(cell)->updateBGColor(index);
}

GroupListView* GroupListView::create(
    GroupSummaryPopup* popup,
    CCArray* actions,
    float width,
    float height
) {
    auto pRet = new GroupListView;

    if (pRet) {
        pRet->m_pPopup = popup;
        if (pRet->init(actions, kBoomListType_Group, width, height)) {
            pRet->autorelease();
            return pRet;
        }
    }

    CC_SAFE_DELETE(pRet);
    return nullptr;
}
