#include "BESettingsLayer.hpp"

using namespace gdmake;
using namespace gdmake::extra;
using namespace gd;
using namespace cocos2d;

void BESettingsLayer::setup() {
    this->m_pPrevPageBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"),
        this,
        (SEL_MenuHandler)&BESettingsLayer::onPage
    );
    this->m_pPrevPageBtn->setUserData(as<void*>(-1));
    this->m_pPrevPageBtn->setPosition(- this->m_pLrSize.width / 2 - 64.0f, 0.0f);
    this->m_pButtonMenu->addChild(this->m_pPrevPageBtn, 150);

    this->m_pNextPageBtn = CCMenuItemSpriteExtra::create(
        CCNodeConstructor()
            .fromFrameName("GJ_arrow_01_001.png")
            .flipX()
            .done(),
        this,
        (SEL_MenuHandler)&BESettingsLayer::onPage
    );
    this->m_pNextPageBtn->setUserData(as<void*>(1));
    this->m_pNextPageBtn->setPosition(this->m_pLrSize.width / 2 + 64.0f, 0.0f);
    this->m_pButtonMenu->addChild(this->m_pNextPageBtn, 150);

    this->addInput("Scale Snap:", "scale-snap");

    this->incrementPageCount(true);
    this->addButton(
        CCNodeConstructor<CCLabelBMFont*>()
            .fromText("Developed by HJfod", "goldFont.fnt")
            .color({ 4, 255, 255 })
            .scale(.75f)
            .done(),
        (SEL_MenuHandler)&BESettingsLayer::onShowHJLinks,
        true
    );
    this->addTitle("Special Thanks:", "bigFont.fnt")->setScale(.6f);
    this->addSubtitle("Mat", true);
    this->addSubtitle("Figment", true);
    this->addSubtitle("cos8oih", true);
    this->addSubtitle("Brittank88", true);
}


cocos2d::CCPoint BESettingsLayer::getItemPos(bool large, bool center) {
    if (large) {
        auto y = this->m_pLrSize.height / 2 - 64.0f - ((this->m_nItemCount + 1) / 2) * s_fItemPaddingV;

        this->m_nItemCount += 2 + (this->m_nItemCount % 2);

        return { 0.0f, y };
    }
    
    auto x = this->m_nItemCount % 2 ? 0.0f : (- this->m_pLrSize.width / 2 + (center ? 0.0f : 24.0f));
    auto y = this->m_pLrSize.height / 2 - 64.0f - (this->m_nItemCount / 2) * s_fItemPaddingV;

    this->m_nItemCount++;

    return { x, y };
}

void BESettingsLayer::incrementPageCount(bool skip) {
    if (skip || this->m_nItemCount >= s_nMaxItemsOnPage) {
        this->m_nDestPage++;
        this->m_nItemCount = 0;
    }
}

void BESettingsLayer::addItem(CCNode* item) {
    if (this->m_nDestPage > this->m_vPages.size() - 1 || !this->m_vPages.size())
        this->m_vPages.push_back(std::vector<CCNode*>());

    this->m_vPages[this->m_nDestPage].push_back(item);
    item->setVisible(!this->m_nDestPage);
}

void BESettingsLayer::addInput(const char* text, const char* key, std::string const& filter) {
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    
    auto y = getItemPos(true).y;

    auto scaleSnapInput = InputNode::create(50.0f, "0");
    auto scaleSnapLabel = CCLabelBMFont::create(text, "goldFont.fnt");

    scaleSnapInput->setPosition(winSize.width / 2 + 60.0f, winSize.height / 2 + y);
    scaleSnapLabel->setPosition(winSize.width / 2 - 30.0f, winSize.height / 2 + y);
    scaleSnapLabel->limitLabelWidth(this->m_pLrSize.width / 2, .7f, .2f);
    scaleSnapInput->getInputNode()->setAllowedChars(filter);

    if (key && strlen(key)) {
        scaleSnapInput->setString(std::to_string(BetterEdit::sharedState()->getKeyInt(key)).c_str());
        scaleSnapInput->getInputNode()->setUserObject(CCString::create(key));
        scaleSnapInput->getInputNode()->setDelegate(this);
    }

    this->m_pLayer->addChild(scaleSnapInput);
    this->m_pLayer->addChild(scaleSnapLabel);

    this->addItem(scaleSnapInput);
    this->addItem(scaleSnapLabel);
    this->incrementPageCount();
}

