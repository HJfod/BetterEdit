#pragma once

#include <Geode/utils/cocos.hpp>

using namespace geode::prelude;

class Label : public CCNodeRGBA, public CCLabelProtocol {
protected:
    std::string m_text;
    std::string m_font;
    CCSize m_size;
    AxisAlignment m_hAlign = AxisAlignment::Center;
    float m_maxScale = 1.f;

    bool init(std::string const& text, const char* font, CCSize const& size);
    void updateContent();

public:
    static Label* create(std::string const& text, const char* font, CCSize const& size);

    void setMaxScale(float scale);
    float getMaxScale() const;

    void setAlignment(AxisAlignment align);
    AxisAlignment getAlignment() const;

    void setFont(const char* font);
    std::string getFont() const;

    void setString(const char* label) override;
    const char* getString() override;
};
