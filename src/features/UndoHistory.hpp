#pragma once

#include "../tracking/Tracking.hpp"
#include <Geode/ui/Popup.hpp>
#include <Geode/loader/Event.hpp>

class History;

class HistoryPopup : public Popup<History*> {
protected:
    History* m_history;
    std::vector<CCMenu*> m_items;

    bool setup(History* history);
    void updateState();
    void onUndoTill(CCObject* sender);
    void onRedoTill(CCObject* sender);

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

    EditorEvent* current();

public:
    static History* create(GJGameLevel* level);
    static History* get(LevelEditorLayer* lel);

    std::vector<EditorEvent*> getEvents() const;
    size_t getUndoneCount() const;
    size_t getRedoneCount() const;

    void undo(size_t count = 1);
    void undoTo(EditorEvent* event);
    void redo(size_t count = 1);
    void redoTo(EditorEvent* event);
    bool canUndo() const;
    bool canRedo() const;
};
