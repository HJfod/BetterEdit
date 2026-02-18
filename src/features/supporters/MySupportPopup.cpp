#include "MySupportPopup.hpp"
#include <features/supporters/Pro.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
#include <Geode/binding/ButtonSprite.hpp>
#include <Geode/binding/SimplePlayer.hpp>
#include <Geode/ui/Notification.hpp>
#include <Geode/ui/TextInput.hpp>
#include <features/about/AboutBEPopup.hpp>
#include <fmt/chrono.h>

using namespace geode::prelude;
using namespace pro;
using namespace pro::server;

class ActivateNewDevicePopup : public Popup {
protected:
    async::TaskHolder<Result<CreatedProductKey>> m_listener;
    LoadingSpinner* m_loading;

    bool init() {
        if (!Popup::init(290, 200))
            return false;

        this->setTitle("Activate New Device");

        m_loading = LoadingSpinner::create(45);
        m_mainLayer->addChildAtPosition(m_loading, Anchor::Center);

        m_listener.spawn(
            server::createNewLicense(*getProKey()),
            [this](Result<CreatedProductKey> result) {
                this->onRequest(std::move(result));
            }
        );

        handleTouchPriority(this);
        
        return true;
    }

    void onRequest(Result<CreatedProductKey> result) {
        if (result.isOk()) {
            m_loading->setVisible(false);

            auto key = std::move(result).unwrap().key;

            auto useInfoLabel = CCLabelBMFont::create(
                "Enter this code on the other device:", "bigFont.fnt"
            );
            useInfoLabel->setColor({ 0, 255, 55 });
            useInfoLabel->setScale(.35f);
            m_mainLayer->addChildAtPosition(useInfoLabel, Anchor::Center, ccp(0, 40));

            auto keyBG = CCScale9Sprite::create("square02_001.png");
            keyBG->setOpacity(90);
            keyBG->setScale(.5f);
            keyBG->setContentSize(ccp(250, 30) * 2);

            auto keyLabel = CCLabelBMFont::create(key.c_str(), "bigFont.fnt");
            keyLabel->limitLabelWidth(keyBG->getContentWidth() - 10, 1.f, .1f);
            keyBG->addChildAtPosition(keyLabel, Anchor::Center);

            m_mainLayer->addChildAtPosition(keyBG, Anchor::Center, ccp(0, 15));

            auto copySpr = ButtonSprite::create("Copy to Clipboard", "goldFont.fnt", "GJ_button_05.png", .8f);
            copySpr->setScale(.6f);
            auto copyBtn = CCMenuItemExt::createSpriteExtra(
                copySpr, [key](auto) {
                    if (clipboard::write(key)) {
                        Notification::create("Copied to Clipboard", NotificationIcon::Success)->show();
                    }
                    else {
                        Notification::create("Failed to Copy", NotificationIcon::Error)->show();
                    }
                }
            );
            m_buttonMenu->addChildAtPosition(copyBtn, Anchor::Center, ccp(0, -15));

            auto useInfo2Label = CCLabelBMFont::create(
                "You can see this code again later!",
                "bigFont.fnt"
            );
            useInfo2Label->setAlignment(CCTextAlignment::kCCTextAlignmentCenter);
            useInfo2Label->setColor({ 55, 255, 255 });
            useInfo2Label->setScale(.35f);
            m_mainLayer->addChildAtPosition(useInfo2Label, Anchor::Center, ccp(0, -40));

            auto useInfo3Label = CCLabelBMFont::create(
                "You need to be logged in on the same account on the other \n"
                "device. If you have an alt account, contact HJfod!",
                "bigFont.fnt"
            );
            useInfo3Label->setColor({ 205, 205, 205 });
            useInfo3Label->setOpacity(205);
            useInfo3Label->setAlignment(CCTextAlignment::kCCTextAlignmentCenter);
            useInfo3Label->setScale(.25f);
            m_mainLayer->addChildAtPosition(useInfo3Label, Anchor::Bottom, ccp(0, 20));
        }
        else {
            this->onClose(nullptr);
            FLAlertLayer::create(
                "Error",
                fmt::format(
                    "Unable to create a new activation key: {}",
                    std::move(result).unwrapErr()
                ),
                "OK"
            )->show();
        }
    }

public:
    static ActivateNewDevicePopup* create() {
        auto ret = new ActivateNewDevicePopup();
        if (ret && ret->init()) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};

bool MySupportPopup::init() {
    if (!PopupWithCorners::init(358, 270, "GJ_square02.png"))
        return false;

    m_noElasticity = true;

    this->setTitle("Supporter Status");
    this->addCorners(Corner::Blue, Corner::Gold);
    this->setCloseButtonSpr(CCSprite::createWithSpriteFrameName("GJ_backBtn_001.png"), .75f);

    auto row = CCNode::create();
    row->setContentWidth(m_size.width - 30);
    row->setAnchorPoint({ .5f, .5f });

    m_playerInfo = CCNode::create();
    m_playerInfo->setAnchorPoint({ .5f, .5f });
    m_playerInfo->setContentSize({ 120, 30 });

    auto playerInfoBG = CCScale9Sprite::create("square02_001.png");
    playerInfoBG->setOpacity(150);
    playerInfoBG->setScale(.5f);
    playerInfoBG->setContentSize(m_playerInfo->getContentSize() * 2);
    m_playerInfo->addChildAtPosition(playerInfoBG, Anchor::Center);

    m_playerIcon = SimplePlayer::create(0);
    m_playerIcon->setScale(.5f);
    m_playerInfo->addChildAtPosition(m_playerIcon, Anchor::Left, ccp(m_playerInfo->getContentHeight() / 2, 0));

    m_playerName = CCLabelBMFont::create("", "bigFont.fnt");
    m_playerInfo->addChildAtPosition(m_playerName, Anchor::Left, ccp(m_playerInfo->getContentHeight(), 0), ccp(0, .5f));

    m_playerInfoLoading = LoadingSpinner::create(20);
    m_playerInfo->addChildAtPosition(m_playerInfoLoading, Anchor::Center);

    row->addChild(m_playerInfo);

    auto showMeMenu = CCMenu::create();
    showMeMenu->setAnchorPoint({ .5f, .5f });
    showMeMenu->setContentSize({ 160, 30 });
    showMeMenu->ignoreAnchorPointForPosition(false);

    auto showMeMenuBG = CCScale9Sprite::create("square02_001.png");
    showMeMenuBG->setOpacity(150);
    showMeMenuBG->setScale(.5f);
    showMeMenuBG->setContentSize(showMeMenu->getContentSize() * 2);
    showMeMenu->addChildAtPosition(showMeMenuBG, Anchor::Center);

    m_showMeToggle = CCMenuItemToggler::createWithStandardSprites(this, menu_selector(MySupportPopup::onShowMe), .5f);
    showMeMenu->addChildAtPosition(m_showMeToggle, Anchor::Left, ccp(showMeMenu->getContentHeight() / 2, 0));

    m_showMeToggleLoading = LoadingSpinner::create(20);
    showMeMenu->addChildAtPosition(m_showMeToggleLoading, Anchor::Left, ccp(showMeMenu->getContentHeight() / 2, 0));

    auto showMeLabel = CCLabelBMFont::create("Show Support Publicly", "bigFont.fnt");
    showMeLabel->limitLabelWidth(showMeMenu->getContentWidth() - showMeMenu->getContentHeight() - 5, .5f, .1f);
    showMeMenu->addChildAtPosition(showMeLabel, Anchor::Left, ccp(showMeMenu->getContentHeight(), 0), ccp(0, .5f));

    row->addChild(showMeMenu);

    row->setLayout(RowLayout::create());
    m_mainLayer->addChildAtPosition(row, Anchor::Center, ccp(0, 80));

    m_supporterSince = CCLabelBMFont::create("", "goldFont.fnt");
    m_supporterSince->setScale(.5f);
    m_mainLayer->addChildAtPosition(m_supporterSince, Anchor::Center, ccp(0, 50));

    m_supporterSinceLoading = LoadingSpinner::create(20);
    m_mainLayer->addChildAtPosition(m_supporterSinceLoading, Anchor::Center, ccp(0, 50));

    auto devicesContainer = CCMenu::create();
    devicesContainer->ignoreAnchorPointForPosition(false);
    devicesContainer->setContentSize({ m_playerInfo->getContentWidth() + 5 + showMeMenu->getContentWidth(), 120 });
    devicesContainer->setAnchorPoint({ .5f, .5f });

    auto devicesTitle = CCLabelBMFont::create("Devices", "bigFont.fnt");
    devicesTitle->setScale(.4f);
    devicesContainer->addChildAtPosition(devicesTitle, Anchor::TopLeft, ccp(0, 10), ccp(0, .5f));

    auto devicesInfoSpr = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
    devicesInfoSpr->setScale(.5f);
    auto devicesInfoBtn = CCMenuItemSpriteExtra::create(
        devicesInfoSpr, this, menu_selector(MySupportPopup::onDevicesInfo)
    );
    devicesContainer->addChildAtPosition(devicesInfoBtn, Anchor::TopRight, ccp(-6, 10));

    auto devicesBG = CCScale9Sprite::create("square02_001.png");
    devicesBG->setScale(.5f);
    devicesBG->setOpacity(150);
    devicesBG->setContentSize(devicesContainer->getContentSize() * 2);
    devicesContainer->addChildAtPosition(devicesBG, Anchor::Center);

    m_devicesList = CCNode::create();
    m_devicesList->setContentSize(devicesContainer->getContentSize());
    m_devicesList->setAnchorPoint({ .5f, .5f });
    m_devicesList->setLayout(
        ColumnLayout::create()
            ->setGap(0)
            ->setCrossAxisOverflow(false)
            ->setAxisReverse(true)
            ->setAxisAlignment(AxisAlignment::End)
    );
    devicesContainer->addChildAtPosition(m_devicesList, Anchor::Center);

    m_devicesLoading = LoadingSpinner::create(30);
    devicesContainer->addChildAtPosition(m_devicesLoading, Anchor::Center);
    
    m_mainLayer->addChildAtPosition(devicesContainer, Anchor::Center, ccp(0, -40));

    m_errorLabel = CCLabelBMFont::create("", "bigFont.fnt");
    m_errorLabel->setColor(ccc3(255, 55, 0));
    m_errorLabel->setScale(.5f);
    m_mainLayer->addChildAtPosition(m_errorLabel, Anchor::Bottom, ccp(0, 20));
    
    auto reloadSpr = CCSprite::createWithSpriteFrameName("GJ_updateBtn_001.png");
    reloadSpr->setScale(.7f);
    auto reloadBtn = CCMenuItemSpriteExtra::create(
        reloadSpr, this, menu_selector(MySupportPopup::onReload)
    );
    m_buttonMenu->addChildAtPosition(reloadBtn, Anchor::BottomLeft, ccp(2, 2));

    this->reloadData();

    return true;
}

void MySupportPopup::setError(std::string const& error) {
    m_playerInfoLoading->setVisible(false);
    m_showMeToggleLoading->setVisible(false);
    m_supporterSinceLoading->setVisible(false);
    m_devicesLoading->setVisible(false);
    m_errorLabel->setString(error.c_str());
}

void MySupportPopup::reloadData() {
    m_supporterSince->setVisible(false);
    m_showMeToggle->setVisible(false);
    m_playerName->setVisible(false);
    m_playerIcon->setVisible(false);

    m_devicesList->removeAllChildren();

    m_playerInfoLoading->setVisible(true);
    m_showMeToggleLoading->setVisible(true);
    m_supporterSinceLoading->setVisible(true);
    m_devicesLoading->setVisible(true);

    m_errorLabel->setString("");
    if (auto key = getProKey()) {
        m_mySupportListener.spawn(
            server::getMySupport(*key),
            [this](Result<pro::server::MySupport> result) {
                this->onLoadData(std::move(result));
            }
        );
    }
    else {
        this->setError("Supporter Key not Found");
    }
}
void MySupportPopup::onLoadData(Result<pro::server::MySupport> result) {
    if (result.isErr()) {
        log::error("Error loading user data: {}", std::move(result).unwrapErr());
        this->setError("Error loading data");
        return;
    }
    this->setError("");
    auto data = std::move(result).unwrap();

    m_playerName->setString(data.gdInfo.username.c_str());
    m_playerName->setColor(pro::getSupporterColor(data.supportedAmount));
    m_playerName->limitLabelWidth(m_playerInfo->getContentWidth() - m_playerInfo->getContentHeight() * 1.5f, .5f, .1f);
    m_playerName->setVisible(true);

    data.gdInfo.update(m_playerIcon);
    m_playerIcon->setVisible(true);

    m_showMeToggle->toggle(data.showingPublicly);
    m_showMeToggle->setVisible(true);

    m_supporterSince->setString(fmt::format("Supporter since {:%d/%m/%Y}", data.supportedOn).c_str());
    m_supporterSince->setVisible(true);

    for (auto device : data.devices) {
        auto node = CCMenu::create();
        node->ignoreAnchorPointForPosition(false);
        node->setContentSize({
            m_devicesList->getContentWidth(),
            m_devicesList->getContentHeight() / server::MAX_DEVICE_COUNT
        });

        float offset = 5;

        auto nameLoading = LoadingSpinner::create(10);
        nameLoading->setID(fmt::format("loading-{}", device.deviceID));
        nameLoading->setVisible(false);
        node->addChildAtPosition(nameLoading, Anchor::Left, ccp(offset + 5, 0));
        
        auto nameInput = TextInput::create(100, "Unnamed");
        nameInput->setScale(.6f);
        nameInput->setID(fmt::format("input-{}", device.deviceID));
        nameInput->setString(device.deviceName);
        nameInput->setTextAlign(TextInputAlign::Left);
        node->addChildAtPosition(nameInput, Anchor::Left, ccp(offset, 0), ccp(0, .5f));

        offset += nameInput->getScaledContentWidth() + 5;
        offset += 5;

        auto updateNameSpr = CCSprite::createWithSpriteFrameName("GJ_updateBtn_001.png");
        updateNameSpr->setScale(.3f);
        auto updateNameBtn = CCMenuItemSpriteExtra::create(
            updateNameSpr, this, menu_selector(MySupportPopup::onUpdateDeviceName)
        );
        updateNameBtn->setID(fmt::format("update-name-btn-{}", device.deviceID));
        updateNameBtn->setUserObject(CCString::create(device.deviceID));
        node->addChildAtPosition(updateNameBtn, Anchor::Left, ccp(offset, 0));

        offset += 20;

        auto platform = CCLabelBMFont::create(
            PlatformID::toString(device.devicePlatform.m_value).data(),
            "goldFont.fnt"
        );
        platform->setScale(.35f);
        node->addChildAtPosition(platform, Anchor::Left, ccp(offset, 5), ccp(0, .5f));

        auto date = CCLabelBMFont::create(
            fmt::format("Activated on {:%d/%m/%Y}", device.activatedOn).c_str(),
            "bigFont.fnt"
        );
        date->setScale(.3f);
        node->addChildAtPosition(date, Anchor::Left, ccp(offset, -5), ccp(0, .5f));

        offset += 50;

        if (getDeviceSalt() == device.deviceID) {
            auto thisDeviceLabel = CCLabelBMFont::create("(This device)", "bigFont.fnt");
            thisDeviceLabel->setColor({ 0, 255, 155 });
            thisDeviceLabel->setScale(.3f);
            node->addChildAtPosition(thisDeviceLabel, Anchor::Left, ccp(offset, 5), ccp(0, .5f));
        }

        // If there's only one device, refuse to deactivate as that would 
        // lock the user out of their supporter status
        if (data.devices.size() > 1) {
            auto deleteSpr = CCSprite::createWithSpriteFrameName("GJ_resetBtn_001.png");
            deleteSpr->setScale(.5f);
            auto deleteBtn = CCMenuItemSpriteExtra::create(
                deleteSpr, this, menu_selector(MySupportPopup::onDeleteDevice)
            );
            deleteBtn->setUserObject(CCString::create(device.deviceID));
            node->addChildAtPosition(deleteBtn, Anchor::Right, ccp(-12, 0));
        }

        m_devicesList->addChild(node);
    }
    if (data.devices.size() < server::MAX_DEVICE_COUNT) {
        auto menu = CCMenu::create();
        menu->ignoreAnchorPointForPosition(false);
        menu->setContentSize({
            m_devicesList->getContentWidth(),
            m_devicesList->getContentHeight() / server::MAX_DEVICE_COUNT
        });

        auto addAnotherSpr = ButtonSprite::create("Activate New Device", "bigFont.fnt", "GJ_button_05.png", .75f);
        addAnotherSpr->setScale(.35f);
        auto addAnotherBtn = CCMenuItemSpriteExtra::create(
            addAnotherSpr, this, menu_selector(MySupportPopup::onActivateNewDevice)
        );
        menu->addChildAtPosition(addAnotherBtn, Anchor::Center);

        m_devicesList->addChild(menu);
    }
    m_devicesList->updateLayout();
    handleTouchPriority(this);
}

void MySupportPopup::onReload(CCObject*) {
    m_reloadListener.spawn(
        server::clearCaches(),
        [this]() {
            this->reloadData();
        }
    );
}
void MySupportPopup::onClose(CCObject* sender) {
    PopupWithCorners::onClose(sender);
    Loader::get()->queueInMainThread([]() {
        AboutBEPopup::create()->show();
    });
}
void MySupportPopup::onShowMe(CCObject* sender) {
    if (auto key = pro::getProKey()) {
        m_showMeToggleLoading->setVisible(true);
        m_showMeToggle->setVisible(false);
        m_updateSupportListener.spawn(
            server::updateSupporter(*key, UpdateSupporter {
                .showPublicly = !static_cast<CCMenuItemToggler*>(sender)->isToggled()
            }),
            [popup = Ref(this)](auto) {
                popup->m_showMeToggle->setVisible(true);
                popup->m_showMeToggleLoading->setVisible(false);
            }
        );
    }
}
void MySupportPopup::onDevicesInfo(CCObject*) {
    FLAlertLayer::create(
        nullptr,
        "More Devices",
        fmt::format(
            "You can activate <cp>BetterEdit Supporter</c> on up to <cy>{} devices</c>.\n"
            "To activate another device, press <cb>Activate New Device</c> and enter the "
            "generated code into the other device.\n"
            "<cj>You must be logged in on the same account on the other device.</c> "
            "<cg>If you have an alt account you would like to activate, contact HJfod.</c>",
            server::MAX_DEVICE_COUNT
        ),
        "OK", nullptr,
        350
    )->show();
}
void MySupportPopup::onDeleteDevice(CCObject* sender) {
    auto id = std::string(static_cast<CCString*>(static_cast<CCNode*>(sender)->getUserObject())->getCString());
    createQuickPopup(
        "Confirm Delete",
        "Are you sure you want to <cr>deactivate this device</c>?\n"
        "This will <cp>disable Supporter Features</c> on it!",
        "Cancel", "Deactivate",
        [popup = Ref(this), id](auto, bool btn2) {
            if (auto key = getProKey(); key && btn2) {
                popup->m_devicesList->removeAllChildren();
                popup->m_devicesLoading->setVisible(true);
                popup->m_updateSupportListener.spawn(
                    server::deactivateLicense(*key, id),
                    [popup, id](auto) {
                        if (id == getDeviceSalt()) {
                            createQuickPopup(
                                "Deactivated!",
                                "<cp>This device has been deactivated!</c>\n"
                                "<cy>Please restart</c> to clean up any leftover resources.",
                                "OK", "Restart",
                                [](auto*, bool btn2) {
                                    if (btn2) {
                                        game::restart(true);
                                    }
                                }
                            );
                        }
                        else {
                            popup->reloadData();
                        }
                    }
                );
            }
        }
    );
}
void MySupportPopup::onUpdateDeviceName(CCObject* sender) {
    if (auto token = getProKey()) {
        auto btn = static_cast<CCNode*>(sender);
        auto id = std::string(static_cast<CCString*>(btn->getUserObject())->getCString());
        auto menu = btn->getParent();

        auto spinner = Ref(menu->querySelector(fmt::format("loading-{}", id)));
        auto updateBtn = Ref(menu->querySelector(fmt::format("update-name-btn-{}", id)));
        auto input = Ref(static_cast<TextInput*>(menu->querySelector(fmt::format("input-{}", id))));

        spinner->setVisible(true);
        updateBtn->setVisible(false);
        input->setVisible(false);

        m_updateDeviceListener.spawn(
            server::updateDeviceInfo(*token, id, UpdateDeviceInfo {
                .deviceName = input->getString()
            }),
            [spinner, updateBtn, input](Result<UpdatedDeviceInfo> res) {
                spinner->setVisible(false);
                updateBtn->setVisible(true);
                input->setVisible(true);
                if (res.isOk()) {
                    input->setString(std::move(res).unwrap().deviceName);
                }
            }
        );
    }
}
void MySupportPopup::onActivateNewDevice(CCObject*) {
    ActivateNewDevicePopup::create()->show();
}

MySupportPopup* MySupportPopup::create() {
    auto ret = new MySupportPopup();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

class $modify(EditorPauseLayer) {
    static void onModify(auto& self) {
        (void)self.setHookPriority("EditorPauseLayer::init", -1);
    }

    $override
    bool init(LevelEditorLayer* lel) {
        if (!EditorPauseLayer::init(lel))
            return false;
        
        if (auto btn = this->querySelector("support-be-btn"_spr)) {
            btn->removeFromParent();
        }
        
        return true;
    }
};

void pro::addAboutPopupStuff(AboutBEPopup* popup) {
    if (!HAS_PRO()) return;

    auto label = CCLabelBMFont::create("Thank You for Supporting BetterEdit <3", "bigFont.fnt");
    label->setColor({ 55, 255, 255 });
    label->setScale(.35f);
    popup->m_mainLayer->addChildAtPosition(label, Anchor::Center, ccp(0, -90));

    if (auto oldBtn = popup->m_mainLayer->querySelector("support-btn")) {
        auto spr = ButtonSprite::create("Support", "goldFont.fnt", "GJ_button_03.png", .8f);
        spr->setScale(.55f);
        auto btn = CCMenuItemExt::createSpriteExtra(spr, [popup](auto) {
            popup->onClose(nullptr);
            // If I don't do this, then ActivateNewDevicePopup becomes click-through...
            Loader::get()->queueInMainThread([]() {
                MySupportPopup::create()->show();
            });
        });
        btn->setID("my-support-btn");

        auto menu = oldBtn->getParent();
        menu->addChild(btn);
        menu->swapChildIndices(oldBtn, btn);
        oldBtn->removeFromParent();
        menu->updateLayout();
    }
}
