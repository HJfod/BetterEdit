#include "AboutBEPopup.hpp"
#include "ChangelogPopup.hpp"
#include "SupportPopup.hpp"
#include <Geode/ui/GeodeUI.hpp>
#include <Geode/ui/MDTextArea.hpp>
#include <Geode/binding/ButtonSprite.hpp>
#include <Geode/utils/web.hpp>
#include <utils/Pro.hpp>

#ifdef BETTEREDIT_PRO
#include <pro/ui/SupportersPopup.hpp>
#endif

struct Dev {
    const char* name;
    std::initializer_list<std::pair<const char*, const char*>> links;

    CCNode* create(AboutBEPopup* popup, bool big) const {
        auto node = CCNode::create();
        node->setContentSize({ links.size() * 25.f + 25, 50 });
        node->setAnchorPoint({ .5f, .5f });

        auto label = CCLabelBMFont::create(name, "bigFont.fnt");
        label->limitLabelWidth(node->getContentSize().width, .5f, .1f);
        node->addChildAtPosition(label, Anchor::Top, ccp(0, big ? -15 : -5));

        auto menu = CCMenu::create();
        menu->setContentSize(node->getContentSize() * ccp(1, 0.5));
        menu->ignoreAnchorPointForPosition(false);
        menu->setAnchorPoint({ .5f, 0 });
        menu->setScale(big ? .8f : 1.f);

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
    this->addCorners(Corner::Blue, Corner::Gold);

    // main dev

    auto developedBy = CCLabelBMFont::create("Developed by", "goldFont.fnt");
    developedBy->setScale(.7f);
    m_mainLayer->addChildAtPosition(developedBy, Anchor::Center, ccp(0, 90));

    auto hjfod = Dev {
        .name = "HJfod",
        .links = {
            { "gj_ytIcon_001.png", "https://youtube.com/hjfod" },
            { "gj_twIcon_001.png", "https://twitter.com/hjfod" },
            { "geode.loader/github.png", "https://github.com/hjfod" },
            { "geode.loader/gift.png", "https://ko-fi.com/hjfod" },
        }
    }.create(this, true);
    m_mainLayer->addChildAtPosition(hjfod, Anchor::Center, ccp(0, 60));

    // other devs

    auto credits = CCLabelBMFont::create("Contributors", "goldFont.fnt");
    credits->setScale(.6f);
    m_mainLayer->addChildAtPosition(credits, Anchor::Center, ccp(0, 15));

    auto devs = CCNode::create();
    devs->setContentSize({ 325, 90 });
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
                { "gj_ytIcon_001.png", "https://www.youtube.com/@TheSillyDoggo" },
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
            .name = "CattoDev",
            .links = {
                { "gj_ytIcon_001.png", "https://youtube.com/@CattoGD" },
                { "gj_twIcon_001.png", "https://twitter.com/cattodevgd/" },
            }
        },
        Dev {
            .name = "Mat",
            .links = {
                { "geode.loader/github.png", "https://github.com/matcool" },
            }
        },
        Dev {
            .name = "Alphalaneous",
            .links = {
                { "geode.loader/github.png", "https://github.com/Alphalaneous" },
                { "gj_twIcon_001.png", "https://twitter.com/Alphalaneous" },
            }
        },
    }) {
        auto label = dev.create(this, false);
        label->setLayoutOptions(AxisLayoutOptions::create()->setScaleLimits(.1f, .65f));
        devs->addChild(label);
    }
    devs->setLayout(
        RowLayout::create()
            ->setCrossAxisOverflow(true)
            ->setGap(20.f)
    );
    m_mainLayer->addChildAtPosition(devs, Anchor::Center, ccp(0, -15));

    // actions

    auto menu = CCMenu::create();
    menu->setContentSize({ 325, 100 });
    menu->ignoreAnchorPointForPosition(false);

    for (auto pair : std::initializer_list<std::pair<const char*, SEL_MenuHandler>> {
        { "Report a Bug", menu_selector(AboutBEPopup::onReportBug) },
        { "Suggest a Feature", menu_selector(AboutBEPopup::onSuggestFeature) },
        { "Changelog", menu_selector(AboutBEPopup::onChangelog) },
        { "Special Thanks", menu_selector(AboutBEPopup::onSpecialThanks) },
        { "Support BE", menu_selector(AboutBEPopup::onSupport) },
        { "Supporters", menu_selector(AboutBEPopup::onSupporters) },
    }) {
        auto spr = ButtonSprite::create(pair.first, "goldFont.fnt", "GJ_button_05.png", .8f);
        spr->setScale(.55f);
        auto btn = CCMenuItemSpriteExtra::create(spr, this, pair.second);
        menu->addChild(btn);
    }

    menu->setLayout(RowLayout::create()->setGrowCrossAxis(true));
    m_mainLayer->addChildAtPosition(menu, Anchor::Center, ccp(0, -60));

    BE_PRO_FEATURE(
        auto label = CCLabelBMFont::create("Thank You for Supporting BetterEdit <3", "bigFont.fnt");
        label->setColor({ 55, 255, 255 });
        label->setScale(.35f);
        m_mainLayer->addChildAtPosition(label, Anchor::Center, ccp(0, -95));
    );

    // BE links

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

    auto optionsBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_optionsBtn02_001.png"),
        this, menu_selector(AboutBEPopup::onSettings)
    );
    optionsBtn->setID("settings"_spr);
    m_buttonMenu->addChildAtPosition(optionsBtn, Anchor::TopRight, ccp(-3, -3));

    return true;
}

