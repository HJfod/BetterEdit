#pragma once

#include <Geode/ui/Popup.hpp>

using namespace geode::prelude;

class InfoPopup : public Popup<std::string const&> {
protected:
    CCNode* m_content;
    CCNode* m_last = nullptr;

    bool setup(std::string const& title) override;

public:
    static InfoPopup* create(std::string const& title);

    void setDirection(Axis axis);
    void addIconText(const char* icon, std::string const& text);
    void addBreak();
};

