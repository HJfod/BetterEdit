#include <Geode/modify/CustomizeObjectLayer.hpp>
#include <Geode/binding/ColorChannelSprite.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/binding/LevelSettingsObject.hpp>
#include <Geode/binding/GJEffectManager.hpp>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/binding/ColorAction.hpp>
#include <Geode/binding/ButtonSprite.hpp>
#include <Geode/binding/CCTextInputNode.hpp>
#include <Geode/binding/GJSpecialColorSelect.hpp>
#include <Geode/utils/cocos.hpp>

using namespace geode::prelude;

std::string shortTextForColorIdx(int channel) {
    switch (channel) {
        case 0: return "D";
        case 1000: return "BG";
        case 1001: return "G1";
        case 1002: return "L";
        case 1003: return "3DL";
        case 1004: return "Obj";
        case 1005: return "P1";
        case 1006: return "P2";
        case 1007: return "LBG";
        case 1008: return "C";  // what
        case 1009: return "G2";
        case 1010: return "";   // black
        case 1011: return "";   // white
        case 1012: return "Ltr";// lighter
        default: return std::to_string(channel);
    }
}

std::string longTextForColorIdx(int channel) {
    if (0 < channel && channel < 1000) {
        return std::to_string(channel);
    }
    else {
        return GJSpecialColorSelect::textForColorIdx(channel);
    }
}

static std::array<int, 8> RECENT_COLOR_IDS {};
static constexpr std::array SPECIAL_CHANNEL_ORDER {
    0,    1005, 1006, 
    1000, 1003, 1007,
    1001, 1004, 1010, 
    1009, 1002, 1011,
};
static constexpr int CHANNELS_ON_PAGE = 20;

class $modify(NewColorSelect, CustomizeObjectLayer) {
    int page = 0;
    bool modified = false;

    void updateSprite(ColorChannelSprite* sprite) {
        auto channel = static_cast<int>(reinterpret_cast<intptr_t>(sprite->getUserData()));
        auto action = LevelEditorLayer::get()->m_levelSettings->m_effectManager->getColorAction(channel);

        sprite->updateValues(action);
        
        switch (channel) {
            case 1010: {
                sprite->setColor({ 0, 0, 0 });
            } break;

            case 0: {
                sprite->setOpacity(50);
            } break;

            case 1007: {
                sprite->setOpacity(120);
            } break;

            case 1008: {
                sprite->setOpacity(200);
            } break;
        }

        if (auto label = sprite->getChildByID("id-label")) {
            label->setVisible(!action->m_copyID);
        }
    }

    ColorChannelSprite* createSprite(int channel, bool recent) {
        auto spr = ColorChannelSprite::create();
        spr->setScale(.8f);
        // tag gets overwritten by CCMenuItemSpriteExtra :(
        spr->setUserData(reinterpret_cast<void*>(channel));
        spr->setID("channel-sprite"_spr);

        if (recent && channel == 0) {
            spr->setOpacity(105);
            spr->setColor({ 20, 20, 20 });
        }
        else {
            auto selection = CCSprite::createWithSpriteFrameName("GJ_select_001.png");
            selection->setScale(1.1f);
            selection->setID("selected-indicator");
            selection->setPosition(spr->getContentSize() / 2);
            selection->setVisible(false);
            spr->addChild(selection);

            auto label = CCLabelBMFont::create(
                shortTextForColorIdx(channel).c_str(),
                "bigFont.fnt"
            );
            label->limitLabelWidth(25.f, .4f, .2f);
            label->setPosition(spr->getContentSize() / 2);
            label->setID("id-label");
            spr->addChild(label);

            this->updateSprite(spr);
        }

        return spr;
    }

    CCMenuItemSpriteExtra* createChannelButton(int channel, bool recent = false) {
        auto btn = CCMenuItemSpriteExtra::create(
            this->createSprite(channel, recent),
            this,
            menu_selector(CustomizeObjectLayer::onSelectColor)
        );
        btn->setTag(channel);
        btn->setID(fmt::format("channel-{}-button", channel));
        if (recent && channel == 0) {
            btn->setEnabled(false);
        }
        return btn;
    }

