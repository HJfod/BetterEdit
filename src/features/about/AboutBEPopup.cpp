#include "AboutBEPopup.hpp"
#include <Geode/ui/GeodeUI.hpp>

struct Dev {
    const char* name;
    std::initializer_list<std::pair<const char*, const char*>> links;

    CCNode* create(AboutBEPopup* popup, bool big) const {
        auto node = CCNode::create();
        node->setContentSize({ 100.f, 50.f });
        node->setAnchorPoint({ .5f, .5f });

        auto label = CCLabelBMFont::create(name, "bigFont.fnt");
        label->limitLabelWidth(node->getContentSize().width, .5f, .1f);
        node->addChildAtPosition(label, Anchor::Top, ccp(0, big ? -15 : -5));

        auto menu = CCMenu::create();
        menu->setContentSize(node->getContentSize() * ccp(1, 0.5));
        menu->ignoreAnchorPointForPosition(false);
        menu->setAnchorPoint({ .5f, 0 });

        for (auto link : links) {
            auto spr = CCSprite::createWithSpriteFrameName(link.first);
            auto btn = CCMenuItemSpriteExtra::create(
                spr, popup, menu_selector(AboutBEPopup::onDevLink)
            );
            btn->setUserObject(CCString::create(link.second));
            menu->addChild(btn);
        }
        menu->setLayout(RowLayout::create());

        node->addChildAtPosition(menu, Anchor::Bottom);

        return node;
    }
};

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
    m_mainLayer->addChildAtPosition(developedBy, Anchor::Center, ccp(0, 85));

    auto hjfod = Dev {
        .name = "HJfod",
        .links = {
            { "gj_ytIcon_001.png", "https://youtube.com/hjfod" },
            { "gj_twIcon_001.png", "https://twitter.com/hjfod" },
            { "geode.loader/github.png", "https://github.com/hjfod" },
            { "geode.loader/gift.png", "https://ko-fi.com/hjfod" },
        }
    }.create(this, true);
    m_mainLayer->addChildAtPosition(hjfod, Anchor::Center, ccp(0, 55));

    auto credits = CCLabelBMFont::create("Credits", "goldFont.fnt");
    credits->setScale(.6f);
    m_mainLayer->addChildAtPosition(credits, Anchor::Center, ccp(0, 10));

    auto devs = CCNode::create();
    devs->setContentSize({ 300.f, 90.f });
    devs->setAnchorPoint({ .5f, .5f });
    for (auto dev : {
        Dev {
            .name = "ninXout",
            .links = {
                { "gj_ytIcon_001.png", "https://youtube.com/@ninXout" },
                { "gj_twIcon_001.png", "https://twitter.com/ninXout" },
                { "geode.loader/github.png", "https://github.com/ninXout" },
            }
        },
        Dev {
            .name = "TheSillyDoggo",
            .links = {
                { "gj_twIcon_001.png", "https://twitter.com/TheSillyDoggo" },
                { "geode.loader/github.png", "https://github.com/TheSillyDoggo" },
            }
        },
        Dev {
            .name = "Fleym",
            .links = {
                { "geode.loader/github.png", "https://github.com/Fleeym" }
            }
        },
        Dev {
            .name = "Mat",
            .links = {
                { "gj_twIcon_001.png", "https://twitter.com/mat_4_4/" },
                { "geode.loader/github.png", "https://github.com/matcool" },
            }
        },
    }) {
        auto label = dev.create(this, false);
        label->setLayoutOptions(AxisLayoutOptions::create()->setMaxScale(.65f));
        devs->addChild(label);
    }
    devs->setLayout(
        RowLayout::create()
            ->setCrossAxisOverflow(true)
            ->setGap(20.f)
    );
    m_mainLayer->addChildAtPosition(devs, Anchor::Center, ccp(0, -20));

    auto menu = CCMenu::create();
    menu->setContentSize({ 300.f, 100.f });
    menu->ignoreAnchorPointForPosition(false);

    auto reportBugSpr = ButtonSprite::create("Report a Bug", "goldFont.fnt", "GJ_button_05.png");
    reportBugSpr->setScale(.55f);
    auto reportBugBtn = CCMenuItemSpriteExtra::create(
        reportBugSpr, this, menu_selector(AboutBEPopup::onReportBug)
    );
    menu->addChild(reportBugBtn);

    auto suggestSpr = ButtonSprite::create("Suggest a Feature", "goldFont.fnt", "GJ_button_05.png");
    suggestSpr->setScale(.55f);
    auto suggestBtn = CCMenuItemSpriteExtra::create(
        suggestSpr, this, menu_selector(AboutBEPopup::onSuggestFeature)
    );
    menu->addChild(suggestBtn);

    auto supportSpr = ButtonSprite::create("Support BE", "goldFont.fnt", "GJ_button_05.png");
    supportSpr->setScale(.55f);
    auto supportBtn = CCMenuItemSpriteExtra::create(
        supportSpr, this, menu_selector(AboutBEPopup::onSupport)
    );
    menu->addChild(supportBtn);

    menu->setLayout(RowLayout::create()->setGrowCrossAxis(true));
    m_mainLayer->addChildAtPosition(menu, Anchor::Center, ccp(0, -70));

    auto linksMenu = CCMenu::create();
    linksMenu->setAnchorPoint({ .5f, .0f });
    linksMenu->setContentSize({ 80, 20 });
    linksMenu->setScale(.8f);

    auto ghBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("geode.loader/github.png"),
        this, menu_selector(AboutBEPopup::onDevLink)
    );
    ghBtn->setUserObject(CCString::create("https://github.com/HJfod/BetterEdit"));
    linksMenu->addChild(ghBtn);

    auto dcBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("gj_discordIcon_001.png"),
        this, menu_selector(AboutBEPopup::onDevLink)
    );
    dcBtn->setUserObject(CCString::create("https://discord.gg/8ADhXvxjZG"));
    linksMenu->addChild(dcBtn);

    linksMenu->setLayout(RowLayout::create());
    m_mainLayer->addChildAtPosition(linksMenu, Anchor::Bottom, ccp(0, 7));

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

