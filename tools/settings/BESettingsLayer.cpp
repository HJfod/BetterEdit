#include "BESettingsLayer.hpp"

using namespace gdmake;
using namespace gdmake::extra;
using namespace gd;
using namespace cocos2d;

int g_nSettingsPage = 0;

#define BE_SETTING_FUNC(__name__) \
    BetterEdit::get##__name__##AsString(),\
    &BetterEdit::set##__name__##FromString

#define BE_SETTING_FUNC_B(__name__) \
    BetterEdit::get##__name__(),\
    &BetterEdit::set##__name__

#define USER_LINK(name, gdid) \
    addButton(                                                  \
        CCNodeConstructor<CCLabelBMFont*>()                     \
            .fromText(name, "goldFont.fnt")                     \
            .limit(this->m_pLrSize.width / 2 - 64.0f, .75f, .1f)\
            .done(),                                            \
        (SEL_MenuHandler)&BESettingsLayer::onShowAccount,       \
        false                                                   \
    )->setUserData(reinterpret_cast<void*>(gdid));

void BESettingsLayer::setup() {
    this->m_bNoElasticity = true;

    as<CCMenu*>(m_pPauseLayer->getChildren()->objectAtIndex(0))->setEnabled(false);
    as<CCMenu*>(m_pPauseLayer->m_pButton0->getParent())->setEnabled(false);

    this->m_pPrevPageBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"),
        this,
        (SEL_MenuHandler)&BESettingsLayer::onPage
    );
    this->m_pPrevPageBtn->setUserData(as<void*>(-1));
    this->m_pPrevPageBtn->setPosition(- this->m_pLrSize.width / 2 - 40.0f, 0.0f);
    this->m_pButtonMenu->addChild(this->m_pPrevPageBtn, 150);
    this->m_nCurrentPage = g_nSettingsPage;

    this->m_pNextPageBtn = CCMenuItemSpriteExtra::create(
        CCNodeConstructor()
            .fromFrameName("GJ_arrow_01_001.png")
            .flipX()
            .done(),
        this,
        (SEL_MenuHandler)&BESettingsLayer::onPage
    );
    this->m_pNextPageBtn->setUserData(as<void*>(1));
    this->m_pNextPageBtn->setPosition(this->m_pLrSize.width / 2 + 40.0f, 0.0f);
    this->m_pButtonMenu->addChild(this->m_pNextPageBtn, 150);

    this->addToggle(
        "Custom Paste State",
        "When using <cy>Paste State</c>, you can select which <cc>attributes</c> to paste",
        BE_SETTING_FUNC_B(PasteStateEnabled)
    );
    this->addToggle(
        "Custom Grid Size",
        "Enable Custom Grid Size <cy>(Static)</c>",
        BE_SETTING_FUNC_B(GridSizeEnabled)
    );
    this->addToggle(
        "Always Use Custom Grid Size",
        "Without this option, using the <cr>grid zoom buttons</c> will reset grid behaviour to normal "
        "when the zoom level is back to normal",
        BE_SETTING_FUNC_B(AlwaysUseCustomGridSize)
    );
    this->addToggle(
        "Disable Move On Zoom",
        "When using <cr>Control</c> + <cp>Scroll Wheel</c> to zoom, the screen won't move toward the cursor",
        BE_SETTING_FUNC_B(DisableMouseZoomMove)
    );
    this->addToggle(
        "Fade Out Percentage",
        "When enabled, the <cl>percentage</c> text below the progress bar will automatically fade out when "
        "the screen is not being moved",
        BE_SETTING_FUNC_B(FadeOutPercentage)
    );
    // this->addToggle("Pulse Objects", nullptr, BE_SETTING_FUNC_B(PulseObjectsInEditor), true);
    this->addToggle("Rotate Saws", nullptr, BE_SETTING_FUNC_B(RotateSawsInEditor));
    this->addToggle("No Global Clipboard", nullptr, BE_SETTING_FUNC_B(DisableGlobalClipboard));
    // this->addToggle("Disable Position Text", nullptr, BE_SETTING_FUNC_B(DisableEditorPos));
    this->addToggle(
        "Enable Ctrl + A",
        "Pressing Ctrl + A selects all objects",
        BE_SETTING_FUNC_B(EnableControlA)
    );
    this->addToggle("Disable Zoom Text", nullptr, BE_SETTING_FUNC_B(DisableZoomText));
    this->addToggle("Disable Percentage", nullptr, BE_SETTING_FUNC_B(DisablePercentage));
    this->addToggle("Disable Extra Object Info", nullptr, BE_SETTING_FUNC_B(DisableExtraObjectInfo));
    this->addToggle(
        "Disable Favorites Tab",
        "Requires a restart to apply",
        BE_SETTING_FUNC_B(DisableFavoritesTab)
    );
    this->addToggle(
        "Bypass Object Limit",
        "Disables the 80k Object Limit and 100 Custom Object Limit",
        BE_SETTING_FUNC_B(BypassObjectLimit)
    );
    this->addToggle(
        "Use Old Progress Bar",
        "Reset the <co>Position Slider's</c> behaviour back to GD's original one\n\n "
        "(<cl>BetterEdit</c> makes the Slider relative to the level's size)",
        BE_SETTING_FUNC_B(UseOldProgressBar)
    );
    this->addToggle(
        "Repeat Copy+Paste",
        "<cy>Paste Repeating</c> means that when using <co>Copy + Paste</c>, "
        "actions such as moving & incrementing trigger targets are automatically "
        "repeated on the next object",
        BE_SETTING_FUNC_B(RepeatCopyPaste), true
    );
    this->addToggle(
        "Enable Relative Swipe",
        "Makes it so that when you scroll the screen while using <co>swipe</c>, "
        "the start point of the swipe rectangle stays relative to the object layer",
        BE_SETTING_FUNC_B(EnableRelativeSwipe)
    );
    this->addToggle(
        "Use Old Color Select",
        "Use GD's original color select popup when using <co>Edit Object</c>",
        BE_SETTING_FUNC_B(DisableNewColorSelection)
    );
    this->addToggle(
        "Enable Up Arrow",
        "Enables the <cr>Up</c> arrow to be used when playtesting",
        BE_SETTING_FUNC_B(UseUpArrowForGameplay)
    );
    this->addToggle(
        "Disable View Tab",
        "Disables the <co>View</c> tab (<cl>requires re-entering the Editor</c>)",
        BE_SETTING_FUNC_B(DisableVisibilityTab)
    );
    this->addToggle(
        "Don't Color Borders",
        "BetterEdit makes portals that are <cy>checked</c> have the line color match "
        "the portal color",
        BE_SETTING_FUNC_B(DontColorGamemodeBorders)
    );

    // this->incrementPageCount(true);

    this->addToggle(
        "Enable Auto-Saving",
        "The level is automatically saved every x minutes\n\n "
        "<cr>Highly experimental!</c>",
        BE_SETTING_FUNC_B(EnableAutoSave),
        true
    );
    this->addToggle(
        "Soft Save",
        "<cy>Saves objects to a file immediately when they're placed</c>\n "
        "<cg>Use the frequency to modify how many blocks have to be placed until data is saved</c>\n "
        "<cp>Can cause lag, but if the game crashes, you can recover the level</c>\n "
        "<cl>Not the same as saving a level; to commit a level to your local data, use auto-saving or "
        "manually save!</c>\n "
        "<cr>Extremely experimental!</c>",
        BE_SETTING_FUNC_B(EnableSoftAutoSave),
        true
    );
    // this->addToggle(
    //     "Fast Save",
    //     "Enables <cl>asynchronous</c> saving\n\n <cr>Ridiculously experimental!</c>",
    //     BE_SETTING_FUNC_B(EnableAsyncSave),
    //     true
    // );
    this->addInput("Save Every (min):", BE_SETTING_FUNC(AutoSaveTime), "0123456789");
    this->addInput("Soft Save Freq.:", BE_SETTING_FUNC(SoftSaveFrequency), "0123456789");
    this->incrementPageCount(true);

    this->addInput("Grid Size:", BE_SETTING_FUNC(GridSize), "0123456789.");
    this->addInput("Scale Snap:", BE_SETTING_FUNC(ScaleSnap), "0123456789.");
    this->addInput("Percentage Accuracy:", BE_SETTING_FUNC(PercentageAccuracy), "0123456789");
    this->addSlider(
        "Music",
        (SEL_MenuHandler)&PauseLayer::musicSliderChanged,
        FMODAudioEngine::sharedEngine()->getMusicVolume()
    );
    this->addSlider(
        "SFX",
        (SEL_MenuHandler)&PauseLayer::sfxSliderChanged,
        FMODAudioEngine::sharedEngine()->getSFXVolume()
    );

    // this->addButton(
    //     ButtonSprite::create(
    //         "Default Object Props", 0, 0, "bigFont.fnt", "GJ_button_01.png", 0, .8f
    //     ), nullptr, true
    // );

    // this->incrementPageCount(true);
    this->addButton(
        CCNodeConstructor<CCLabelBMFont*>()
            .fromText("Developed by HJfod", "bigFont.fnt")
            .color({ 15, 255, 45 })
            .scale(.6f)
            .done(),
        (SEL_MenuHandler)&BESettingsLayer::onShowAccount,
        true
    )->setUserData(as<void*>(104257));
    this->addTitle("Special Thanks:", "bigFont.fnt")->setScale(.6f);
    this->USER_LINK("Mat", 5568872);
    this->USER_LINK("Figment", 107269);
    this->USER_LINK("cos8oih", 4504713);
    this->USER_LINK("Brittank88", 93792);
    this->USER_LINK("Alphalaneous", 0x116147);
    this->USER_LINK("Zidnes", 0x42f5cd);
    this->addTitle("Special Thanks (Part 2):", "bigFont.fnt")->setScale(.6f);
    this->USER_LINK("Absolute", 0x5243ef);
    this->USER_LINK("Adaf", 0x55b039);
    this->USER_LINK("RobTop", 71);
    this->USER_LINK("PoweredByPie", 0x2beddd);
    this->USER_LINK("GDColon", 106255);
    this->USER_LINK("Fed", 0xced343);
    this->addButton(
        CCNodeConstructor<CCLabelBMFont*>()
            .fromText("And You :)", "goldFont.fnt")
            .limit(this->m_pLrSize.width / 2 - 64.0f, .75f, .1f)
            .done(),
        (SEL_MenuHandler)&BESettingsLayer::onURL,
        false
    )->setUserObject(CCString::create("https://www.youtube.com/watch?v=4TnAKurylA8"));

    this->incrementPageCount(true);

    this->addButton(
        CCNodeConstructor<ButtonSprite*>()
            .fromButtonSprite("Support Server", "GJ_button_05.png", "goldFont.fnt")
            .scale(.75f)
            .done(),
        (SEL_MenuHandler)&BESettingsLayer::onURL,
        true
    )->setUserObject(CCString::create("https://discord.gg/K9Kuh3hzTC"));
    this->addTitle("Running version GDMAKE_PROJECT_VERSION", "goldFont.fnt")->setScale(.6f);
    // this->addToggle(
    //     "Check for Updates", "Checks for available updates at startup",
    //     BE_SETTING_FUNC_B(AutoUpdateEnabled)
    // );
    // this->addToggle(
    //     "Beta Test", "If auto-updates are enabled, automatically updates to the latest version, even if it is a beta",
    //     BE_SETTING_FUNC_B(AutoUpdateEnabled)
    // );
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

    item->setVisible(this->m_nDestPage == g_nSettingsPage);
}

