#pragma once

#include <features/supporters/Pro.hpp>
#include <Geode/ui/Popup.hpp>
#include <Geode/ui/TextInput.hpp>

using namespace geode::prelude;

class ActivateLicensePopup : public Popup {
protected:
    std::array<TextInput*, 5> m_inputs;
    CCLabelBMFont* m_keyErrorLabel;
    ButtonSprite* m_activateSpr;
    CCMenuItemSpriteExtra* m_activateBtn;

    bool init() override;

    void updateState();
    std::string getCurrentKey() const;

    void onPaste(CCObject*);
    void onInfo(CCObject*);
    void onActivate(CCObject*);

public:
    static ActivateLicensePopup* create();
    static ActivateLicensePopup* tryShowIfLoggedIn();

    void onClose(CCObject*) override;
};
