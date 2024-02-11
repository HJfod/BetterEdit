#pragma once

#include <utils/PopupWithCorners.hpp>

using namespace geode::prelude;

class ChangelogPopup : public PopupWithCorners<std::optional<VersionInfo> const&> {
protected:
    bool setup(std::optional<VersionInfo> const& since) override;

public:
    static ChangelogPopup* create(std::optional<VersionInfo> const& since = std::nullopt);
};
