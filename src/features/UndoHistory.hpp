#pragma once

#include "../tracking/Tracking.hpp"
#include <Geode/ui/Popup.hpp>

class History;

class HistoryPopup : public Popup<History*> {
protected:
    bool setup(History* history);

public:
    static HistoryPopup* create(History* history);
};

class History : public CCObject {
protected:
    std::string m_initialString;
    std::vector<std::unique_ptr<EditorEvent>> m_events;
    size_t m_undone = 0;
    EventListener<EditorFilter> m_listener = { this, &History::onEvent };

    void onEvent(EditorEvent* ev);

public:
    static History* create(GJGameLevel* level);
    static History* get(LevelEditorLayer* lel);

    std::vector<EditorEvent*> getEvents() const;
    size_t getUndoneCount() const;
    size_t getRedoneCount() const;

    void undo();
    void redo();
    bool canUndo() const;
    bool canRedo() const;
};