CCLabelBMFont* BESettingsLayer::addTitle(const char* text, const char* font) {
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    auto y = getItemPos(true).y;

    auto titleLabel = CCLabelBMFont::create(text, font);
    titleLabel->setPosition(winSize.width / 2, winSize.height / 2 + y);
    titleLabel->limitLabelWidth(this->m_pLrSize.width - 64.0f, .9f, .2f);
    this->m_pLayer->addChild(titleLabel);

    this->addItem(titleLabel);
    this->incrementPageCount();

    return titleLabel;
}

CCLabelBMFont* BESettingsLayer::addSubtitle(const char* text, bool center) {
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    auto [x, y] = getItemPos(false, true);

    auto subtitleLabel = CCLabelBMFont::create(text, "goldFont.fnt");
    subtitleLabel->limitLabelWidth(this->m_pLrSize.width / 2 - 64.0f, .7f, .2f);
    subtitleLabel->setPosition(
        winSize.width / 2 + x + (center ? this->m_pLrSize.width / 4 : subtitleLabel->getScaledContentSize().width / 2),
        winSize.height / 2 + y
    );
    this->m_pLayer->addChild(subtitleLabel);

    this->addItem(subtitleLabel);
    this->incrementPageCount();

    return subtitleLabel;
}

CCMenuItemSpriteExtra* BESettingsLayer::addButton(CCNode* sprite, SEL_MenuHandler callback, bool large) {
    auto [x, y] = getItemPos(large);

    auto btn = CCMenuItemSpriteExtra::create(sprite, this, callback);
    btn->setPosition(x, y);
    this->m_pButtonMenu->addChild(btn);

    this->addItem(btn);
    this->incrementPageCount();

    return btn;
}

void BESettingsLayer::addToggle(const char* text, const char* desc, const char* key) {
    auto toggle = CCMenuItemToggler::createWithStandardSprites(
        this,
        (SEL_MenuHandler)&BESettingsLayer::onToggle,
        .75f
    );

    auto label = CCLabelBMFont::create(text, "bigFont.fnt");
    label->limitLabelWidth(this->m_pLrSize.width / 2 - 60.0f, .6f, .2f);

    auto [x, y] = getItemPos(false);

    toggle->setPosition(x, y);
    label->setPosition(x + 16.0f + label->getScaledContentSize().width / 2, y);

    if (key != nullptr) {
        toggle->toggle(BetterEdit::sharedState()->getKeyInt(key));
        toggle->setUserObject(CCString::create(key));
    }

    this->m_pButtonMenu->addChild(toggle);
    this->m_pButtonMenu->addChild(label);

    this->addItem(toggle);
    this->addItem(label);
    this->incrementPageCount();
}


void BESettingsLayer::onPage(CCObject* pSender) {
    for (auto page : this->m_vPages)
        for (auto item : page)
            item->setVisible(false);
    
    auto add = as<int>(as<CCNode*>(pSender)->getUserData());
    this->m_nCurrentPage += add;

    if (this->m_nCurrentPage > (int)this->m_vPages.size() - 1)
        this->m_nCurrentPage = 0;
    else if (this->m_nCurrentPage < 0)
        this->m_nCurrentPage = this->m_vPages.size() - 1;

    for (auto item : this->m_vPages[this->m_nCurrentPage])
        item->setVisible(true);
}

void BESettingsLayer::onToggle(CCObject* pSender) {
    auto toggle = as<CCMenuItemToggler*>(pSender);

    if (toggle && toggle->getUserObject())
        BetterEdit::sharedState()->setKeyInt(
            as<CCString*>(toggle->getUserObject())->getCString(),
            !toggle->isToggled()
        );
}

void BESettingsLayer::textChanged(gd::CCTextInputNode* input) {
    if (input && input->getUserObject())
        BetterEdit::sharedState()->setKeyInt(
            as<CCString*>(input->getUserObject())->getCString(),
            strlen(input->getString()) ? std::atoi(input->getString()) : 0
        );
}

void BESettingsLayer::onShowHJLinks(CCObject*) {
    ProfilePage::create(104257, true)->show();
}

BESettingsLayer* BESettingsLayer::create() {
    auto ret = new BESettingsLayer();

    if (ret && ret->init(340.0f, 240.0f, "GJ_square01.png", "BetterEdit Settings")) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}