void BESettingsLayer::addInput(
    const char* text,
    std::string const& value,
    BE_Callback cb,
    std::string const& filter
) {
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    
    auto y = getItemPos(true).y;

    auto scaleSnapInput = InputNode::create(50.0f, "0");
    auto scaleSnapLabel = CCLabelBMFont::create(text, "goldFont.fnt");

    scaleSnapInput->setPosition(winSize.width / 2 + 60.0f, winSize.height / 2 + y);
    scaleSnapInput->getInputNode()->setAllowedChars(filter);

    scaleSnapLabel->limitLabelWidth(this->m_pLrSize.width / 2 - 40.0f, .7f, .2f);
    scaleSnapLabel->setPosition(
        winSize.width / 2 - scaleSnapLabel->getScaledContentSize().width / 2,
        winSize.height / 2 + y
    );

    scaleSnapInput->setString(value.c_str());
    scaleSnapInput->getInputNode()->setUserData(as<void*>(cb));
    scaleSnapInput->getInputNode()->setDelegate(this);

    this->m_pLayer->addChild(scaleSnapInput);
    this->m_pLayer->addChild(scaleSnapLabel);

    this->addItem(scaleSnapInput);
    this->addItem(scaleSnapLabel);
    this->incrementPageCount();
}

void BESettingsLayer::addSlider(const char* text, cocos2d::SEL_MenuHandler onChange, float val) {
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    
    auto y = getItemPos(true).y;

    auto slider = Slider::create(this, onChange, .8f);
    auto label = CCLabelBMFont::create(text, "goldFont.fnt");
    label->limitLabelWidth(this->m_pLrSize.width / 3, .7f, .2f);

    slider->setPosition(winSize.width / 2 + 40.0f, winSize.height / 2 + y);
    label->setPosition(winSize.width / 2 - 100.0f, winSize.height / 2 + y);

    slider->setValue(val);
    slider->updateBar();

    this->m_pLayer->addChild(label);
    this->m_pLayer->addChild(slider);

    this->addItem(label);
    this->addItem(slider);
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
    auto [x, y] = getItemPos(large, true);

    auto btn = CCMenuItemSpriteExtra::create(sprite, this, callback);
    btn->setPosition(x + (large ? 0 : this->m_pLrSize.width / 4), y);
    this->m_pButtonMenu->addChild(btn);

    this->addItem(btn);
    this->incrementPageCount();

    return btn;
}

