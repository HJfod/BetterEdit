#include "RGBColorInputWidget.hpp"

inline std::string color_to_hex(ccColor3B color) {
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

bool RGBColorInputWidget::init(ColorSelectPopup* parent) {
    if (!CCLayer::init()) return false;
    this->parent = parent;

    const ccColor3B placeholder_color = {100, 100, 100};

    constexpr float total_w = 115.f;
    constexpr float spacing = 4.f;
    constexpr float comp_width = (total_w - spacing * 2.f) / 3.f; // components (R G B) width
    constexpr float comp_height = 22.5f;
    constexpr float hex_height = 30.f;
    constexpr float hex_y = -hex_height - spacing / 2.f;
    constexpr float r_xpos = -comp_width - spacing;
    constexpr float b_xpos = -r_xpos;
    constexpr float bg_scale = 1.6f;
    constexpr GLubyte opacity = 100;

    red_input = CCTextInputNode::create("R", this, "bigFont.fnt", 30.f, 20.f);
    red_input->setAllowedChars("0123456789");
    red_input->setMaxLabelLength(3);
    red_input->setMaxLabelScale(0.6f);
    red_input->setLabelPlaceholderColor(placeholder_color);
    red_input->setLabelPlaceholerScale(0.5f);
    red_input->setPositionX(r_xpos);
    red_input->setDelegate(this);

    green_input = CCTextInputNode::create("G", this, "bigFont.fnt", 30.f, 20.f);
    green_input->setAllowedChars("0123456789");
    green_input->setMaxLabelLength(3);
    green_input->setMaxLabelScale(0.6f);
    green_input->setLabelPlaceholderColor(placeholder_color);
    green_input->setLabelPlaceholerScale(0.5f);
    green_input->setPositionX(0.f);
    green_input->setDelegate(this);
    
    blue_input = CCTextInputNode::create("B", this, "bigFont.fnt", 30.f, 20.f);
    blue_input->setAllowedChars("0123456789");
    blue_input->setMaxLabelLength(3);
    blue_input->setMaxLabelScale(0.6f);
    blue_input->setLabelPlaceholderColor(placeholder_color);
    blue_input->setLabelPlaceholerScale(0.5f);
    blue_input->setPositionX(b_xpos);
    blue_input->setDelegate(this);

    hex_input = CCTextInputNode::create("hex", this, "bigFont.fnt", 100.f, 20.f);
    hex_input->setAllowedChars("0123456789ABCDEFabcdef");
    hex_input->setMaxLabelLength(6);
    hex_input->setMaxLabelScale(0.7f);
    hex_input->setLabelPlaceholderColor(placeholder_color);
    hex_input->setLabelPlaceholerScale(0.5f);
    hex_input->setPositionY(hex_y);
    hex_input->setDelegate(this);

    addChild(red_input);
    addChild(green_input);
    addChild(blue_input);
    addChild(hex_input);

    update_labels(true, true);

    auto bg = extension::CCScale9Sprite::create("square02_small.png");
    bg->setContentSize({total_w * bg_scale, hex_height * bg_scale});
    bg->setScale(1.f / bg_scale);
    bg->setOpacity(opacity);
    bg->setZOrder(-1);
    bg->setPositionY(hex_y);
    addChild(bg);

    bg = extension::CCScale9Sprite::create("square02_small.png");
    bg->setContentSize({comp_width * bg_scale, comp_height * bg_scale});
    bg->setScale(1.f / bg_scale);
    bg->setOpacity(opacity);
    bg->setZOrder(-1);
    bg->setPositionX(r_xpos);
    addChild(bg);

    bg = extension::CCScale9Sprite::create("square02_small.png");
    bg->setContentSize({comp_width * bg_scale, comp_height * bg_scale});
    bg->setScale(1.f / bg_scale);
    bg->setOpacity(opacity);
    bg->setZOrder(-1);
    addChild(bg);

    bg = extension::CCScale9Sprite::create("square02_small.png");
    bg->setContentSize({comp_width * bg_scale, comp_height * bg_scale});
    bg->setScale(1.f / bg_scale);
    bg->setOpacity(opacity);
    bg->setZOrder(-1);
    bg->setPositionX(b_xpos);
    addChild(bg);

    return true;
}

void RGBColorInputWidget::textChanged(CCTextInputNode* input) {
    if (ignore)
        return;

    if (input == hex_input) {

        std::string value(input->getString());
        ccColor3B color;

        if (value.empty())
            return;

        auto num_value = std::stoi(value, 0, 16);

        switch (value.size()) {
            case 6: {
                auto r = static_cast<uint8_t>((num_value & 0xFF0000) >> 16);
                auto g = static_cast<uint8_t>((num_value & 0x00FF00) >> 8);
                auto b = static_cast<uint8_t>((num_value & 0x0000FF));

                color = {r, g, b};
            } break;

            case 3: {
                auto r = static_cast<uint8_t>(((num_value & 0xF00) >> 8) * 17);
                auto g = static_cast<uint8_t>(((num_value & 0x0F0) >> 4) * 17);
                auto b = static_cast<uint8_t>(((num_value & 0x00F))      * 17);

                color = {r, g, b};
            } break;

            case 2: {
                auto number = static_cast<uint8_t>(num_value);

                color = {number, number, number};
            } break;

            default: return;
        }

        ignore = true;
        parent->setPickerColor(color);
        ignore = false;

        update_labels(false, true);

    } else if (input == red_input || input == green_input || input == blue_input) {
        std::string value(input->getString());
        auto _num = value.empty() ? 0 : std::stoi(value);
        if (_num > 255) {
            _num = 255;
            input->setString("255");
        }
        GLubyte num = static_cast<GLubyte>(_num);
        auto color = parent->getPickerColor();
        if (input == red_input)
            color.r = num;
        else if (input == green_input)
            color.g = num;
        else if (input == blue_input)
            color.b = num;
        ignore = true;
        parent->setPickerColor(color);
        ignore = false;
        update_labels(true, false);
    }
}

void RGBColorInputWidget::update_labels(bool hex, bool rgb) {
    if (ignore) return;
    ignore = true;
    auto color = parent->getPickerColor();
    if (hex) {
        hex_input->setString(color_to_hex(color).c_str());
    }
    if (rgb) {
        red_input->setString(std::to_string(color.r).c_str());
        green_input->setString(std::to_string(color.g).c_str());
        blue_input->setString(std::to_string(color.b).c_str());
    }
    ignore = false;
}

RGBColorInputWidget* RGBColorInputWidget::create(ColorSelectPopup* parent) {
    auto pRet = new RGBColorInputWidget();

    if (pRet && pRet->init(parent)) {
        pRet->autorelease();
        return pRet;
    }

    CC_SAFE_DELETE(pRet);
    return nullptr;
}
