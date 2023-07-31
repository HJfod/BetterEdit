#pragma once

#include "../Macros.hpp"
#include <Geode/ui/Popup.hpp>

namespace better_edit {
    class BE_DLL InfoPopup : public geode::Popup<std::string const&> {
    protected:
        cocos2d::CCNode* m_content;
        cocos2d::CCNode* m_last = nullptr;

        bool setup(std::string const& title) override;

    public:
        static InfoPopup* create(std::string const& title);

        void setDirection(cocos2d::Axis axis);
        void addIconText(const char* icon, std::string const& text);
        void addBreak();
    };
}