void BESettingsLayer::addToggle(
    const char* text,
    const char* desc,
    bool value,
    BE_Callback_B cb,
    bool experimental
) {
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

    toggle->toggle(value);
    toggle->setUserData(as<void*>(cb));

    if (desc && strlen(desc)) {
        auto infoButton = CCMenuItemSpriteExtra::create(
            CCNodeConstructor()
                .fromFrameName("GJ_infoIcon_001.png")
                .scale(.5f)
                .done(),
            this,
            (SEL_MenuHandler)&BESettingsLayer::onInfo
        );

        infoButton->setPosition(toggle->getPosition() + cocos2d::CCPoint { -15.0f, 15.0f });
        infoButton->setUserObject(CCString::create(desc));

        this->m_pButtonMenu->addChild(infoButton);
        this->addItem(infoButton);
    }

    if (experimental) {
        auto eLabel = CCMenuItemSpriteExtra::create(
            CCNodeConstructor<CCLabelBMFont*>()
                .fromText("*", "bigFont.fnt")
                .color({ 255, 80, 0 })
                .scale(.7f)
                .done(),
            this,
            (SEL_MenuHandler)&BESettingsLayer::onInfo
        );

        eLabel->setPosition(
            x + 20.0f + label->getScaledContentSize().width,
            y + 2.0f
        );
        eLabel->setUserObject(CCString::create(
            "<cr>Warning</c>\n\n "
            "This feature is <co>experimental</c>, and may contain "
            "<cy>bugs</c> ranging from <cp>visual glitches</c> to potentially <cl>crashing "
            "the game</c>.\n\n <cc>Proceed at your own risk!</c>"
        ));

        this->m_pButtonMenu->addChild(eLabel);
        this->addItem(eLabel);
    }

    this->m_pButtonMenu->addChild(toggle);
    this->m_pButtonMenu->addChild(label);

    this->addItem(toggle);
    this->addItem(label);
    this->incrementPageCount();
}


