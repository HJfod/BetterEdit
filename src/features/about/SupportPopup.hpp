#pragma once

#include <Geode/ui/Popup.hpp>

using namespace geode::prelude;

class SupportPopup : public Popup<bool> {
protected:
    bool setup(bool showDontShowAgain) override;

    void onKofi(CCObject*);
    void onDontShowAgain(CCObject*);

public:
    static SupportPopup* create(bool showDontShowAgain);
};
