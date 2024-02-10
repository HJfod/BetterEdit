#pragma once

#include <Geode/DefaultInclude.hpp>
#include <Geode/ui/Popup.hpp>

using namespace geode::prelude;

class AboutBEPopup : public Popup<> {
protected:
    bool setup() override;

public:
    static AboutBEPopup* create();

    void onSettings(CCObject*);
    void onReportBug(CCObject*);
    void onSuggestFeature(CCObject*);
    void onSupport(CCObject*);
    void onDevLink(CCObject* sender);
};
