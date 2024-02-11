#pragma once

#include <Geode/DefaultInclude.hpp>
#include <utils/PopupWithCorners.hpp>

using namespace geode::prelude;

class AboutBEPopup : public PopupWithCorners<> {
protected:
    bool setup() override;

public:
    static AboutBEPopup* create();

    void onSettings(CCObject*);
    void onReportBug(CCObject*);
    void onSuggestFeature(CCObject*);
    void onSupport(CCObject*);
    void onChangelog(CCObject*);
    void onDevLink(CCObject* sender);
};
