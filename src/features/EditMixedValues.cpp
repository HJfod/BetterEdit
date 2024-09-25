#include <Geode/modify/SetGroupIDLayer.hpp>
#include <Geode/utils/cocos.hpp>

using namespace geode::prelude;

template <class T>
struct ValueLimits final {
    T min;
    T max;
    constexpr ValueLimits(T value) : min(value), max(value) {}
    constexpr ValueLimits(T min, T max) : min(min), max(max) {}
    static constexpr ValueLimits zeroToInf() {
        return ValueLimits(0, std::numeric_limits<T>::max());
    }
};

template <class T>
class MixedValuesHandler final {
protected:
    std::vector<typename T::GameObjectType*> m_targets;
    typename T::Type m_mixedSource;
    CCTextInputNode* m_input;

public:
    void setup(GameObject* obj, CCArray* objs, CCTextInputNode* input) {
        m_input = input;
        m_targets.clear();
        if (obj) {
            if constexpr (std::is_same_v<typename T::GameObjectType, GameObject>) {
                m_targets.push_back(obj);
            }
            else {
                if (auto o = typeinfo_cast<typename T::GameObjectType*>(obj)) {
                    m_targets.push_back(o);
                }
            }
        }
        for (auto o : CCArrayExt<GameObject*>(objs)) {
            if constexpr (std::is_same_v<typename T::GameObjectType, GameObject>) {
                m_targets.push_back(o);
            }
            else {
                if (auto e = typeinfo_cast<typename T::GameObjectType*>(o)) {
                    m_targets.push_back(e);
                }
            }
        }
        if (!m_targets.empty()) {
            m_mixedSource = T::get(m_targets.front());
        }
        // Disable input until explicitly unmixed via button
        // This is because A) i'm too lazy to parse "Mix+N" strings and 
        // B) this makes it clear how to unmix
        auto unmixSpr = ButtonSprite::create("Unmix", "goldFont.fnt", "GJ_button_05.png", .8f);
        unmixSpr->setScale(.3f);
        auto unmixBtn = CCMenuItemExt::createSpriteExtra(
            unmixSpr, [this](auto) {
                auto limits = this->getMinMax();
                this->override(limits.min + (limits.max - limits.min) / 2);
            }
        );
        auto menu = CCMenu::create();
        menu->setID("unmix-menu"_spr);
        menu->ignoreAnchorPointForPosition(false);
        menu->setContentSize({ 25, 15 });
        menu->addChildAtPosition(unmixBtn, Anchor::Center);
        input->getParent()->addChildAtPosition(menu, Anchor::Bottom, ccp(0, 0), false);

        this->updateLabel();
    }
    bool isMixed() const {
        if (m_targets.empty()) {
            return false;
        }
        auto value = T::get(m_targets.front());
        for (auto obj : m_targets) {
            if (T::get(obj) != value) {
                return true;
            }
        }
        return false;
    }
    void override(typename T::Type value) const {
        for (auto obj : m_targets) {
            T::set(obj, clamp(value, T::LIMITS.min, T::LIMITS.max));
        }
        this->updateLabel();
    }
    void parse(std::string const& value) const {
        this->override(numFromString<typename T::Type>(value).unwrapOr(0));
    }
    void increment(typename T::Type value) const {
        for (auto obj : m_targets) {
            auto val = clamp(T::get(obj) + value, T::LIMITS.min, T::LIMITS.max);
            if (T::shouldSkipZero(obj) && val == 0) {
                val = value > 0 ? 1 : -1;
            }
            T::set(obj, val);
        }
        this->updateLabel();
    }
    void nextFree() const {
        std::set<typename T::Type> usedLayers;
        for (auto obj : m_targets) {
            usedLayers.insert(T::get(obj));
        }
        typename T::Type nextFree;
        for (nextFree = T::LIMITS.min; nextFree < T::LIMITS.max; nextFree += 1) {
            if (!usedLayers.contains(nextFree)) {
                break;
            }
        }
        this->override(nextFree);
    }
    void updateLabel() const {
        if (m_targets.empty()) return;
        if (this->isMixed()) {
            auto limits = this->getMinMax();
            m_input->setMouseEnabled(false);
            m_input->setTouchEnabled(false);
            m_input->detachWithIME();
            m_input->getParent()->getChildByID("unmix-menu"_spr)->setVisible(true);
            m_input->setString(fmt::format("{}..{}", limits.min, limits.max));
        }
        else {
            m_input->setMouseEnabled(true);
            m_input->setTouchEnabled(true);
            m_input->getParent()->getChildByID("unmix-menu"_spr)->setVisible(false);
            m_input->setString(fmt::format("{}", T::get(m_targets.front())));
        }
    }
    ValueLimits<typename T::Type> getMinMax() const {
        auto limits = ValueLimits(T::get(m_targets.front()));
        for (auto target : m_targets) {
            auto value = T::get(target);
            if (value < limits.min) {
                limits.min = value;
            }
            if (value > limits.max) {
                limits.max = value;
            }
        }
        return limits;
    }
};