void AboutBEPopup::onSupport(CCObject*) {
    SupportPopup::create(false)->show();
    // openSupportPopup(Mod::get());
}
void AboutBEPopup::onSupporters(CCObject*) {
#ifdef BETTEREDIT_PRO
    pro::SupportersPopup::create()->show();
#endif
}

void AboutBEPopup::onSuggestFeature(CCObject*) {
    createQuickPopup(
        "Suggest a Feature",
        "If you have an idea for a new BetterEdit feature, please open an "
        "Issue on the <cy>Github</c> with the <cp>Suggestion</c> label!\n\n"
        "Note that you need a <cb>Github Account</c> to post Issues.",
        "Cancel", "Open Github",
        340.f,
        [](auto, bool btn2) {
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
        [](auto, bool btn2) {
            if (btn2) {
                web::openLinkInBrowser("https://github.com/HJfod/BetterEdit/issues/new/choose");
            }
        }
    );
}

void AboutBEPopup::onChangelog(CCObject*) {
    ChangelogPopup::create()->show();
}

void AboutBEPopup::onSettings(CCObject*) {
    openSettingsPopup(Mod::get());
}

void AboutBEPopup::onSpecialThanks(CCObject*) {
    SpecialThanksPopup::create()->show();
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

bool SpecialThanksPopup::setup() {
    auto specialThanks = 
        "## Donators <3\n\n"
        " * <cp>Alphalaneous</c>\n"
        " * <cg>KontrollFreek</c>\n"
        " * <cp>Aikoyori</c>\n"
        " * <cg>Waaffel</c>\n"
        " * <cp>Rayden</c>\n"
        " * <cg>TDP9</c>\n"
        " * <cp>Olly</c>\n"
        " * <cg>SeeBeyond</c>\n"
        " * <cp>Brittank88</c>\n"
        "## Programming help <3\n\n"
        " * <cl>Mat</c>\n"
        " * <co>Camila</c>\n"
        " * <cl>Brittank88</c>\n"
        " * <co>Alphalaneous</c>\n"
        "## Other thanks <3\n\n"
        " * <cr>TWICE - Scientist MV</c> & <cr>TWICE - What is Love? MV</c>\n"
        " * <cy>RobTop</c>\n"
        " * [And you!](https://www.youtube.com/watch?v=4TnAKurylA8)\n";

    auto textArea = MDTextArea::create(specialThanks, ccp(250, 170));
    m_mainLayer->addChildAtPosition(textArea, Anchor::Center);

    this->addCorners(Corner::Gold, Corner::Gold);

    return true;
}

SpecialThanksPopup* SpecialThanksPopup::create() {
    auto ret = new SpecialThanksPopup();
    if (ret && ret->initAnchored(280, 210, "GJ_square01.png")) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}
