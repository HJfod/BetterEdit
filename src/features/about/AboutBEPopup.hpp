#pragma once

#include <Geode/DefaultInclude.hpp>
#include <utils/PopupWithCorners.hpp>

using namespace geode::prelude;

class AboutBEPopup : public PopupWithCorners<> {
protected:
    bool setup() override;

public:
    static AboutBEPopup* create();

    void onClose(CCObject* sender) override;
    void onSettings(CCObject*);
    void onReportBug(CCObject*);
    void onSuggestFeature(CCObject*);
    void onSupport(CCObject*);
    void onSupporters(CCObject*);
    void onChangelog(CCObject*);
    void onSpecialThanks(CCObject*);
    void onDevLink(CCObject* sender);
};

class SpecialThanksPopup : public PopupWithCorners<> {
protected:
    bool setup() override;

public:
    static SpecialThanksPopup* create();
};
