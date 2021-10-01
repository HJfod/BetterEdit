#include "GroupSummaryPopup.hpp"
#include "MoreTriggersPopup.hpp"

static constexpr const cocos2d::ccColor3B listBGLight { 142, 68, 28 };
static constexpr const cocos2d::ccColor3B listBGDark  { 114, 55, 22 };

void GroupSummaryPopup::setup() {
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    this->m_bNoElasticity = true;

    this->m_pButtonMenu->addChild(CCNodeConstructor<CCMenuItemSpriteExtra*>()
        .fromNode(CCMenuItemSpriteExtra::create(
            CCNodeConstructor()
                .fromFrameName("GJ_arrow_01_001.png")
                .scale(.8f)
                .done(),
            this,
            (SEL_MenuHandler)&GroupSummaryPopup::onPage
        ))
        .udata(-1)
        .move(- m_fItemWidth / 2 - 25.0f, 0.0f)
        .done()
    );
    this->m_pButtonMenu->addChild(CCNodeConstructor<CCMenuItemSpriteExtra*>()
        .fromNode(CCMenuItemSpriteExtra::create(
            CCNodeConstructor()
                .fromFrameName("GJ_arrow_01_001.png")
                .scale(.8f)
                .flipX()
                .done(),
            this,
            (SEL_MenuHandler)&GroupSummaryPopup::onPage
        ))
        .udata(1)
        .move(m_fItemWidth / 2 + 25.0f, 0.0f)
        .done()
    );

    this->updateGroups();
    this->updatePage();
}

CCNode* GroupSummaryPopup::createItem(int group) {
    auto node = CCNode::create();

    bool color = group % 2;

    auto bg = CCScale9Sprite::create(
        "square02b_001.png", { 0.0f, 0.0f, 80.0f, 80.0f }
    );
    bg->setScale(.5f);
    bg->setColor(color ? listBGLight : listBGDark);
    bg->setContentSize({ m_fItemWidth * 2.f, 60.f });
    bg->setZOrder(static_cast<int>(color));
    node->addChild(bg);

    auto menu = CCMenu::create();
    menu->setPosition(0, 0);
    menu->setZOrder(2);
    node->addChild(menu);

    auto num = CCLabelBMFont::create(std::to_string(group).c_str(), "goldFont.fnt");
    num->setPosition(- m_fItemWidth / 2 + 15.f, 0.f);
    num->setScale(.5f);
    num->setAnchorPoint({ .0f, .5f });
    num->setZOrder(2);
    node->addChild(num);

    auto info = this->m_mGroupInfo[group];

    auto count = CCLabelBMFont::create(
        (
            std::to_string(info.m_vObjects.size()) +
            " obj" +
            (info.m_vObjects.size() == 1 ? "" : "s")
        ).c_str(),
        "bigFont.fnt"
    );
    count->setPosition(- m_fItemWidth / 2 + 45.f, 0.f);
    count->setScale(.375f);
    count->setAnchorPoint({ .0f, .5f });
    count->setZOrder(2);
    node->addChild(count);

    auto posx = - m_fItemWidth / 2 + 155.f;
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
                moreCount, this, menu_selector(GroupSummaryPopup::onShowRestOfTheTriggers)
            );
            moreBtn->setPosition(posx - 15.f, 0.f);
            moreBtn->setAnchorPoint({ .0f, .5f });
            moreBtn->setTag(group);
            menu->addChild(moreBtn);

            break;
        }
        auto sprName = ObjectToolbox::sharedState()->intKeyToFrame(trigger->m_nObjectID);
        auto spr = CCSprite::createWithSpriteFrameName(sprName);
        spr->setScale(.65f);
        spr->setPosition({ posx, 0.f });
        spr->setZOrder(2);
        node->addChild(spr);
        posx += 30.f;
        shown++;
    }

    return node;
}

void GroupSummaryPopup::onShowRestOfTheTriggers(CCObject* pSender) {
    MoreTriggersPopup::create(this, as<CCNode*>(pSender))->show();
}

void GroupSummaryPopup::addGroups(GameObject* obj) {
    for (auto i = 0; i < obj->m_nGroupCount; i++) {
        auto id = obj->m_pGroups[i];

        if (!m_mGroupInfo.count(id))
            m_mGroupInfo[id] = {};
        
        m_mGroupInfo[id].m_vObjects.push_back(obj);
    }
}

void GroupSummaryPopup::addGroups(EffectGameObject* obj) {
    this->addGroups(as<GameObject*>(obj));

    if (obj->m_nTargetGroupID) {
        if (!m_mGroupInfo.count(obj->m_nTargetGroupID))
            m_mGroupInfo[obj->m_nTargetGroupID] = {};
        m_mGroupInfo[obj->m_nTargetGroupID].m_vTriggers.push_back(obj);
    }

    if (obj->m_nCenterGroupID) {
        if (!m_mGroupInfo.count(obj->m_nCenterGroupID))
            m_mGroupInfo[obj->m_nCenterGroupID] = {};
        m_mGroupInfo[obj->m_nCenterGroupID].m_vTriggers.push_back(obj);
    }
}

void GroupSummaryPopup::updateGroups() {
    m_mGroupInfo = {};

    CCARRAY_FOREACH_B_TYPE(LevelEditorLayer::get()->getAllObjects(), obj, GameObject) {
        auto eobj = asEffectGameObject(obj);
        if (eobj) {
            this->addGroups(eobj);
        } else {
            this->addGroups(obj);
        }
    }
}

void GroupSummaryPopup::updatePage() {
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    for (auto ix = 0; ix < m_nItemCount; ix++) {
        auto i = ix + m_nPage * m_nItemCount + 1;

        auto x = winSize.width / 2;
        auto y = winSize.height / 2 + this->m_pLrSize.height / 2 - 60.f - 30.f * ix;

        auto item = this->createItem(i);
        item->setPosition(x, y);
        this->m_pLayer->addChild(item);

        this->m_vPageContent.push_back(item);
    }
}

void GroupSummaryPopup::onPage(CCObject* pSender) {
    this->m_nPage += as<int>(as<CCNode*>(pSender)->getUserData());

    if (this->m_nPage < 0)
        this->m_nPage = 0;
    if (this->m_nPage > 999 / m_nItemCount)
        this->m_nPage = 999 / m_nItemCount;

    for (auto const& node : m_vPageContent) {
        node->removeFromParent();
    }
    this->m_vPageContent.clear();

    this->updatePage();
}

GroupSummaryPopup::~GroupSummaryPopup() {}

GroupSummaryPopup* GroupSummaryPopup::create(LevelEditorLayer* lel) {
    auto ret = new GroupSummaryPopup;

    if (
        ret &&
        (ret->m_pEditor = lel) &&
        ret->init(460.0f, 280.0f, "GJ_square01.png", "Group Summary")
    ) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return ret;
}
