#include "UndoHistory.hpp"
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/binding/GJGameLevel.hpp>

void History::onEvent(EditorEvent* ev) {
    log::info("{}", ev->toDiffString());
    m_events.emplace_back(ev->unique());
}

History* History::create(GJGameLevel* level) {
    auto ret = new History();
    ret->m_initialString = level->m_levelString;
    ret->autorelease();
    return ret;
}

struct $modify(LevelEditorLayer) {
    Ref<History> history;

    bool init(GJGameLevel* level) {
        if (!LevelEditorLayer::init(level))
            return false;
        
        m_fields->history = History::create(level);

        return true;
    }
};
