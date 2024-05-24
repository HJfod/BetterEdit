#pragma once

#include <Geode/DefaultInclude.hpp>

using namespace geode::prelude;

struct UIShowEvent : public Event {
    EditorUI* ui;
    bool show;

    UIShowEvent(EditorUI* ui, bool show);
};
class UIShowFilter : public EventFilter<UIShowEvent> {
protected:
    EditorUI* m_ui = nullptr;

public:
    using Callback = void(UIShowEvent*);

    UIShowFilter() = default;
    UIShowFilter(EditorUI* ui);

    ListenerResult handle(MiniFunction<Callback> fn, UIShowEvent* ev);
};
using OnUIHide = EventListener<UIShowFilter>;
