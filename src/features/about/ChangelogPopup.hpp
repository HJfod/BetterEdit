#pragma once

#include <utils/PopupWithCorners.hpp>
#include <Geode/utils/VersionInfo.hpp>

using namespace geode::prelude;

class ChangelogPopup : public PopupWithCorners {
protected:
    CCMenuItemToggler* m_showToggle;

    bool init(std::optional<VersionInfo> const& since);

    void onToggleShow(CCObject*);

public:
    static ChangelogPopup* create(std::optional<VersionInfo> const& since = std::nullopt);
};
