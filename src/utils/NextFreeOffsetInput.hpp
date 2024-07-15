#pragma once

#include <Geode/binding/GameObject.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/binding/TextInputDelegate.hpp>
#include <Geode/utils/general.hpp>
#include <Geode/utils/cocos.hpp>
#include <Geode/ui/TextInput.hpp>
#include <span>

using namespace geode::prelude;

template <class C>
concept NextFreeSource = requires {
    typename C::ValueType;
    { C::getUsedIDs(std::declval<GameObject*>(), std::declval<std::unordered_set<typename C::ValueType>&>()) } -> std::same_as<void>;
    { C::MIN_VALUE } -> std::convertible_to<typename C::ValueType>;
    { C::MAX_VALUE } -> std::convertible_to<typename C::ValueType>;
};

// Nån kanske vill veta varför några kommentarer här är på svenska.
// Det är eftersom jag har en svenskkurs just nu och ville använda denna möjligheten 
// för att öva mig datavetenskaps vokabulär och att tala detta språket allmänt

template <NextFreeSource Source>
class NextFreeOffsetInput : public CCNode {
public:
    using ValueType = typename Source::ValueType;

protected:
    TextInput* m_input;
    // Bevara värdet mellan öppningar
    static inline ValueType s_value = Source::MIN_VALUE;

    bool init() {
        if (!CCNode::init())
            return false;
        
        this->setAnchorPoint({ .5f, .5f });
        this->setContentSize({ 40, 30 });

        auto label = CCLabelBMFont::create("Offset", "goldFont.fnt");
        label->setScale(.35f);
        this->addChildAtPosition(label, Anchor::Top, ccp(0, -5));
        
        m_input = TextInput::create(60.f, "Num");
        m_input->setCommonFilter(CommonFilter::Uint);
        m_input->setScale(.5f);
        // Minns värdet
        if (s_value != Source::MIN_VALUE) {
            m_input->setString(numToString(s_value));
        }
        m_input->setCallback([](auto str) {
            log::info("hi: {}", str);
            if (auto value = numFromString<ValueType>(str)) {
                s_value = clamp(value.unwrap(), Source::MIN_VALUE, Source::MAX_VALUE);
            }
            else {
                s_value = Source::MIN_VALUE;
            }
        });
        this->addChildAtPosition(m_input, Anchor::Bottom, ccp(0, 10));

        return true;
    }

public:
    static NextFreeOffsetInput* create() {
        auto ret = new NextFreeOffsetInput();
        if (ret && ret->init()) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }

    static ValueType getNextFreeID() {
        std::unordered_set<ValueType> used;
        for (auto obj : CCArrayExt<GameObject*>(LevelEditorLayer::get()->m_objects)) {
            Source::getUsedIDs(obj, used);
        }
        auto next = s_value;
        while (next != Source::MAX_VALUE && used.contains(next)) {
            next += 1;
        }
        return next;
    }
};
