#include "SupportersPopup.hpp"
#include <Geode/ui/General.hpp>
#include <Geode/binding/ProfilePage.hpp>

using namespace pro;
using namespace pro::server;

bool SupportersPopup::init() {
    if (!Popup::init(358, 270, "GJ_square02.png"))
        return false;

    m_noElasticity = true;

    this->setTitle("BetterEdit Supporters");

    addSideArt(m_mainLayer, SideArt::Top, SideArtStyle::PopupBlue);
    addSideArt(m_mainLayer, SideArt::Bottom, SideArtStyle::PopupGold);

    m_prevPageBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"),
        this, menu_selector(SupportersPopup::onPage)
    );
    m_prevPageBtn->setTag(-1);
    m_buttonMenu->addChildAtPosition(m_prevPageBtn, Anchor::Left, ccp(-20, 0));

    auto nextPageSpr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    nextPageSpr->setFlipX(true);
    m_nextPageBtn = CCMenuItemSpriteExtra::create(
        nextPageSpr, this, menu_selector(SupportersPopup::onPage)
    );
    m_nextPageBtn->setTag(1);
    m_buttonMenu->addChildAtPosition(m_nextPageBtn, Anchor::Right, ccp(20, 0));

    m_supportersList = CCLayerColor::create({ 0, 0, 0, 150 }, m_size.width - 40, m_size.height - 80);
    m_supportersList->ignoreAnchorPointForPosition(false);
    m_supportersList->setAnchorPoint({ .5f, .5f });
    m_supportersList->setLayout(
        RowLayout::create()
            ->setGap(0)
            ->setGrowCrossAxis(true)
            ->setAxisAlignment(AxisAlignment::Start)
            ->setCrossAxisAlignment(AxisAlignment::End)
            ->setCrossAxisOverflow(false)
    );
    m_mainLayer->addChildAtPosition(m_supportersList, Anchor::Center);

    m_loadingCircle = LoadingSpinner::create(45);
    m_mainLayer->addChildAtPosition(m_loadingCircle, Anchor::Center);
    
    m_errorLabel = CCLabelBMFont::create("", "bigFont.fnt");
    m_mainLayer->addChildAtPosition(m_errorLabel, Anchor::Center);

    m_pageLabel = CCLabelBMFont::create("", "bigFont.fnt");
    m_pageLabel->setAnchorPoint({ 1, .5f });
    m_pageLabel->setScale(.35f);
    m_mainLayer->addChildAtPosition(m_pageLabel, Anchor::TopRight, ccp(-20, -20));

    if (HAS_PRO()) {
        m_mySupportMenu = CCMenu::create();
        m_mySupportMenu->setContentWidth(200);
        m_mySupportMenu->setAnchorPoint({ .5f, .5f });

        auto loading = LoadingSpinner::create(20);
        m_mySupportMenu->addChild(loading);

        m_mySupportMenu->setLayout(RowLayout::create()->setGap(-3));
        m_mainLayer->addChildAtPosition(m_mySupportMenu, Anchor::Bottom, ccp(0, 22));

        m_mySupportListener.spawn(
            server::getMySupport(*getProKey()),
            [this](auto value) {
                this->onLoadMySupport(std::move(value));
            }
        );
    }

    this->loadPage(0);

    return true;
}