void BESettingsLayer::onInfo(CCObject* pSender) {
    FLAlertLayer::create(
        nullptr,
        "Info",
        "OK", nullptr,
        380.0f,
        as<CCString*>(as<CCNode*>(pSender)->getUserObject())->getCString()
    )->show();
}

void BESettingsLayer::onPage(CCObject* pSender) {
    for (auto page : this->m_vPages)
        for (auto item : page) {
            item->setVisible(false);
            if (dynamic_cast<CCMenuItemSprite*>(item))
                as<CCMenuItemSpriteExtra*>(item)->setEnabled(false);
            if (dynamic_cast<InputNode*>(item)) {
                as<InputNode*>(item)->getInputNode()->setTouchEnabled(false);
                as<InputNode*>(item)->getInputNode()->detachWithIME();
            }
        }
    
    auto add = as<int>(as<CCNode*>(pSender)->getUserData());
    this->m_nCurrentPage += add;

    if (this->m_nCurrentPage > (int)this->m_vPages.size() - 1)
        this->m_nCurrentPage = 0;
    else if (this->m_nCurrentPage < 0)
        this->m_nCurrentPage = this->m_vPages.size() - 1;

    for (auto item : this->m_vPages[this->m_nCurrentPage]) {
        item->setVisible(true);
        if (dynamic_cast<CCMenuItemSprite*>(item))
            as<CCMenuItemSpriteExtra*>(item)->setEnabled(true);
        if (dynamic_cast<InputNode*>(item))
            as<InputNode*>(item)->getInputNode()->setTouchEnabled(true);
    }
    
    g_nSettingsPage = this->m_nCurrentPage;
}

