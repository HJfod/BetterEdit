#include "SupportPopup.hpp"
#include <Geode/utils/web.hpp>
#include <Geode/loader/Mod.hpp>
#include <Geode/binding/ButtonSprite.hpp>

bool SupportPopup::setup(bool showDontShowAgain) {
    m_bgSprite->setVisible(false);

    auto bg = CCSprite::create("support-popup.png"_spr);
    m_mainLayer->addChildAtPosition(bg, Anchor::Center);

    auto invisibleSupportSpr = ButtonSprite::create("Support", "goldFont.fnt", "GJ_button_01.png", .8f);
    invisibleSupportSpr->setScale(1.8f);
    invisibleSupportSpr->setVisible(false);
    auto invisibleSupportBtn = CCMenuItemSpriteExtra::create(
        invisibleSupportSpr, this, menu_selector(SupportPopup::onKofi)
    );
    m_buttonMenu->addChildAtPosition(invisibleSupportBtn, Anchor::Center, ccp(0, 20));


    auto bottomMenu = CCMenu::create();
    bottomMenu->setContentWidth(100);
    bottomMenu->setAnchorPoint({ .5f, .5f });

    auto supportSpr = ButtonSprite::create("Support", "goldFont.fnt", "GJ_button_01.png", .8f);
    auto supportBtn = CCMenuItemSpriteExtra::create(
        supportSpr, this, menu_selector(SupportPopup::onKofi)
    );
    bottomMenu->addChild(supportBtn);

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
