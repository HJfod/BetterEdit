#include <Geode/modify/ConfigureHSVWidget.hpp>
#include <Geode/modify/HSVWidgetPopup.hpp>
#include <Geode/modify/CustomizeObjectLayer.hpp>
#include <Geode/modify/ColorSelectPopup.hpp>

using namespace geode::prelude;

static ccColor3B getRealizedColor(int channelID, size_t depth = 0) {
    // Just in case there's a circular color channel dependency
    if (depth > 10) {
        return ccWHITE;
    }
    auto channel = LevelEditorLayer::get()->m_levelSettings->m_effectManager->getColorAction(channelID);
    if (!channel) {
        return ccWHITE;
    }
    if (channel->m_copyID) {
        return GameToolbox::transformColor(getRealizedColor(channel->m_copyID, depth + 1), channel->m_copyHSV);
    }
    return channel->m_fromColor;
}

class $modify(HSVWidgetWithPreview, ConfigureHSVWidget) {
    struct Fields {
        struct ColorPreview final {
            CCSprite* origColor;
            CCSprite* destColor;
            // function so it can be automatically resourced if it changes
            std::function<int()> srcChannel;
        };
        std::optional<ColorPreview> preview;
    };

    $override
    void updateLabels() {
        ConfigureHSVWidget::updateLabels();
        if (m_fields->preview) {
            auto prev = *m_fields->preview;
            auto color = getRealizedColor(prev.srcChannel());
            prev.origColor->setColor(color);
            prev.destColor->setColor(GameToolbox::transformColor(color, m_hsv));
        }
    }

    void setSourceChannel(std::function<int()> channel) {
        auto orig = static_cast<CCSprite*>(this->getChildByID("original-color"_spr));
        if (!orig) {
            orig = CCSprite::createWithSpriteFrameName("whiteSquare60_001.png");
            orig->setID("original-color"_spr);
            orig->setPosition(ccp(-120, 15 * .6f)); // no ids or layouts,.....
            orig->setScale(.6f);
            this->addChild(orig);
        }
        auto dest = static_cast<CCSprite*>(this->getChildByID("destination-color"_spr));
        if (!dest) {
            dest = CCSprite::createWithSpriteFrameName("whiteSquare60_001.png");
            dest->setID("destination-color"_spr);
            dest->setPosition(ccp(-120, -15 * .6f));
            dest->setScale(.6f);
            this->addChild(dest);
        }
        m_fields->preview.emplace(Fields::ColorPreview {
            .origColor = orig,
            .destColor = dest,
            .srcChannel = channel,
        });
        this->updateLabels();
    }
};

static std::optional<int> HSV_SOURCE_COLOR;
class $modify(CustomizeObjectLayer) {
    $override
    void onHSV(CCObject* sender) {
        HSV_SOURCE_COLOR = this->getActiveMode(true);
        CustomizeObjectLayer::onHSV(sender);
    }
};
class $modify(HSVWidgetPopup) {
    $override
    bool init(ccHSVValue hsv, HSVWidgetDelegate* delegate, gd::string p2) {
        if (!HSVWidgetPopup::init(hsv, delegate, p2))
            return false;

        if (HSV_SOURCE_COLOR && m_widget) {
            static_cast<HSVWidgetWithPreview*>(m_widget)->setSourceChannel([c = *HSV_SOURCE_COLOR] {
                return c;
            });
            HSV_SOURCE_COLOR = std::nullopt;
        }
        
        return true;
    }
};
class $modify(ColorSelectPopup) {
    $override
    bool init(EffectGameObject* obj, CCArray* objs, ColorAction* action) {
        if (!ColorSelectPopup::init(obj, objs, action))
            return false;
        
        if (m_hsvWidget) {
            if (obj) {
                static_cast<HSVWidgetWithPreview*>(m_hsvWidget)->setSourceChannel([obj] {
                    return obj->m_copyColorID;
                });
            }
            else if (action) {
                static_cast<HSVWidgetWithPreview*>(m_hsvWidget)->setSourceChannel([action] {
                    return action->m_colorID;
                });
            }
        }

        return true;
    }

    $override
    void onUpdateCopyColor(CCObject* sender) {
        ColorSelectPopup::onUpdateCopyColor(sender);
        if (m_hsvWidget) {
            static_cast<HSVWidgetWithPreview*>(m_hsvWidget)->updateLabels();
        }
    }
};
