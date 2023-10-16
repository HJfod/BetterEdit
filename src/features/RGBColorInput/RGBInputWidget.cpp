#include "RGBInputWidget.hpp"

// Credit to mat for creating this
// https://github.com/matcool/geode-mods/blob/main/rgb-color-input/

bool RGBInputWidget::init(ColorSelectPopup* colorSelect, SetupPulsePopup* setupPulse) {
    m_colorSelect = colorSelect;
    m_setupPulse = setupPulse;

    const ccColor3B placeholder = { 100, 100, 100 };
    constexpr float totalWidth = 115.0f;
    constexpr float spacing = 4.0f;
    constexpr float componentWidth = (totalWidth - spacing * 2.0f) / 3.0f;
    constexpr float componentHeight = (22.5f);
    constexpr float hexHeight = 30.0f;
    constexpr float hexYPos = -hexHeight - spacing / 2.f;
    constexpr float redXPos = -componentWidth - spacing;
    constexpr float blueXPos = -redXPos;
    constexpr float bgScale = 1.6f;
    constexpr GLubyte opacity = 100;

    m_redInput = CCTextInputNode::create(30.f, 20.f, "R", "bigFont.fnt");
    m_redInput->setAllowedChars("0123456789");
    m_redInput->m_maxLabelLength = 3;
    m_redInput->setMaxLabelScale(0.6f);
    m_redInput->setLabelPlaceholderColor(placeholder);
    m_redInput->setPositionX(redXPos);
    m_redInput->setID("red-input"_spr);
    m_redInput->setDelegate(this);

    m_greenInput = CCTextInputNode::create(30.f, 20.f, "G", "bigFont.fnt");
    m_greenInput->setAllowedChars("0123456789");
    m_greenInput->m_maxLabelLength = 3;
    m_greenInput->setMaxLabelScale(0.6f);
    m_greenInput->setLabelPlaceholderColor(placeholder);
    m_greenInput->setID("green-input"_spr);
    m_greenInput->setDelegate(this);

    m_blueInput = CCTextInputNode::create(30.f, 20.f, "B", "bigFont.fnt");
    m_blueInput->setAllowedChars("0123456789");
    m_blueInput->m_maxLabelLength = 3;
    m_blueInput->setMaxLabelScale(0.6f);
    m_blueInput->setLabelPlaceholderColor(placeholder);
    m_blueInput->setPositionX(blueXPos);
    m_blueInput->setID("blue-input"_spr);
    m_blueInput->setDelegate(this);

    m_hexInput = CCTextInputNode::create(100.f, 20.f, "HEX", "bigFont.fnt");
    m_hexInput->setAllowedChars("0123456789ABCDEFabcdef");
    m_hexInput->m_maxLabelLength = 6;
    m_hexInput->setMaxLabelScale(0.7f);
    m_hexInput->setLabelPlaceholderColor(placeholder);
    m_hexInput->setPositionY(hexYPos);
    m_hexInput->setID("hex-input"_spr);
    m_hexInput->setDelegate(this);

    this->addChild(m_blueInput);
    this->addChild(m_greenInput);
    this->addChild(m_redInput);
    this->addChild(m_hexInput);

    this->updateLabels(true, true);

    // Hex BG
    auto bg = CCScale9Sprite::create("square02_small.png");
    bg->setContentSize({ totalWidth * bgScale, hexHeight * bgScale });
    bg->setScale(1.0f / bgScale);
    bg->setZOrder(-1);
    bg->setPositionY(hexYPos);
    bg->setOpacity(opacity);
    bg->setID("hex-input-bg"_spr);
    this->addChild(bg);

    // Red BG
    bg = CCScale9Sprite::create("square02_small.png");
    bg->setContentSize({ componentWidth * bgScale, componentHeight * bgScale });
    bg->setScale(1.0f / bgScale);
    bg->setZOrder(-1);
    bg->setPositionX(redXPos);
    bg->setOpacity(opacity);
    bg->setID("red-input-bg"_spr);
    this->addChild(bg);

    // Green BG
    bg = CCScale9Sprite::create("square02_small.png");
    bg->setContentSize({ componentWidth * bgScale, componentHeight * bgScale });
    bg->setScale(1.0f / bgScale);
    bg->setZOrder(-1);
    bg->setOpacity(opacity);
    bg->setID("green-input-bg"_spr);
    this->addChild(bg);

    // Blue BG
    bg = CCScale9Sprite::create("square02_small.png");
    bg->setContentSize({ componentWidth * bgScale, componentHeight * bgScale });
    bg->setScale(1.0f / bgScale);
    bg->setZOrder(-1);
    bg->setPositionX(blueXPos);
    bg->setOpacity(opacity);
    bg->setID("blue-input-bg"_spr);
    this->addChild(bg);

    return true;
}


