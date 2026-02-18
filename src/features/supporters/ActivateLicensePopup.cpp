#include "ActivateLicensePopup.hpp"
#include <Geode/binding/ButtonSprite.hpp>
#include <Geode/ui/Popup.hpp>
#include <Geode/ui/Notification.hpp>
#include <Geode/ui/LoadingSpinner.hpp>
#include <server/Server.hpp>

using namespace pro;
using namespace pro::server;

class ActivatingLicensePopup : public Popup {
protected:
    Ref<ActivateLicensePopup> m_popup;
    async::TaskHolder<Result<ActivatedLicense>> m_listener;

    bool init(ActivateLicensePopup* popup, std::string const& key) {
        if (!Popup::init(180, 100, "square01_001.png", CCRectZero))
            return false;

        m_closeBtn->setVisible(false);
        m_popup = popup;

        this->setTitle("Activating license...");

        auto spinner = LoadingSpinner::create(40);
        m_mainLayer->addChildAtPosition(spinner, Anchor::Center, ccp(0, -10));

        m_listener.spawn(
            server::activateLicense(key),
            [this](Result<ActivatedLicense> result) {
                this->onRequest(std::move(result));
            }
        );

        return true;
    }

    void onRequest(Result<ActivatedLicense> result) {
        if (result.isOk()) {
            // Copy info first as onClose may free the listener which will free the event
            auto info = std::move(result).unwrap();
            this->onClose(nullptr);
            m_popup->onClose(nullptr);

            auto save = saveProKey(info.token);
            if (!save) {
                FLAlertLayer::create(
                    "Unable to Activate",
                    fmt::format(
                        "Unable to save the BetterEdit License Key!\n"
                        "<cr>Please contact HJfod for assistance</c>\n"
                        "Error: {}",
                        save.unwrapErr()
                    ),
                    "OK"
                )->show();
            }
            else {
                createQuickPopup(
                    "Activated!",
                    "<cp>Thank you for supporting BetterEdit! <3\n</c>"
                    "<cy>Please restart</c> to finish setting up Supporter features :)",
                    "OK", "Restart",
                    [](auto*, bool btn2) {
                        if (btn2) {
                            game::restart(true);
                        }
                    }
                );
            }
        }
        else {
            this->onClose(nullptr);
            FLAlertLayer::create("Unable to Activate", std::move(result).unwrapErr(), "OK")->show();
        }
    }

public:
    static ActivatingLicensePopup* create(ActivateLicensePopup* popup, std::string const& key) {
        auto ret = new ActivatingLicensePopup();
        if (ret && ret->init(popup, key)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_RELEASE(ret);
        return nullptr;
    }
};

bool ActivateLicensePopup::init() {
    if (!Popup::init(290, 200))
        return false;

    this->setTitle("Activate Supporter");

    auto activateTitle = CCLabelBMFont::create("Enter Your Activation Key", "goldFont.fnt");
    activateTitle->setScale(.5f);
    m_mainLayer->addChildAtPosition(activateTitle, Anchor::Center, ccp(0, 50));

    auto inputs = CCNode::create();
    inputs->setContentWidth(m_size.width - 20);
    inputs->setAnchorPoint({ .5f, .5f });
    for (size_t i = 0; i < 5u; i += 1) {
        if (i > 0) {
            auto dashOffsetContainer = CCNode::create();
            auto dash = CCLabelBMFont::create("-", "bigFont.fnt");
            dashOffsetContainer->setContentSize(dash->getContentSize());
            dashOffsetContainer->addChildAtPosition(dash, Anchor::Center, ccp(0, 3));
            inputs->addChild(dashOffsetContainer);
        }
        auto input = TextInput::create(55, i == 0 ? "BEXX" : "XXXX");
        m_inputs[i] = input;
        input->setMaxCharCount(4);
        input->setCallback([this, i](std::string const& text) {
            if (i + 1 < m_inputs.size() && text.size() >= 4) {
                m_inputs[i]->defocus();
                m_inputs[i + 1]->focus();
            }
            else if (i > 0 && text.size() == 0) {
                m_inputs[i]->defocus();
                m_inputs[i - 1]->focus();
            }
            this->updateState();
        });
        inputs->addChild(input);
    }
    inputs->setLayout(RowLayout::create());
    m_mainLayer->addChildAtPosition(inputs, Anchor::Center, ccp(0, 20));

    m_keyErrorLabel = CCLabelBMFont::create("", "bigFont.fnt");
    m_keyErrorLabel->setColor(ccc3(255, 55, 0));
    m_keyErrorLabel->setScale(.4f);
    m_mainLayer->addChildAtPosition(m_keyErrorLabel, Anchor::Center, ccp(0, 0));

    auto pasteSpr = ButtonSprite::create("Paste from Clipboard", "goldFont.fnt", "GJ_button_05.png", .8f);
    pasteSpr->setScale(.6f);
    auto pasteBtn = CCMenuItemSpriteExtra::create(
        pasteSpr, this, menu_selector(ActivateLicensePopup::onPaste)
    );
    m_buttonMenu->addChildAtPosition(pasteBtn, Anchor::Center, ccp(0, -20));

    auto infoSpr = ButtonSprite::create("Where do I get a Key?", "goldFont.fnt", "GJ_button_05.png", .8f);
    infoSpr->setScale(.6f);
    auto infoBtn = CCMenuItemSpriteExtra::create(
        infoSpr, this, menu_selector(ActivateLicensePopup::onInfo)
    );
    m_buttonMenu->addChildAtPosition(infoBtn, Anchor::Center, ccp(0, -40));

    m_activateSpr = ButtonSprite::create("Activate", "goldFont.fnt", "GJ_button_01.png", .8f);
    m_activateSpr->setScale(.9f);
    m_activateBtn = CCMenuItemSpriteExtra::create(
        m_activateSpr, this, menu_selector(ActivateLicensePopup::onActivate)
    );
    m_buttonMenu->addChildAtPosition(m_activateBtn, Anchor::Bottom, ccp(0, 25));

    this->updateState();

    return true;
}

static size_t getCharValue(char ch) {
    if ('0' <= ch && ch <= '9') {
        return ch - '0';
    }
    if ('a' <= ch && ch <= 'z') {
        return ch - 'a' + 10;
    }
    if ('A' <= ch && ch <= 'Z') {
        return ch - 'A' + 10;
    }
    return 0;
}

void ActivateLicensePopup::updateState() {
    bool validKey = false;
    std::string error = "";

    // Validate key
    auto key = getCurrentKey();
    if (key.size() == 20) {
        if (key[0] == 'B' && key[1] == 'E') {
            size_t checksum = getCharValue(key[2]) + getCharValue(key[3]);
            size_t sum = 0;
            for (size_t i = 4; i < key.size(); i += 1) {
                sum += getCharValue(key[i]);
            }
            if (checksum == sum % 18) {
                validKey = true;
            }
            else {
                error = "Checksum mismatch";
            }
        }
        else {
            error = "Key does not start with 'BE'";
        }
    }

    m_keyErrorLabel->setString(error.c_str());
    m_activateSpr->setCascadeColorEnabled(true);
    m_activateSpr->setCascadeOpacityEnabled(true);
    m_activateSpr->setColor(validKey ? ccWHITE : ccGRAY);
    m_activateSpr->setOpacity(validKey ? 255 : 155);
    m_activateBtn->setEnabled(validKey);
}
std::string ActivateLicensePopup::getCurrentKey() const {
    std::string keychars;
    for (auto input : m_inputs) {
        keychars += input->getString();
    }
    string::toUpperIP(keychars);
    return keychars;
}

void ActivateLicensePopup::onPaste(CCObject*) {
    std::string keychars;
    for (auto c : clipboard::read()) {
        if (
            ('0' <= c && c <= '9') ||
            ('a' <= c && c <= 'z') ||
            ('A' <= c && c <= 'Z')
        ) {
            keychars.push_back(c);
        }
    }
    if (keychars.length() != 20) {
        Notification::create("No Product Key in Clipboard", NotificationIcon::Error)->show();
        return;
    }
    for (size_t i = 0; i < m_inputs.size(); i += 1) {
        m_inputs[i]->setString(keychars.substr(4 * i, 4));
    }
    this->updateState();
}
void ActivateLicensePopup::onInfo(CCObject*) {
    FLAlertLayer::create(
        nullptr,
        "Supporter Keys",
        fmt::format(
            "BetterEdit <cp>supporters</c> can get access to <cj>extra features</c> "
            "by activating their account with an <cy>Activation Key</c>.\n"
            "If you have <cg>donated to HJfod on Ko-Fi</c>, DM him on <cp>Ko-Fi</c> or "
            "<ca>Discord</c> to get your activation key.\n"
            "<cb>In the future there will be a website to purchase keys from</c>, but for now "
            "the process is very manual - sorry!\n"
            "<cd>If you have already activated one device and want to activate more for free</c> (up to {}!), "
            "check the <cl>My Supporter Status</c> popup.",
            server::MAX_DEVICE_COUNT
        ),
        "OK", nullptr, 
        380
    )->show();
}
void ActivateLicensePopup::onActivate(CCObject*) {
    ActivatingLicensePopup::create(this, this->getCurrentKey())->show();
}

ActivateLicensePopup* ActivateLicensePopup::create() {
    auto ret = new ActivateLicensePopup();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

ActivateLicensePopup* ActivateLicensePopup::tryShowIfLoggedIn() {
    if (GJAccountManager::get()->m_accountID == 0) {
        FLAlertLayer::create(
            "Login to Activate",
            "Please <cp>Log In to your GD Account</c> to <cy>Activate Supporter Perks</c>!",
            "OK"
        )->show();
        return nullptr;
    }
    else {
        auto ret = ActivateLicensePopup::create();
        ret->show();
        return ret;
    }
}

void ActivateLicensePopup::onClose(CCObject* sender) {
    Popup::onClose(sender);
}
