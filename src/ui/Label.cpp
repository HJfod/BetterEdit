#include "Label.hpp"
#include <Geode/utils/string.hpp>

bool Label::init(std::string const& text, const char* font, CCSize const& size) {
    if (!CCNodeRGBA::init())
        return false;

    m_font = font;
    m_text = text;
    m_size = size;

    this->updateContent();

    return true;
}

void Label::updateContent() {
    this->setContentSize(m_size);

    float scale = m_maxScale;
    float scaleDown = m_maxScale;
    float height = 0.f;
    CCLabelBMFont* label;

    auto updateRatio = [&]() -> bool {
        if (label->getScaledContentSize().width > m_size.width) {
            auto ratio = m_size.width / label->getScaledContentSize().width;
            if (ratio < scaleDown) {
                scaleDown = ratio;
            }
            return true;
        }
        return false;
    };

try_fit:
    this->removeAllChildren();
    height = 0.f;

    label = CCLabelBMFont::create("", m_font.c_str());
    label->setScale(scale);
    this->addChild(label);

    for (auto& word : string::split(m_text, " ")) {
        auto old = std::string(label->getString());
        if (old.size()) {
            label->setString((old + " " + word).c_str());
        }
        else {
            label->setString(word.c_str());
        }
        updateRatio();
        if (label->getScaledContentSize().width > m_size.width) {
            label->setString(old.c_str());
            if (old.empty()) {
                label->removeFromParent();
            }
            height += label->getScaledContentSize().height;

            label = CCLabelBMFont::create(word.c_str(), m_font.c_str());
            label->setScale(scale);
            this->addChild(label);

            if (updateRatio() && scaleDown != scale && scaleDown >= .1f) {
                scale = scaleDown;
                goto try_fit;
            }
        }
    }
    height += label->getScaledContentSize().height;

    if (height > m_size.height) {
        auto ratio = m_size.height / height * scale;
        if (ratio < scaleDown) {
            scaleDown = ratio;
        }
        if (scaleDown != scale && scaleDown >= .1f) {
            scale = scaleDown;
            goto try_fit;
        }
    }

    this->setContentSize({ m_size.width, height });

    float y = 0.f;
    for (auto& node : CCArrayExt<CCLabelBMFont>(m_pChildren)) {
        node->setAnchorPoint({ .0f, 1.f });
        node->setPosition(0.f, height - y);
        y += node->getScaledContentSize().height;
    }
}

void Label::setMaxScale(float scale) {
    m_maxScale = scale;
}

float Label::getMaxScale() const {
    return m_maxScale;
}

void Label::setAlignment(AxisAlignment align) {
    m_hAlign = align;
    this->updateContent();
}

AxisAlignment Label::getAlignment() const {
    return m_hAlign;
}

void Label::setFont(const char* font) {
    m_font = font;
    this->updateContent();
}

std::string Label::getFont() const {
    return m_font;
}

void Label::setString(const char* label) {
    m_text = label;
    this->updateContent();
}

const char* Label::getString() {
    return m_text.c_str();
}

Label* Label::create(std::string const& text, const char* font, CCSize const& size) {
    auto ret = new Label();
    if (ret && ret->init(text, font, size)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}