void SupportersPopup::loadPage(size_t page) {
    m_page = page;
    m_loadingCircle->setVisible(true);
    m_errorLabel->setVisible(false);
    m_supportersList->removeAllChildren();
    this->updatePageInfo();
    m_reqListener.spawn(
        server::getSupporters(page),
        [this](auto value) {
            this->onLoadPage(std::move(value));
        }
    );
}
void SupportersPopup::onLoadPage(Result<Supporters> result) {
    m_loadingCircle->setVisible(false);
    if (result.isOk()) {
        auto data = std::move(result).unwrap();
        m_maxPage = data.totalPublicSupporterCount > 0 ?
            (data.totalPublicSupporterCount - 1) / SUPPORTERS_PER_PAGE : 
            0;
        this->updatePageInfo();

        for (auto supporter : data.supporters) {
            auto supporterCell = CCMenu::create();
            supporterCell->setContentSize({
                m_supportersList->getContentWidth() / 2,
                m_supportersList->getContentHeight() / (SUPPORTERS_PER_PAGE * .5f)
            });
            supporterCell->ignoreAnchorPointForPosition(false);

            auto cube = SimplePlayer::create(supporter.info.cubeID);
            supporter.info.update(cube);
            cube->setScale(.7f);
            supporterCell->addChildAtPosition(cube, Anchor::Left, ccp(supporterCell->getContentHeight() / 2, 0));

            auto name = CCLabelBMFont::create(supporter.info.username.c_str(), "bigFont.fnt");
            name->limitLabelWidth(supporterCell->getContentWidth() - supporterCell->getContentHeight() - 5, .5f, .1f);
            name->setAnchorPoint({ 0, .5f });
            name->setColor(getSupporterColor(supporter.supportedAmount));

            auto nameBtn = CCMenuItemSpriteExtra::create(
                name, this, menu_selector(SupportersPopup::onSupporter)
            );
            nameBtn->setTag(supporter.info.gdAccountID);
            supporterCell->addChildAtPosition(
                nameBtn, Anchor::Left,
                ccp(supporterCell->getContentHeight() + name->getScaledContentWidth() / 2, 0)
            );
            
            m_supportersList->addChild(supporterCell);
        }
        m_supportersList->updateLayout();
    }
    else {
        m_errorLabel->setString(std::move(result).unwrapErr().c_str());
        m_errorLabel->limitLabelWidth(m_size.width - 50, .5f, .1f);
        m_errorLabel->setVisible(true);
    }
}
void SupportersPopup::onLoadMySupport(Result<MySupport> result) {
    m_mySupportMenu->removeChildByID("loading-spinner");
    if (result.isOk()) {
        auto showMeToggle = CCMenuItemToggler::createWithStandardSprites(
            this, menu_selector(SupportersPopup::onShowMe), .5f
        );
        showMeToggle->toggle(result.unwrap().showingPublicly);
        m_mySupportMenu->addChild(showMeToggle);

        auto showMeLabel = CCLabelBMFont::create("Show Me Publicly in This List", "bigFont.fnt");
        showMeLabel->setLayoutOptions(
            AxisLayoutOptions::create()
                ->setScalePriority(1)
                ->setScaleLimits(.1f, .5f)
        );
        m_mySupportMenu->addChild(showMeLabel);
    }
    else {
        log::error("Failed to fetch user info: {}", result.unwrapErr());
        auto errorLabel = CCLabelBMFont::create("Failed to Fetch User Info", "bigFont.fnt");
        errorLabel->setColor(ccc3(255, 55, 0));
        errorLabel->setLayoutOptions(
            AxisLayoutOptions::create()
                ->setScaleLimits(.1f, .5f)
        );
        m_mySupportMenu->addChild(errorLabel);
    }
    m_mySupportMenu->updateLayout();
}
void SupportersPopup::updatePageInfo() {
    m_nextPageBtn->setVisible(m_page < m_maxPage);
    m_prevPageBtn->setVisible(m_page > 0);
    m_pageLabel->setString(fmt::format("Page: {}/{}", m_page + 1, m_maxPage + 1).c_str());
}

void SupportersPopup::onPage(CCObject* sender) {
    auto page = static_cast<int>(m_page) + sender->getTag();
    if (page < 0) {
        page = 0;
    }
    this->loadPage(static_cast<size_t>(page));
}
void SupportersPopup::onSupporter(CCObject* sender) {
    ProfilePage::create(sender->getTag(), false)->show();
}
void SupportersPopup::onShowMe(CCObject* sender) {
    if (auto key = pro::getProKey()) {
        m_updateSupportListener.spawn(
            server::updateSupporter(*key, UpdateSupporter {
                .showPublicly = !static_cast<CCMenuItemToggler*>(sender)->isToggled(),
            }),
            [popup = Ref(this)](auto) {
                popup->loadPage(popup->m_page);
            }
        );
    }
}

SupportersPopup* SupportersPopup::create() {
    auto ret = new SupportersPopup();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}