struct MixedPropEditorLayer final {
    using Type = short;
    using GameObjectType = GameObject;
    static constexpr Type MIXED_VALUE = -1;
    static constexpr ValueLimits<Type> LIMITS = ValueLimits<Type>::zeroToInf();
    static bool shouldSkipZero(GameObjectType*) {
        return false;
    }
    static Type get(GameObjectType* obj) {
        return obj->m_editorLayer;
    }
    static void set(GameObjectType* obj, Type value) {
        obj->m_editorLayer = value;
    }
};
struct MixedPropEditorLayer2 final {
    using Type = short;
    using GameObjectType = GameObject;
    static constexpr Type MIXED_VALUE = -1;
    static constexpr ValueLimits<Type> LIMITS = ValueLimits<Type>::zeroToInf();
    static bool shouldSkipZero(GameObjectType*) {
        return false;
    }
    static Type get(GameObjectType* obj) {
        return obj->m_editorLayer2;
    }
    static void set(GameObjectType* obj, Type value) {
        obj->m_editorLayer2 = value;
    }
};
struct MixedPropZOrder final {
    using Type = int;
    using GameObjectType = GameObject;
    static constexpr Type MIXED_VALUE = -1000;
    static constexpr ValueLimits<Type> LIMITS = { -999, 999 };
    static bool shouldSkipZero(GameObjectType* obj) {
        return obj->m_defaultZOrder != 0;
    }
    static Type get(GameObjectType* obj) {
        return obj->m_zOrder == 0 ? obj->m_defaultZOrder : obj->m_zOrder;
    }
    static void set(GameObjectType* obj, Type value) {
        obj->m_zOrder = value;
    }
};
struct MixedPropOrder final {
    using Type = int;
    using GameObjectType = EffectGameObject;
    static constexpr Type MIXED_VALUE = -1;
    static constexpr ValueLimits<Type> LIMITS = ValueLimits<Type>::zeroToInf();
    static bool shouldSkipZero(GameObjectType*) {
        return false;
    }
    static Type get(GameObjectType* obj) {
        return obj->m_ordValue;
    }
    static void set(GameObjectType* obj, Type value) {
        obj->m_ordValue = value;
    }
};
struct MixedPropChannel final {
    using Type = int;
    using GameObjectType = EffectGameObject;
    static constexpr Type MIXED_VALUE = -1;
    static constexpr ValueLimits<Type> LIMITS = ValueLimits<Type>::zeroToInf();
    static bool shouldSkipZero(GameObjectType*) {
        return false;
    }
    static Type get(GameObjectType* obj) {
        return obj->m_channelValue;
    }
    static void set(GameObjectType* obj, Type value) {
        obj->m_channelValue = value;
    }
};

class $modify(SetGroupIDLayer) {
    struct Fields {
        MixedValuesHandler<MixedPropEditorLayer> editorLayerHandler;
        MixedValuesHandler<MixedPropEditorLayer2> editorLayer2Handler;
        MixedValuesHandler<MixedPropZOrder> zOrderHandler;
        MixedValuesHandler<MixedPropOrder> channelOrderHandler;
        MixedValuesHandler<MixedPropChannel> channelHandler;
    };

