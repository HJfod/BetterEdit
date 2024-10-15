#include <Geode/modify/SetGroupIDLayer.hpp>
#include <Geode/modify/SetupTriggerPopup.hpp>
#include <Geode/modify/SetupRotateCommandPopup.hpp>
#include <Geode/utils/cocos.hpp>

using namespace geode::prelude;

template <class T>
struct ValueLimits final {
    T min;
    T max;
    constexpr ValueLimits() : min(std::numeric_limits<T>::min()), max(std::numeric_limits<T>::max()) {}
    constexpr ValueLimits(T value) : min(value), max(value) {}
    constexpr ValueLimits(T min, T max) : min(min), max(max) {}
    static constexpr ValueLimits zeroToInf() {
        return ValueLimits(0, std::numeric_limits<T>::max());
    }
};

template <class T>
struct MixedValuesConfig final {
    using Type = T;
    ValueLimits<Type> limits;
    bool (*isTarget)(GameObject*) = +[](GameObject*) {
        return true;
    };
    Type (*getDefault)(GameObject*);
    Type (*get)(GameObject*);
    void (*set)(GameObject*, Type);
    bool (*shouldSkipZero)(GameObject*) = +[](GameObject*) {
        return false;
    };
};

template <class T>
class MixedValuesInput : public CCMenu {
protected:
    MixedValuesConfig<T> m_config;
    std::vector<GameObject*> m_targets;
    TextInput* m_input;
    CCLabelBMFont* m_title = nullptr;
    CCMenuItemSpriteExtra* m_arrowLeftBtn;
    CCMenuItemSpriteExtra* m_arrowRightBtn;
    CCMenuItemSpriteExtra* m_unmixBtn;
    CCMenuItemSpriteExtra* m_nextFreeBtn = nullptr;

protected:
    bool init(
        GameObject* obj, CCArray* objs, MixedValuesConfig<T> const& config,
        const char* title, const char* arrowSpr, bool showNextFree
    ) {
        if (!CCMenu::init())
            return false;
        
        m_config = config;

        if (obj) {
            if (m_config.isTarget(obj)) {
                m_targets.push_back(obj);
            }
        }
        for (auto o : CCArrayExt<GameObject*>(objs)) {
            if (m_config.isTarget(o)) {
                m_targets.push_back(o);
            }
        }
        
        this->ignoreAnchorPointForPosition(false);
        this->setContentSize(ccp(120, 60));
        this->setAnchorPoint(ccp(.5f, .5f));

        if (title) {
            m_title = CCLabelBMFont::create(title, "goldFont.fnt");
            m_title->setScale(.65f);
            this->addChildAtPosition(m_title, Anchor::Top, ccp(0, -10));
        }

        m_input = TextInput::create(50, "Num");
        m_input->setCallback([this](std::string const& text) {
            this->override(numFromString<T>(text).unwrapOr(0));
        });
        m_input->setCommonFilter(CommonFilter::Int);
        this->addChildAtPosition(m_input, Anchor::Center, ccp(0, -10));

        auto arrowLeftSpr = CCSprite::createWithSpriteFrameName(arrowSpr);
        arrowLeftSpr->setScale(.7f);
        m_arrowLeftBtn = CCMenuItemSpriteExtra::create(
            arrowLeftSpr, this, menu_selector(MixedValuesInput::onArrow)
        );
        m_arrowLeftBtn->setTag(-1);
        m_arrowLeftBtn->setID("arrow-left-button"_spr);
        this->addChildAtPosition(m_arrowLeftBtn, Anchor::Left, ccp(15, -10));

        auto arrowRightSpr = CCSprite::createWithSpriteFrameName(arrowSpr);
        arrowRightSpr->setScale(.7f);
        arrowRightSpr->setFlipX(true);
        m_arrowRightBtn = CCMenuItemSpriteExtra::create(
            arrowRightSpr, this, menu_selector(MixedValuesInput::onArrow)
        );
        m_arrowRightBtn->setTag(1);
        m_arrowRightBtn->setID("arrow-right-button"_spr);
        this->addChildAtPosition(m_arrowRightBtn, Anchor::Right, ccp(-15, -10));

        if (showNextFree) {
            auto nextFreeSpr = CCSprite::createWithSpriteFrameName("GJ_plus2Btn_001.png");
            nextFreeSpr->setScale(.8f);
            m_nextFreeBtn = CCMenuItemSpriteExtra::create(
                nextFreeSpr, this, menu_selector(MixedValuesInput::onNextFree)
            );
            m_nextFreeBtn->setTag(1);
            m_nextFreeBtn->setID("next-free-button"_spr);
            this->addChildAtPosition(m_nextFreeBtn, Anchor::TopRight, ccp(-6, -10));
        }
        
        auto unmixSpr = ButtonSprite::create("Unmix", "goldFont.fnt", "GJ_button_05.png", .8f);
        unmixSpr->setScale(.3f);
        m_unmixBtn = CCMenuItemExt::createSpriteExtra(
            unmixSpr, [this](auto) {
                auto limits = this->getMinMax();
                this->override(limits.min + (limits.max - limits.min) / 2);
            }
        );
        m_unmixBtn->setID("unmix-button"_spr);
        this->addChildAtPosition(m_unmixBtn, Anchor::Bottom, ccp(0, 0));

        this->updateLabel();

        return true;
    }

