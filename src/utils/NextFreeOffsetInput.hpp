#pragma once

#include <Geode/binding/GameObject.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/binding/TextInputDelegate.hpp>
#include <Geode/utils/general.hpp>
#include <Geode/ui/TextInput.hpp>
#include <span>

using namespace geode::prelude;

template <class C>
concept NextFreeAllocator = requires(std::span<GameObject*> const& objs, C::ValueType upTo) {
    { C::init(objs, upTo) } -> std::same_as<void>;
    { C::uninit(objs, upTo) } -> std::same_as<void>;
    { C::getObjectCountToAllocate(upTo) } -> std::same_as<size_t>;
    { C::MIN_VALUE } -> std::convertible_to<typename C::ValueType>;
    { C::MAX_VALUE } -> std::convertible_to<typename C::ValueType>;
};

// Nån kanske vill veta varför några kommentarer här är på svenska.
// Det är eftersom jag har en svenskkurs just nu och ville använda denna möjligheten 
// för att öva mig datavetenskaps vokabulär och att tala detta språket allmänt

// Denna här är sådan sak som Linus Torvalds skulle vilja verbalt mörda en man över

template <NextFreeAllocator Allocator>
class FakeObjectsForNextFree final {
private:
    void* m_alloc;
    unsigned int m_oldObjectCount;
    unsigned int m_addObjectCount;
    Allocator::ValueType m_upTo;

public:
    // Oallokerande version
    explicit FakeObjectsForNextFree() : m_alloc(nullptr) {}

    // Allokerande version
    explicit FakeObjectsForNextFree(Allocator::ValueType upTo) {
        // Allokerar nog minne för att lagra falskobjekt
        // Allocator initialiserar denna medlemmarna som krävs för denna 
        // 'Next Free' funktion i GD som falskobjekten har skapats för
        // Alla falskobjekten är allokerad bredvid varandra och pekar 
        // till dom är läggat till slutet av editorns objektlistan

        m_upTo = upTo;
        m_addObjectCount = Allocator::getObjectCountToAllocate(upTo);
        m_alloc = std::malloc(sizeof(GameObject) * m_addObjectCount);

        auto lel = LevelEditorLayer::get();
        m_oldObjectCount = lel->m_objects->data->num;
        ccArrayEnsureExtraCapacity(lel->m_objects->data, m_addObjectCount);
        lel->m_objects->data->num += m_addObjectCount;
        for (size_t i = 0; i < m_addObjectCount; i += 1) {
            lel->m_objects->data->arr[m_oldObjectCount + i] = reinterpret_cast<GameObject*>(
                reinterpret_cast<uintptr_t>(m_alloc) + i * sizeof(GameObject)
            );
        }
        Allocator::init(std::span(
            reinterpret_cast<GameObject**>(lel->m_objects->data->arr) + m_oldObjectCount,
            m_addObjectCount
        ), upTo);
    }

    FakeObjectsForNextFree(FakeObjectsForNextFree&& other) {
        m_alloc = other.m_alloc;
        m_oldObjectCount = other.m_oldObjectCount;
        other.m_alloc = nullptr;
    }

    FakeObjectsForNextFree(FakeObjectsForNextFree const&) = delete;
    FakeObjectsForNextFree& operator=(FakeObjectsForNextFree const&) = delete;
    FakeObjectsForNextFree& operator=(FakeObjectsForNextFree&&) = delete;

    // Rengöring
    ~FakeObjectsForNextFree() {
        if (!m_alloc) return;

        // Om Allocator har också allokerad nånting (t.ex. grupp id behövs att 
        // allokera en egen lista) behövs det anropa uninit för att deallokera 
        // detta minnet
        // Sedan kan man bara dra av tillaga objekt från editorns objektantalet 
        // och deallokera hela allokerad minnet för falskobjekten

        auto lel = LevelEditorLayer::get();
        Allocator::uninit(std::span(
            reinterpret_cast<GameObject**>(lel->m_objects->data->arr) + m_oldObjectCount,
            m_addObjectCount
        ), m_upTo);
        lel->m_objects->data->num = m_oldObjectCount;
        std::free(m_alloc);
    }
};

template <NextFreeAllocator Allocator>
class NextFreeOffsetInput : public CCNode {
protected:
    TextInput* m_input;
    // Bevara värdet mellan öppningar
    static inline typename Allocator::ValueType s_cachedValue = Allocator::MIN_VALUE;

    bool init() {
        if (!CCNode::init())
            return false;
        
        this->setAnchorPoint({ .5f, .5f });
        this->setContentSize({ 40, 30 });

        auto label = CCLabelBMFont::create("Offset", "goldFont.fnt");
        label->setScale(.35f);
        this->addChildAtPosition(label, Anchor::Top, ccp(0, -5));
        
        m_input = TextInput::create(60.f, "Off");
        m_input->setCommonFilter(CommonFilter::Uint);
        m_input->setScale(.5f);
        // Minns värdet
        if (s_cachedValue != Allocator::MIN_VALUE) {
            m_input->setString(numToString(s_cachedValue));
        }
        m_input->setCallback([](auto str) {
            if (auto value = numFromString<typename Allocator::ValueType>(str)) {
                s_cachedValue = clamp(value.unwrap(), Allocator::MIN_VALUE, Allocator::MAX_VALUE);
            }
            else {
                s_cachedValue = Allocator::MIN_VALUE;
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

    static FakeObjectsForNextFree<Allocator> addFakeObjects() {
        // Det krävs inte offsettera om vi räknar upp till det minsta
        if (s_cachedValue == Allocator::MIN_VALUE) {
            return FakeObjectsForNextFree<Allocator>();
        }
        return FakeObjectsForNextFree<Allocator>(s_cachedValue);
    }
};