    $override
    bool init(GameObject* obj, CCArray* objs) {
        if (!SetGroupIDLayer::init(obj, objs))
            return false;
        
        m_fields->editorLayerHandler.setup(obj, objs, m_editorLayerInput);
        m_fields->editorLayer2Handler.setup(obj, objs, m_editorLayer2Input);
        m_fields->zOrderHandler.setup(obj, objs, m_zOrderInput);
        if (m_orderInput) m_fields->channelOrderHandler.setup(obj, objs, m_orderInput);
        if (m_channelInput) m_fields->channelHandler.setup(obj, objs, m_channelInput);

        return true;
    }

    $override
    void onArrow(int tag, int increment) {
        if (tag == m_editorLayerInput->getTag()) {
            m_fields->editorLayerHandler.increment(increment);
        }
        else if (tag == m_editorLayer2Input->getTag()) {
            m_fields->editorLayer2Handler.increment(increment);
        }
        else if (tag == m_zOrderInput->getTag()) {
            m_fields->zOrderHandler.increment(increment);
        }
        else if (m_orderInput && tag == m_orderInput->getTag()) {
            m_fields->channelOrderHandler.increment(increment);
        }
        else if (m_channelInput && tag == m_channelInput->getTag()) {
            m_fields->channelHandler.increment(increment);
        }
        else {
            SetGroupIDLayer::onArrow(tag, increment);
        }
    }

    $override
    void onNextFreeEditorLayer1(CCObject* sender) {
        if (0) SetGroupIDLayer::onNextFreeEditorLayer1(sender);
        m_fields->editorLayerHandler.nextFree();
    }
    $override
    void onNextFreeEditorLayer2(CCObject* sender) {
        if (0) SetGroupIDLayer::onNextFreeEditorLayer2(sender);
        m_fields->editorLayerHandler.nextFree();
    }
    $override
    void onNextFreeOrderChannel(CCObject* sender) {
        if (0) SetGroupIDLayer::onNextFreeOrderChannel(sender);
        m_fields->channelOrderHandler.nextFree();
    }

    $override
    void updateEditorLayerID() {
        if (0) SetGroupIDLayer::updateEditorLayerID();
    }
    $override
    void updateEditorLabel() {
        if (0) SetGroupIDLayer::updateEditorLabel();
    }
    $override
    void updateEditorLayerID2() {
        if (0) SetGroupIDLayer::updateEditorLayerID2();
    }
    $override
    void updateEditorLabel2() {
        if (0) SetGroupIDLayer::updateEditorLabel2();
    }
    $override
    void updateZOrder() {
        if (0) SetGroupIDLayer::updateZOrder();
    }
    $override
    void updateZOrderLabel() {
        if (0) SetGroupIDLayer::updateZOrderLabel();
    }
    $override
    void updateOrderChannel() {
        if (0) SetGroupIDLayer::updateOrderChannel();
    }
    $override
    void updateOrderChannelLabel() {
        if (0) SetGroupIDLayer::updateOrderChannelLabel();
    }
    $override
    void updateEditorOrder() {
        if (0) SetGroupIDLayer::updateEditorOrder();
    }
    $override
    void updateEditorOrderLabel() {
        if (0) SetGroupIDLayer::updateEditorOrderLabel();
    }

    $override
    virtual void textChanged(CCTextInputNode* input) {
        // Check if the input is enabled
        if (!input->isTouchEnabled()) {
            return;
        }
        if (input == m_editorLayerInput) {
            m_fields->editorLayerHandler.parse(input->getString());
        }
        else if (input == m_editorLayer2Input) {
            m_fields->editorLayer2Handler.parse(input->getString());
        }
        else if (input == m_zOrderInput) {
            m_fields->zOrderHandler.parse(input->getString());
        }
        else if (input == m_orderInput) {
            m_fields->channelOrderHandler.parse(input->getString());
        }
        else if (input == m_channelInput) {
            m_fields->channelHandler.parse(input->getString());
        }
        else {
            SetGroupIDLayer::textChanged(input);
        }
    }
};
