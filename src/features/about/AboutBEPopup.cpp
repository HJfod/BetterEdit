#include "AboutBEPopup.hpp"
#include <Geode/ui/GeodeUI.hpp>

bool AboutBEPopup::setup() {
    m_noElasticity = true;

    this->setTitle("About BetterEdit");

    auto cornerBL = CCSprite::createWithSpriteFrameName("dailyLevelCorner_001.png");
    m_mainLayer->addChildAtPosition(cornerBL, Anchor::BottomLeft, ccp(25, 25));

    auto cornerBR = CCSprite::createWithSpriteFrameName("dailyLevelCorner_001.png");
    cornerBR->setFlipX(true);
    m_mainLayer->addChildAtPosition(cornerBR, Anchor::BottomRight, ccp(-25, 25));

    auto cornerTL = CCSprite::createWithSpriteFrameName("rewardCorner_001.png");
    cornerTL->setFlipY(true);
    m_mainLayer->addChildAtPosition(cornerTL, Anchor::TopLeft, ccp(25, -25));

    auto cornerTR = CCSprite::createWithSpriteFrameName("rewardCorner_001.png");
    cornerTR->setFlipY(true);
    cornerTR->setFlipX(true);
    m_mainLayer->addChildAtPosition(cornerTR, Anchor::TopRight, ccp(-25, -25));

    auto developedBy = CCLabelBMFont::create("Developed by", "goldFont.fnt");
    developedBy->setScale(.7f);
    m_mainLayer->addChildAtPosition(developedBy, Anchor::Center, ccp(0, 65));

    auto hjfod = CCLabelBMFont::create("HJfod", "bigFont.fnt");
    hjfod->setScale(.65f);
    m_mainLayer->addChildAtPosition(hjfod, Anchor::Center, ccp(0, 40));

    auto credits = CCLabelBMFont::create("Credits", "goldFont.fnt");
    credits->setScale(.6f);
    m_mainLayer->addChildAtPosition(credits, Anchor::Center, ccp(0, 0));

    auto devs = CCNode::create();
    devs->setContentSize({ 300.f, 90.f });
    devs->setAnchorPoint({ .5f, .5f });
    for (auto dev : { "ninXout", "TheSillyDoggo", "Fleym" }) {
        auto label = CCLabelBMFont::create(dev, "bigFont.fnt");
        label->setLayoutOptions(AxisLayoutOptions::create()->setMaxScale(.45f));
        devs->addChild(label);
    }
    devs->setLayout(
        RowLayout::create()
            ->setCrossAxisOverflow(true)
            ->setGap(50.f)
    );
    m_mainLayer->addChildAtPosition(devs, Anchor::Center, ccp(0, -25));

    auto supportSpr = ButtonSprite::create("Support BE!", "goldFont.fnt", "GJ_button_05.png");
    supportSpr->setScale(.8f);
    auto supportBtn = CCMenuItemSpriteExtra::create(
        supportSpr, this, menu_selector(AboutBEPopup::onSupport)
    );
    m_buttonMenu->addChildAtPosition(supportBtn, Anchor::Center, ccp(0, -85));

    auto spr = CCSprite::createWithSpriteFrameName("GJ_optionsBtn02_001.png");
    auto btn = CCMenuItemSpriteExtra::create(
        spr, this, menu_selector(AboutBEPopup::onSettings)
    );
    btn->setID("settings"_spr);
    m_buttonMenu->addChildAtPosition(btn, Anchor::TopRight, ccp(-3, -3));

    return true;
}

void AboutBEPopup::onSupport(CCObject*) {
    createQuickPopup(
        "Support BetterEdit",
        "BetterEdit is first and foremost <cy>a passion project</c>, "
        "but if you would like to support development, <cg>I have a Ko-fi</c>!\n\n"
        "I'm also planning on making a <cb>Pro version</c> in the future with some "
        "cool extra features :)\n\n"
        "However, please <cr>don't donate just for the purpose of getting Pro</c>! "
        "While I fully do intend to make it, I know from past experience that "
        "something might come in the way, and <cr>I do not want to receive money "
        "for a product until I can guarantee it exists</c>.",
        "Cancel", "Open Ko-fi",
        400.f,
        [](auto layer, bool btn2) {
            if (btn2) {
                web::openLinkInBrowser("https://ko-fi.com/hjfod");
            }
        }
    );

    // todo: on next Geode release, just use this instead
    // openSupportPopup(Mod::get());
}

void AboutBEPopup::onSettings(CCObject*) {
    openSettingsPopup(Mod::get());
}

AboutBEPopup* AboutBEPopup::create() {
    auto ret = new AboutBEPopup();
    if (ret && ret->initAnchored(358.f, 250.f, "GJ_square02.png")) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}