void RGBInputWidget::textChanged(CCTextInputNode* input) {
    if (m_ignore) {
        return;
    }

    ccColor3B color;
    if (!input->getString() || !strlen(input->getString())) {
        return;
    }

    bool hex = false;
    bool rgb = false;

    if (input->getID() == "hex-input"_spr) {
        auto result = this->getColorFromHex();
        if (result.has_error()) {
            return;
        }
        rgb = true;
        color = result.unwrap();
    } else {
        color = this->getColorFromRGB(input);
        hex = true;
    }

    m_ignore = true;
    this->setParentColor(color);
    m_ignore = false;
    this->updateLabels(hex, rgb);
}

Result<ccColor3B> RGBInputWidget::getColorFromHex() {
    ccColor3B ret;
    std::string value = m_hexInput->getString();

    if (value.empty()) {
        return Err("skip");
    }

    if (value.size() > 6) {
        FLAlertLayer::create("Sneaky...", "Stop that!", "Ok")->show();
        return Ok(this->getParentColor());
    }

    int intValue;
    try {
        intValue = std::stoi(value, 0, 16);
    } catch (...) {
        FLAlertLayer::create("Sneaky...", "Stop that!", "Ok")->show();
        return Ok(this->getParentColor());
    }

    switch (value.size()) {
        case 6: {
            auto r = static_cast<uint8_t>((intValue & 0xFF0000) >> 16);
            auto g = static_cast<uint8_t>((intValue & 0x00FF00) >> 8);
            auto b = static_cast<uint8_t>((intValue & 0x0000FF));

            ret = {r, g, b};
        } break;
        case 3: {
            auto r = static_cast<uint8_t>(((intValue & 0xF00) >> 8) * 17);
            auto g = static_cast<uint8_t>(((intValue & 0x0F0) >> 4) * 17);
            auto b = static_cast<uint8_t>(((intValue & 0x00F))      * 17);

            ret = {r, g, b};
        } break;
        case 2: {
            auto number = static_cast<uint8_t>(intValue);
            ret = { number, number, number };
        } break;
        default:
            return Err("skip");
    }


    return Ok(ret);
}

ccColor3B RGBInputWidget::getColorFromRGB(CCTextInputNode* input) {
    std::string value = input->getString();
    try {
        auto numeric = value.empty() ? 0 : std::stoi(value);
        if (numeric > 255) {
            numeric = 255;
            input->setString("255");
        }

        auto byte = static_cast<GLubyte>(numeric);
        auto color = this->getParentColor();

        if (input->getID() == "red-input"_spr) {
            color.r = byte;
        } else if (input->getID() == "green-input"_spr) {
            color.g = byte;
        } else if (input->getID() == "blue-input"_spr) {
            color.b = byte;
        }

        return color;
    } catch (...) {
        return this->getParentColor();
    }
}

void RGBInputWidget::updateLabels(bool hex, bool rgb) {
    if (m_ignore) {
        return;
    }
    m_ignore = true;

    auto color = this->getParentColor();
    if (hex) {
        m_hexInput->setString(this->colorToHex(color));
    }

    if (rgb) {
        m_redInput->setString(std::to_string(color.r).c_str());
        m_greenInput->setString(std::to_string(color.g).c_str());
        m_blueInput->setString(std::to_string(color.b).c_str());
    }
    m_ignore = false;
}

ccColor3B RGBInputWidget::getParentColor() {
    if (m_colorSelect != nullptr) {
        return m_colorSelect->m_colorPicker->getColorValue();
    }

    if (m_setupPulse != nullptr) {
        return m_setupPulse->m_colorPicker->getColorValue();
    }

    throw std::runtime_error("lmao");
}

void RGBInputWidget::setParentColor(ccColor3B color) {
    if (m_colorSelect != nullptr) {
        m_colorSelect->m_colorPicker->setColorValue(color);
    }

    if (m_setupPulse != nullptr) {
        m_setupPulse->m_colorPicker->setColorValue(color);
    }
}

inline std::string RGBInputWidget::colorToHex(ccColor3B color) {
    // Shamelessly stolen from BEv4 :)

    static constexpr auto digits = "0123456789ABCDEF";
    std::string output;
    output += digits[color.r >> 4 & 0xF];
    output += digits[color.r      & 0xF];
    output += digits[color.g >> 4 & 0xF];
    output += digits[color.g      & 0xF];
    output += digits[color.b >> 4 & 0xF];
    output += digits[color.b      & 0xF];
    return output;
}