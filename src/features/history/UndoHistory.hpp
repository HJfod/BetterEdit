#pragma once

// todo: merge history, recovery, and version control

#include <tracking/Tracking.hpp>
#include <Geode/ui/Popup.hpp>
#include <Geode/ui/ScrollLayer.hpp>
#include <Geode/loader/Event.hpp>
#include <ui/Clip.hpp>

using namespace geode::prelude;
using namespace better_edit;

class History;
class HistoryPopup;

class HistoryNode : public CCNode {
protected:
    HistoryPopup* m_popup;
    EditorEvent* m_event;
    CCLabelBMFont* m_name;
    CCMenu* m_topMenu;
    CCMenu* m_bottomMenu;
    Clip* m_bottomClip;
    CCMenuItemSpriteExtra* m_undoBtn;
    CCMenuItemSpriteExtra* m_detailsBtn = nullptr;
    float m_detailsOpen = 0.f;

    bool init(HistoryPopup* popup, EditorEvent* event, float width);

    void onUndoTill(CCObject*);
    void onRedoTill(CCObject*);
    void onDetails(CCObject*);
    
public:
    static HistoryNode* create(HistoryPopup* popup, EditorEvent* event, float width);

    void updateState(bool redoable);
};

class HistoryPopup : public Popup<History*> {
protected:
    History* m_history;
    std::vector<HistoryNode*> m_items;
    ScrollLayer* m_scroll;

    bool setup(History* history);

    void onClear(CCObject*);
    
public:
    static HistoryPopup* create(History* history);

    void updateState();
    History* getHistory() const;
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
    size_t getMaxCount() const;

    void undo(size_t count = 1);
    void undoTo(EditorEvent* event);
    void redo(size_t count = 1);
    void redoTo(EditorEvent* event);
    bool canUndo() const;
    bool canRedo() const;

    void clear();
};