    void updateSpritesInMenu(CCNode* menu) {
        if (menu) {
            for (auto child : CCArrayExt<CCMenuItemSpriteExtra>(menu->getChildren())) {
                if (auto spr = static_cast<ColorChannelSprite*>(
                    child->getChildByID("channel-sprite"_spr)
                )) {
                    this->updateSprite(spr);
                }
            }
        }
    }

    void updateSprites() {
        this->updateSpritesInMenu(m_mainLayer->getChildByID("channels-menu"));
        this->updateSpritesInMenu(m_mainLayer->getChildByID("special-channels-menu"));
        this->updateSpritesInMenu(m_mainLayer->getChildByID("recent-channels-menu"));
    }

    void onSelectColor(CCObject* sender) {
        if (!Mod::get()->template getSettingValue<bool>("new-color-menu")) {
            return CustomizeObjectLayer::onSelectColor(sender);
        }
        CustomizeObjectLayer::onSelectColor(sender);
        // calculate actual color if default, otherwise use sender tag
        auto channel = sender->getTag() ? sender->getTag() : this->getActiveMode(true);
        this->gotoPageWithChannel(channel);
        m_customColorChannel = channel;
        this->updateCustomColorLabels();
        m_fields->modified = true;
    }

    void highlightSelectedInMenu(CCNode* menu, int selected) {
        if (menu) {
            for (auto child : CCArrayExt<CCMenuItemSpriteExtra>(menu->getChildren())) {
                if (auto spr = child->getChildByID("channel-sprite"_spr)) {
                    if (auto i = spr->getChildByID("selected-indicator")) {
                        i->setVisible(child->getTag() == selected);
                    }
                }
            }
        }
    }

    void highlightSelected(ButtonSprite* sprite) {
        if (!Mod::get()->template getSettingValue<bool>("new-color-menu")) {
            return CustomizeObjectLayer::highlightSelected(sprite);
        }
        auto selected = this->getActiveMode(true);
        this->highlightSelectedInMenu(m_mainLayer->getChildByID("channels-menu"), selected);
        this->highlightSelectedInMenu(m_mainLayer->getChildByID("special-channels-menu"), selected);
        this->highlightSelectedInMenu(m_mainLayer->getChildByID("recent-channels-menu"), selected);
    }

    void colorSelectClosed(CCNode* target) {
        if (!Mod::get()->template getSettingValue<bool>("new-color-menu")) {
            return CustomizeObjectLayer::colorSelectClosed(target);
        }
        CustomizeObjectLayer::colorSelectClosed(target);
        this->updateSprites();
    }

    void updateCustomColorLabels() {
        if (!Mod::get()->template getSettingValue<bool>("new-color-menu")) {
            return CustomizeObjectLayer::updateCustomColorLabels();
        }
        // prevent textChanged from firing
        auto delegate = m_customColorInput->m_delegate;
        m_customColorInput->setDelegate(nullptr);
        m_customColorInput->setString(longTextForColorIdx(m_customColorChannel));
        m_customColorInput->setDelegate(delegate);
        m_customColorInput->getTextField()->detachWithIME();
    }