    void onArrow(CCObject* sender) {
        for (auto obj : m_targets) {
            auto val = clamp(
                m_config.get(obj) + sender->getTag(),
                m_config.limits.min, m_config.limits.max
            );
            if (m_config.shouldSkipZero(obj) && val == 0) {
                val = sender->getTag() > 0 ? 1 : -1;
            }
            m_config.set(obj, val);
        }
        this->updateLabel();
    }
    void onNextFree(CCObject*) {
        std::set<T> usedLayers;
        for (auto obj : m_targets) {
            usedLayers.insert(m_config.get(obj));
        }
        T nextFree;
        for (nextFree = m_config.limits.min; nextFree < m_config.limits.max; nextFree += 1) {
            if (!usedLayers.contains(nextFree)) {
                break;
            }
        }
        this->override(nextFree);
    }

    bool isMixed() const {
        if (m_targets.empty()) {
            return false;
        }
        auto value = m_config.get(m_targets.front());
        for (auto obj : m_targets) {
            if (m_config.get(obj) != value) {
                return true;
            }
        }
        return false;
    }
    void override(T value) {
        for (auto obj : m_targets) {
            m_config.set(obj, clamp(value, m_config.limits.min, m_config.limits.max));
        }
        this->updateLabel();
    }

    ValueLimits<T> getMinMax() const {
        auto limits = ValueLimits(m_config.get(m_targets.front()));
        for (auto target : m_targets) {
            auto value = m_config.get(target);
            limits.min = std::min(value, limits.min);
            limits.max = std::max(value, limits.max);
        }
        return limits;
    }

public:
    static MixedValuesInput* create(
        GameObject* obj, CCArray* objs,
        MixedValuesConfig<T> const& config,
        const char* title, const char* arrowSpr, bool showNextFree
    ) {
        auto ret = new MixedValuesInput();
        if (ret && ret->init(obj, objs, config, title, arrowSpr, showNextFree)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }

    void replace(CCTextInputNode*& inputMember, CCNode* menu) {
        if (!menu) return;
        inputMember = m_input->getInputNode();
        
        this->setPosition(menu->getPosition());
        menu->getParent()->addChild(this);

        this->setID(menu->getID());
        // Try to match all of the menu's existing stuff to the new menu
        for (auto child : CCArrayExt<CCNode*>(menu->getChildren())) {
            // First check using ID to avoid type casting overhead
            auto id = child->getID();
            if (string::contains(id, "next-free")) {
                if (m_nextFreeBtn) m_nextFreeBtn->setID(id);
                continue;
            }
            if (string::contains(id, "prev-button")) {
                m_arrowLeftBtn->setID(id);
                continue;
            }
            if (string::contains(id, "next-button")) {
                m_arrowRightBtn->setID(id);
                continue;
            }
            if (string::contains(id, "input")) {
                m_input->setID(id);
                continue;
            }
            if (string::contains(id, "label")) {
                if (m_title) m_title->setID(id);
                continue;
            }
        }

        menu->removeFromParent();
    }

    void updateLabel() {
        if (m_targets.empty()) return;
        if (this->isMixed()) {
            auto minmax = this->getMinMax();
            m_input->setEnabled(false);
            m_input->getInputNode()->m_placeholderLabel->setOpacity(255);
            m_input->defocus();
            m_unmixBtn->setVisible(true);
            m_input->setString(fmt::format("{}..{}", minmax.min, minmax.max));
        }
        else {
            m_input->setEnabled(true);
            m_unmixBtn->setVisible(false);
            m_input->setString(fmt::format("{}", m_config.get(m_targets.front())));
        }
    }
};

class $modify(SetGroupIDLayer) {
    $override
    bool init(GameObject* obj, CCArray* objs) {
        if (!SetGroupIDLayer::init(obj, objs))
            return false;
        
        MixedValuesInput<short>::create(
            obj, objs, MixedValuesConfig<short> {
                .limits = ValueLimits<short>::zeroToInf(),
                .getDefault = +[](GameObject*) {
                    return short(0);
                },
                .get = +[](GameObject* obj) {
                    return obj->m_editorLayer;
                },
                .set = +[](GameObject* obj, short value) {
                    obj->m_editorLayer = value;
                },
            },
            "Editor L", "GJ_arrow_02_001.png", true
        )->replace(m_editorLayerInput, m_mainLayer->querySelector("editor-layer-menu"));
        
        MixedValuesInput<short>::create(
            obj, objs, MixedValuesConfig<short> {
                .limits = ValueLimits<short>::zeroToInf(),
                .getDefault = +[](GameObject*) {
                    return short(0);
                },
                .get = +[](GameObject* obj) {
                    return obj->m_editorLayer2;
                },
                .set = +[](GameObject* obj, short value) {
                    obj->m_editorLayer2 = value;
                },
            },
            "Editor L2", "GJ_arrow_03_001.png", true
        )->replace(m_editorLayer2Input, m_mainLayer->querySelector("editor-layer-2-menu"));
        
        MixedValuesInput<int>::create(
            obj, objs, MixedValuesConfig<int> {
                .limits = ValueLimits<int>(-999, 999),
                .getDefault = +[](GameObject* obj) {
                    return obj->m_defaultZOrder;
                },
                .get = +[](GameObject* obj) {
                    return obj->m_zOrder == 0 ? obj->m_defaultZOrder : obj->m_zOrder;
                },
                .set = +[](GameObject* obj, int value) {
                    obj->m_zOrder = value;
                },
                .shouldSkipZero = +[](GameObject* obj) {
                    return obj->m_defaultZOrder != 0;
                },
            },
            "Z Order", "GJ_arrow_02_001.png", false
        )->replace(m_zOrderInput, m_mainLayer->querySelector("z-order-menu"));

        if (m_orderInput) {
            MixedValuesInput<int>::create(
                obj, objs, MixedValuesConfig<int> {
                    .limits = ValueLimits<int>::zeroToInf(),
                    .isTarget = +[](GameObject* obj) {
                        return typeinfo_cast<EffectGameObject*>(obj) != nullptr;
                    },
                    .getDefault = +[](GameObject*) {
                        return 0;
                    },
                    .get = +[](GameObject* obj) {
                        return static_cast<EffectGameObject*>(obj)->m_ordValue;
                    },
                    .set = +[](GameObject* obj, int value) {
                        static_cast<EffectGameObject*>(obj)->m_ordValue = value;
                    }
                },
                nullptr, "GJ_arrow_02_001.png", false
            )->replace(m_orderInput, m_mainLayer->querySelector("channel-order-menu"));
        }
        if (m_channelInput) {
            MixedValuesInput<int>::create(
                obj, objs, MixedValuesConfig<int> {
                    .limits = ValueLimits<int>::zeroToInf(),
                    .isTarget = +[](GameObject* obj) {
                        return typeinfo_cast<EffectGameObject*>(obj) != nullptr;
                    },
                    .getDefault = +[](GameObject*) {
                        return 0;
                    },
                    .get = +[](GameObject* obj) {
                        return static_cast<EffectGameObject*>(obj)->m_channelValue;
                    },
                    .set = +[](GameObject* obj, int value) {
                        static_cast<EffectGameObject*>(obj)->m_channelValue = value;
                    }
                },
                nullptr, "GJ_arrow_02_001.png", false
            )->replace(m_channelInput, m_mainLayer->querySelector("channel-menu"));
        }

        return true;
    }
};

// class $modify(SetupTriggerPopup) {
//     $override
//     void createValueControlAdvanced(
//         int property, gd::string title, CCPoint position, float scale,
//         bool disableSlider, InputValueType type, int charCountLimit, bool enableArrows,
//         float minValue, float maxValue,
//         int page, int group,
//         GJInputStyle style,
//         int precision, bool enableTrashCan
//     ) {
//         // SetupTriggerPopup::createValueControlAdvanced(
//         //     property, title, position, scale, disableSlider, type, charCountLimit, enableArrows,
//         //     minValue, maxValue, page, group, style, precision, enableTrashCan
//         // );
//     }
// };

// class $modify(SetupRotateCommandPopup) {
//     $override
//     bool init(EffectGameObject* obj, CCArray* objs) {
//         if (!SetupRotateCommandPopup::init(obj, objs))
//             return false;
        
//         auto test = CCLabelBMFont::create("balls", "bigFont.fnt");
//         m_mainLayer->addChildAtPosition(test, Anchor::Center, ccp(0, 0), false);

//         this->getGroupContainer(0x2329)->addObject(test);

//         return true;
//     }
// };