void AboutBEPopup::onSuggestFeature(CCObject*) {
    createQuickPopup(
        "Report a Bug",
        "If you have an idea for a new BetterEdit feature, please open an "
        "Issue on the <cy>Github</c> with the <cp>Suggestion</c> label!\n\n"
        "Note that you need a <cb>Github Account</c> to post Issues.",
        "Cancel", "Open Github",
        340.f,
        [](auto layer, bool btn2) {
            if (btn2) {
                web::openLinkInBrowser("https://github.com/HJfod/BetterEdit/issues/new/choose");
            }
        }
    );
}

void AboutBEPopup::onReportBug(CCObject*) {
    createQuickPopup(
        "Report a Bug",
        "If you have encountered a bug with BetterEdit, please open an "
        "Issue on the <cy>Github</c>!\n\n"
        "Note that you need a <cb>Github Account</c> to post Issues.",
        "Cancel", "Open Github",
        340.f,
        [](auto layer, bool btn2) {
            if (btn2) {
                web::openLinkInBrowser("https://github.com/HJfod/BetterEdit/issues/new/choose");
            }
        }
    );
}

void AboutBEPopup::onSettings(CCObject*) {
    openSettingsPopup(Mod::get());
}

void AboutBEPopup::onDevLink(CCObject* sender) {
    auto link = static_cast<CCString*>(static_cast<CCNode*>(sender)->getUserObject())->getCString();
    web::openLinkInBrowser(link);
}

AboutBEPopup* AboutBEPopup::create() {
    auto ret = new AboutBEPopup();
    if (ret && ret->initAnchored(358.f, 270.f, "GJ_square02.png")) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}