void BESettingsLayer::onToggle(CCObject* pSender) {
    auto toggle = as<CCMenuItemToggler*>(pSender);

    if (toggle && toggle->getUserData())
        (reinterpret_cast<BE_Callback_B>(toggle->getUserData()))(!toggle->isToggled());
}

void BESettingsLayer::textChanged(CCTextInputNode* input) {
    if (input && input->getUserData() && input->getString() && strlen(input->getString()))
        (reinterpret_cast<BE_Callback>(input->getUserData()))(input->getString());
}

void BESettingsLayer::onShowAccount(CCObject* pSender) {
    if (as<CCNode*>(pSender)->getUserData())
        ProfilePage::create(as<int>(as<CCNode*>(pSender)->getUserData()), true)->show();
}

void BESettingsLayer::onURL(CCObject* pSender) {
    ShellExecuteA(
        0, 0,
        as<CCString*>(as<CCNode*>(pSender)->getUserObject())->getCString(),
        0, 0 ,
        SW_SHOW
    );
}

void BESettingsLayer::onClose(CCObject* pSender) {
    GameManager::sharedState()->getEditorLayer()->getEditorUI()->updateGridNodeSize();

    as<CCMenu*>(m_pPauseLayer->getChildren()->objectAtIndex(0))->setEnabled(true);
    as<CCMenu*>(m_pPauseLayer->m_pButton0->getParent())->setEnabled(true);

    BrownAlertDelegate::onClose(pSender);
}

void BESettingsLayer::keyDown(enumKeyCodes key) {
    switch (key) {
        case KEY_Left:
            onPage(m_pPrevPageBtn);
            break;
        case KEY_Right:
            onPage(m_pNextPageBtn);
            break;
        default:
            BrownAlertDelegate::keyDown(key);
    }
}

BESettingsLayer* BESettingsLayer::create(EditorPauseLayer* pause) {
    auto ret = new BESettingsLayer();

    if (ret && (ret->m_pPauseLayer = pause) && ret->init(340.0f, 240.0f, "GJ_square01.png", "BetterEdit Settings")) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}