    void onUpdateCustomColor(CCObject* sender) {
        if (!Mod::get()->template getSettingValue<bool>("new-color-menu")) {
            return CustomizeObjectLayer::onUpdateCustomColor(sender);
        }
        // recreate to avoid the default clamp of 10..999 without needing to patch
        m_customColorSelected = true;
        int i = m_customColorChannel;
        // cycling to special colors is a blatantly useless feature that i added for some reason
        if (sender->getTag() == 1) {
            // don't allow crossing 999 by pressing up
            if (i < 999) {
                i += 1;
            }
            else if (i >= 1000) {
                // loop around to normal channels
                if (i == SPECIAL_CHANNEL_ORDER.back()) {
                    i = 1;
                }
                else {
                    // find the current channel in special channels
                    auto it = std::find(
                        SPECIAL_CHANNEL_ORDER.begin(),
                        SPECIAL_CHANNEL_ORDER.end(),
                        i
                    );
                    // if it's invalid, just hop to start
                    if (it == SPECIAL_CHANNEL_ORDER.end()) {
                        i = 1;
                    }
                    // otherwise get the next in sequence
                    // this is safe because we have already checked if i is the last element
                    else {
                        i = *std::next(it);
                    }
                }
            }
        }
        else {
            // don't allow crossing 999 by pressing down
            if (i >= 1000) {
                // special case for default
                if (i == 0) {
                    i = 1;
                }
                // first in order is default so we want second
                else if (i != SPECIAL_CHANNEL_ORDER.at(1)) {
                    // find the current channel in special channels
                    auto it = std::find(
                        SPECIAL_CHANNEL_ORDER.begin(),
                        SPECIAL_CHANNEL_ORDER.end(),
                        i
                    );
                    // if it's invalid, just hop to start
                    if (it == SPECIAL_CHANNEL_ORDER.end()) {
                        i = 1;
                    }
                    // otherwise get the previous in sequence
                    else {
                        i = *std::prev(it);
                    }
                }
            }
            else {
                if (i > 1) {
                    i -= 1;
                }
                // loop around to special colors
                else {
                    i = SPECIAL_CHANNEL_ORDER.back();
                }
            }
        }
        m_customColorChannel = i;
        this->updateCustomColorLabels();
        this->updateSelected(i);
        m_customColorSelected = false;
        this->updateColorSprite();
        this->updateChannelLabel(i);
        this->highlightSelected(nullptr);
        this->gotoPageWithChannel(m_customColorChannel);
        m_fields->modified = true;
    }

    void textChanged(CCTextInputNode* input) {
        if (!Mod::get()->template getSettingValue<bool>("new-color-menu")) {
            return CustomizeObjectLayer::textChanged(input);
        }
        CustomizeObjectLayer::textChanged(input);
        this->gotoPageWithChannel(m_customColorChannel);
        m_fields->modified = true;
    }

    void onClose(CCObject* sender) {
        if (!Mod::get()->template getSettingValue<bool>("new-color-menu")) {
            return CustomizeObjectLayer::onClose(sender);
        }
        // add selected color to recent list if it's not there and it's not 0
        if (
            m_fields->modified && m_customColorChannel != 0 &&
            !ranges::contains(RECENT_COLOR_IDS, m_customColorChannel)
        ) {
            std::rotate(
                RECENT_COLOR_IDS.begin(),
                std::prev(RECENT_COLOR_IDS.end()),
                RECENT_COLOR_IDS.end()
            );
            *RECENT_COLOR_IDS.begin() = m_customColorChannel;
        }
        CustomizeObjectLayer::onClose(sender);
    }

    void gotoPageWithChannel(int channel) {
        if (0 < channel && channel < 1000) {
            this->gotoPage((channel - 1) / CHANNELS_ON_PAGE);
        }
    }

    void gotoPage(int page) {
        if (page < 0) {
            page = 0;
        }
        if (page > 999 / CHANNELS_ON_PAGE) {
            page = 999 / CHANNELS_ON_PAGE;
        }
        m_fields->page = page;
        auto channelsMenu = m_mainLayer->getChildByID("channels-menu");
        if (!channelsMenu) {
            // some GD code may cause this to be called before IDs have been added
            return;
        }
        channelsMenu->removeAllChildren();

        for (int channel = 1; channel <= CHANNELS_ON_PAGE; channel++) {
            auto c = channel + page * CHANNELS_ON_PAGE;
            if (c < 1000) {
                channelsMenu->addChild(this->createChannelButton(c));
            }
        }
        channelsMenu->updateLayout();

        if (auto menu = m_mainLayer->getChildByID("page-menu"_spr)) {
            static_cast<CCLabelBMFont*>(menu->getChildByID("current-page"))
                ->setString(fmt::format("Page {} / {}", page + 1, 1000 / CHANNELS_ON_PAGE).c_str());
        }

        this->highlightSelected(nullptr);
    }

    void onPage(CCObject* sender) {
        this->gotoPage(m_fields->page + sender->getTag());
    }

