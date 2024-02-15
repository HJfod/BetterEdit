#pragma once

#include <Geode/ui/InputNode.hpp>
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

class NextFreeOffsetInput : public CCNode, public TextInputDelegate {
protected:
    InputNode* m_input;

    bool init();

public:
    static NextFreeOffsetInput* create();

    template <NextFreeAllocator Allocator>
    Allocator::ValueType getValue() const {
        if (auto value = numFromString<typename Allocator::ValueType>(m_input->getInput()->getString())) {
            return clamp(value.unwrap(), Allocator::MIN_VALUE, Allocator::MAX_VALUE);
        }
        else {
            return Allocator::MIN_VALUE;
        }
    }
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
FakeObjectsForNextFree<Allocator> addFakeObjects(NextFreeOffsetInput* input) {
    // Det krävs inte offsettera om vi har ingen input
    if (!input) {
        return FakeObjectsForNextFree<Allocator>();
    }
    auto upTo = input->template getValue<Allocator>();
    // Det krävs inte offsettera om vi räknar upp till det minsta
    if (upTo == Allocator::MIN_VALUE) {
        return FakeObjectsForNextFree<Allocator>();
    }
    return FakeObjectsForNextFree<Allocator>(upTo);
}
