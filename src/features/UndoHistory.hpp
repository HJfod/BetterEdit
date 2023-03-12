#pragma once

#include "../tracking/Tracking.hpp"

class History : public CCObject {
protected:
    std::string m_initialString;
    std::vector<std::unique_ptr<EditorEvent>> m_events;
    EventListener<EditorFilter> m_listener = { this, &History::onEvent };

    void onEvent(EditorEvent* ev);

public:
    static History* create(GJGameLevel* level);
};