    bool init(GameObject* obj, CCArray* objs) {
        if (!CustomizeObjectLayer::init(obj, objs))
            return false;
        
        if (!Mod::get()->template getSettingValue<bool>("new-color-menu")) {
            return true;
        }

        auto winSize = CCDirector::get()->getWinSize();

        // move the browse and copy paste menus to be inline with the popup because 
        // i will actually get diarrhea if they stay the way they are in vanilla
        if (auto menu = m_mainLayer->getChildByID("browse-menu")) {
            menu->setPositionY(winSize.height / 2 - 50.f);
        }
        if (auto menu = m_mainLayer->getChildByID("copy-paste-menu")) {
            menu->setPositionY(winSize.height / 2 + 50.f);
        }

        auto channelsMenu = m_mainLayer->getChildByID("channels-menu");
        auto specialsMenu = m_mainLayer->getChildByID("special-channels-menu");

        // remove existing color button UI; we will recreate our own from scratch
        m_colorButtons->removeAllObjects();
        channelsMenu->removeAllChildren();
        specialsMenu->removeAllChildren();

        // move and resize selected channel menu down a bit to fit the color buttons
        if (auto menu = m_mainLayer->getChildByID("selected-channel-menu")) {
            menu->setPosition(winSize.width / 2 + 125.f, winSize.height / 2 - 95.f);
            menu->setScale(.8f);
        }

        auto selectPos = CCPoint(winSize.width / 2 + 210.f, winSize.height / 2 - 15.f);
        m_mainLayer->getChildByID("select-channel-menu")->setPosition(selectPos);
        m_mainLayer->getChildByID("channel-input-bg")->setPosition(selectPos);
        m_mainLayer->getChildByID("channel-input")->setPosition(selectPos);
        m_customColorInput->m_maxLabelWidth = 38.f;
        
        // // remove the bg from custom color select
        // m_colorTabNodes->removeObject(m_customColorInputBG);
        // m_customColorInputBG->removeFromParent();

        // keeping both m_customColorInputBG and m_customColorButtonSprite 
        // in memory so there are no faultful accesses due to GD code

        // add bg. it's the special HJfod sauce that makes UI look good
        auto bg = CCScale9Sprite::create("square02_001.png", { 0, 0, 80, 80 });
        bg->setOpacity(80);
        bg->setContentSize({ 340, 150 });
        bg->setPosition(winSize / 2);
        bg->setID("bg"_spr);
        m_mainLayer->addChild(bg);
        m_colorTabNodes->addObject(bg); // this makes the bg be hidden in the text tab

        // move and resize the channel menus to be side-by-side horizontally
        // also reset layout to enforce the new one incase someone has altered it >:3

        // add special color channels
        specialsMenu->setContentSize({ 90.f, 125.f });
        specialsMenu->setPosition(winSize.width / 2 - 120.f, winSize.height / 2 - 7.5f);
        specialsMenu->setLayout(
            RowLayout::create()
                ->setCrossAxisOverflow(false)
                ->setGrowCrossAxis(true)
                ->setAxisAlignment(AxisAlignment::Center)
                ->setCrossAxisAlignment(AxisAlignment::Center)
                ->setGap(4.f)
        );

        for (auto channel : SPECIAL_CHANNEL_ORDER) {
            specialsMenu->addChild(this->createChannelButton(channel));
        }
        specialsMenu->updateLayout();

        auto specialTitle = CCLabelBMFont::create("Special", "bigFont.fnt");
        specialTitle->setID("special-channels-title"_spr);
        specialTitle->setScale(.35f);
        specialTitle->setPosition(
            specialsMenu->getPositionX(),
            specialsMenu->getPositionY() + 72.f
        );
        m_mainLayer->addChild(specialTitle);
        m_colorTabNodes->addObject(specialTitle);

        // add separator line between items. it's the special HJfod UI sauce cherry
        auto line = CCSprite::createWithSpriteFrameName("edit_vLine_001.png");
        line->setPosition({ winSize.width / 2 - 67.5f, winSize.height / 2 - 7.5f });
        line->setScaleY(1.65f);
        line->setOpacity(100);
        line->setID("separator"_spr);
        m_mainLayer->addChild(line);
        m_colorTabNodes->addObject(line);

        // add new color buttons to channel menu
        channelsMenu->setContentSize({ 160.f, 125.f });
        channelsMenu->setPosition(winSize.width / 2 + 15.f, winSize.height / 2 - 7.5f);
        channelsMenu->setLayout(
            RowLayout::create()
                ->setCrossAxisOverflow(false)
                ->setGrowCrossAxis(true)
                ->setAxisAlignment(AxisAlignment::Center)
                ->setCrossAxisAlignment(AxisAlignment::Center)
                ->setGap(4.f)
        );
        channelsMenu->updateLayout();

        auto pageMenu = CCMenu::create();
        pageMenu->setID("page-menu"_spr);
        pageMenu->ignoreAnchorPointForPosition(false);
        pageMenu->setPosition(
            channelsMenu->getPositionX(),
            channelsMenu->getPositionY() + 72.f
        );
        pageMenu->setContentSize({ 150.f, 20.f });

        auto pageTitle = CCLabelBMFont::create("Page 1/X", "bigFont.fnt");
        pageTitle->setID("current-page");
        pageTitle->setScale(.35f);
        pageTitle->setPosition(70.f, 10.f);
        pageMenu->addChild(pageTitle);

        auto lastPageSpr = CCSprite::createWithSpriteFrameName("edit_leftBtn_001.png");
        lastPageSpr->setScale(.5f);
        auto lastPageBtn = CCMenuItemSpriteExtra::create(
            lastPageSpr, this, menu_selector(NewColorSelect::onPage)
        );
        lastPageBtn->setTag(-1);
        lastPageBtn->setPosition(10.f, 10.f);
        pageMenu->addChild(lastPageBtn);

        auto nextPageSpr = CCSprite::createWithSpriteFrameName("edit_rightBtn_001.png");
        nextPageSpr->setScale(.5f);
        auto nextPageBtn = CCMenuItemSpriteExtra::create(
            nextPageSpr, this, menu_selector(NewColorSelect::onPage)
        );
        nextPageBtn->setTag(1);
        nextPageBtn->setPosition(140.f, 10.f);
        pageMenu->addChild(nextPageBtn);

        m_mainLayer->addChild(pageMenu);
        m_colorTabNodes->addObject(pageMenu);

        // add second separator line between channels and recent
        auto line2 = CCSprite::createWithSpriteFrameName("edit_vLine_001.png");
        line2->setPosition({ winSize.width / 2 + 97.5f, winSize.height / 2 - 7.5f });
        line2->setScaleY(1.65f);
        line2->setOpacity(100);
        line2->setID("separator-2"_spr);
        m_mainLayer->addChild(line2);
        m_colorTabNodes->addObject(line2);

        // add recently used channels menu
        auto recentMenu = CCMenu::create();
        recentMenu->ignoreAnchorPointForPosition(false);
        recentMenu->setID("recent-channels-menu"_spr);
        recentMenu->setContentSize({ 60.f, 125.f });
        recentMenu->setPosition(winSize.width / 2 + 135.f, winSize.height / 2 - 7.5f);
        recentMenu->setLayout(
            RowLayout::create()
                ->setCrossAxisOverflow(false)
                ->setGrowCrossAxis(true)
                ->setAxisAlignment(AxisAlignment::Center)
                ->setCrossAxisAlignment(AxisAlignment::Center)
                ->setGap(4.f)
        );
        m_mainLayer->addChild(recentMenu);
        m_colorTabNodes->addObject(recentMenu);

        for (auto channel : RECENT_COLOR_IDS) {
            recentMenu->addChild(this->createChannelButton(channel, true));
        }
        recentMenu->updateLayout();

        auto recentTitle = CCLabelBMFont::create("Recent", "bigFont.fnt");
        recentTitle->setID("recent-channels-title"_spr);
        recentTitle->setScale(.35f);
        recentTitle->setPosition(
            recentMenu->getPositionX(),
            recentMenu->getPositionY() + 72.f
        );
        m_mainLayer->addChild(recentTitle);
        m_colorTabNodes->addObject(recentTitle);

        this->gotoPage(0);
        this->updateCustomColorLabels();

        return true;
    }
};