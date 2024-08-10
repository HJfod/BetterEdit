#include "SupportPopup.hpp"
#include <Geode/utils/web.hpp>
#include <Geode/loader/Mod.hpp>
#include <Geode/binding/ButtonSprite.hpp>
#include <utils/Pro.hpp>

bool SupportPopup::setup(bool showDontShowAgain) {
    m_bgSprite->setVisible(false);

    // This incredibly silly practice is just to avoid CLI build times due to it not caching singular sprites
    auto bg0 = CCSprite::createWithSpriteFrameName("support-shard-0.png"_spr);
    bg0->setAnchorPoint({ 1, 0 });
    m_mainLayer->addChildAtPosition(bg0, Anchor::Center);
    auto bg1 = CCSprite::createWithSpriteFrameName("support-shard-1.png"_spr);
    bg1->setAnchorPoint({ 0, 0 });
    m_mainLayer->addChildAtPosition(bg1, Anchor::Center);
    auto bg2 = CCSprite::createWithSpriteFrameName("support-shard-2.png"_spr);
    bg2->setAnchorPoint({ 1, 1 });
    m_mainLayer->addChildAtPosition(bg2, Anchor::Center);
    auto bg3 = CCSprite::createWithSpriteFrameName("support-shard-3.png"_spr);
    bg3->setAnchorPoint({ 0, 1 });
    m_mainLayer->addChildAtPosition(bg3, Anchor::Center);

    auto invisibleSupportSpr = ButtonSprite::create("Support", "goldFont.fnt", "GJ_button_01.png", .8f);
    invisibleSupportSpr->setScale(1.8f);
    invisibleSupportSpr->setVisible(false);
    auto invisibleSupportBtn = CCMenuItemSpriteExtra::create(
        invisibleSupportSpr, this, menu_selector(SupportPopup::onKofi)
    );
    m_buttonMenu->addChildAtPosition(invisibleSupportBtn, Anchor::Center, ccp(0, 20));


    auto bottomMenu = CCMenu::create();
    bottomMenu->setContentWidth(200);
    bottomMenu->setAnchorPoint({ .5f, .5f });

    auto supportSpr = ButtonSprite::create("Support", "goldFont.fnt", "GJ_button_01.png", .8f);
    auto supportBtn = CCMenuItemSpriteExtra::create(
        supportSpr, this, menu_selector(SupportPopup::onKofi)
    );
    bottomMenu->addChild(supportBtn);

#ifdef BETTEREDIT_PRO
    pro::addSupportPopupStuff(bottomMenu);
#endif

    bottomMenu->setLayout(RowLayout::create()->setDefaultScaleLimits(.1f, 1.f));
    m_mainLayer->addChildAtPosition(bottomMenu, Anchor::Bottom, ccp(0, 18));


    if (showDontShowAgain) {
        auto shutupMenu = CCMenu::create();
        shutupMenu->setContentWidth(100);
        shutupMenu->setAnchorPoint({ 1, .5f });

        auto shutupSpr = ButtonSprite::create("Don't Show Again", "goldFont.fnt", "GJ_button_01.png", .8f);
        auto shutupBtn = CCMenuItemSpriteExtra::create(
            shutupSpr, this, menu_selector(SupportPopup::onDontShowAgain)
        );
        shutupMenu->addChild(shutupBtn);

        shutupMenu->setLayout(RowLayout::create()->setDefaultScaleLimits(.1f, .5f));
        m_mainLayer->addChildAtPosition(shutupMenu, Anchor::TopRight, ccp(15, 5));
    }

    return true;
}

void SupportPopup::onKofi(CCObject*) {
    web::openLinkInBrowser("https://ko-fi.com/HJfod");
}

void SupportPopup::onDontShowAgain(CCObject*) {
    Mod::get()->setSavedValue("dont-show-support-button", true);
    this->onClose(nullptr);
    if (auto btn = CCScene::get()->querySelector("support-be-btn"_spr)) {
        btn->removeFromParent();
    }
    FLAlertLayer::create(
        "Support",
        "If you want to <cy>support BetterEdit in the future</c>, you can find the "
        "support button in the <cj>About</c> popup!\n\n"
        "<cp>This button won't be shown again :)</c>",
        "OK"
    )->show();
}

SupportPopup* SupportPopup::create(bool showDontShowAgain) {
    auto ret = new SupportPopup();
    if (ret && ret->initAnchored(410, 270, showDontShowAgain)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}
