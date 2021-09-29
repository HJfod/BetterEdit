#include "GroupSummaryPopup.hpp"

static constexpr const cocos2d::ccColor3B listBGLight { 102, 68, 28 };
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

    this->updatePage();
}

void GroupSummaryPopup::updatePage() {
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    for (auto ix = 0; ix < m_nItemCount; ix++) {
        auto i = ix + m_nPage * m_nItemCount;

        auto x = winSize.width / 2;
        auto y = winSize.height / 2 + this->m_pLrSize.height / 2 - 50.f - 30.f * ix;
        bool color = i % 2;

        auto bg = CCScale9Sprite::create(
            "square02b_001.png", { 0.0f, 0.0f, 80.0f, 80.0f }
        );
        bg->setPosition(CCPoint { x, y });
        bg->setScale(.5f);
        bg->setColor(color ? listBGLight : listBGDark);
        bg->setContentSize({ m_fItemWidth, 60.f });
        bg->setZOrder(-static_cast<int>(color));
        this->m_pLayer->addChild(bg);

        auto num = CCLabelBMFont::create(std::to_string(ix + 1).c_str(), "goldFont.fnt");
        num->setPosition(x - m_fItemWidth / 2 + 15.f, y);
        num->setScale(.5f);
        num->setAnchorPoint({ .0f, .5f });
        this->m_pLayer->addChild(num);

        this->m_vPageContent.push_back(bg);
        this->m_vPageContent.push_back(num);
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
        ret->init(460.0f, 260.0f, "GJ_square01.png", "Group Summary")
    ) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return ret;
}
