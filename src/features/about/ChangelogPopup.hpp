#pragma once

#include <utils/PopupWithCorners.hpp>
#include <Geode/utils/VersionInfo.hpp>

using namespace geode::prelude;

class ChangelogPopup : public PopupWithCorners {
protected:
    bool init(std::optional<VersionInfo> const& since);

public:
    static ChangelogPopup* create(std::optional<VersionInfo> const& since = std::nullopt);
};
