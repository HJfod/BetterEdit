#pragma once

#include "../Macros.hpp"
#include <Geode/utils/cocos.hpp>

namespace better_edit {
    class BE_DLL Label : public cocos2d::CCNodeRGBA, public cocos2d::CCLabelProtocol {
    protected:
        std::string m_text;
        std::string m_font;
        cocos2d::CCSize m_size;
        cocos2d::AxisAlignment m_hAlign = cocos2d::AxisAlignment::Center;
        float m_maxScale = 1.f;

        bool init(std::string const& text, const char* font, cocos2d::CCSize const& size);
        void updateContent();

    public:
        static Label* create(std::string const& text, const char* font, cocos2d::CCSize const& size);

        void setMaxScale(float scale);
        float getMaxScale() const;

        void setAlignment(cocos2d::AxisAlignment align);
        cocos2d::AxisAlignment getAlignment() const;

        void setFont(const char* font);
        std::string getFont() const;

        void setString(const char* label) override;
        const char* getString() override;
    };
}
