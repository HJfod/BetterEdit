#include "StartPosManager.hpp"

StartPosManager* StartPosManager::get() {
    if (!s_instance) {
        auto instance = new StartPosManager;
        if (instance) {
            s_instance = instance;
            return instance;
        }

        CC_SAFE_DELETE(instance);
        return instance;
    }
    return s_instance;
}

void StartPosManager::setStartPositions(CCArray* objects) {
    m_positions.clear();
    m_positions.push_back(CCPointZero);
    if (!objects) {
        return;
    }
    for (auto object : CCArrayExt<GameObject*>(objects)) {
        if (object->m_objectID == 31) {
            auto skip = false;
            for (auto position : m_positions) {
                if (position == object->getPosition()) {
                    skip = true;
                    break;
                }
            }
            if (skip) {
                continue;
            }
            m_positions.push_back(object->getPosition());
            auto editor = LevelEditorLayer::get();
            if (m_active != CCPointZero && editor && !editor->m_editorInitialising) {
                if (object->getPosition() == editor->m_editorUI->getGridSnappedPos(m_active)) {
                    m_active = object->getPosition();
                }
            }
        }
    }

    this->sort();

    if (m_positions.size() == 1) {
        m_active = CCPointZero;
        return;
    }
    if (m_initialized) {
        return;
    }

    m_active = m_positions.at(m_positions.size() - 1);
    m_initialized = true;
}

void StartPosManager::sort() {
    std::sort(m_positions.begin(), m_positions.end(), [](CCPoint first, CCPoint second) {
        return first.x < second.x;
    });
}

void StartPosManager::setActive(CCPoint const& pos) {
    bool set = false;
    for (auto startPos : m_positions) {
        if (startPos == pos) {
            m_active = pos;
            set = true;
            break;
        }
    }

    if (!set) {
        m_active = m_positions.at(m_positions.size() - 1);
    }
}

void StartPosManager::addStartPos(CCPoint const& position) {
    for (auto startPosition : m_positions) {
        if (startPosition == position) {
            return;
        }
    }
    m_positions.push_back(position);
    this->sort();
}

void StartPosManager::replaceStartPos(CCPoint const& before, CCPoint const& after) {
    if (!LevelEditorLayer::get() || LevelEditorLayer::get()->m_editorInitialising) {
        return;
    }

    for (auto& position : m_positions) {
        if (position == before) {
            position = after;
            break;
        }
    }
    if (m_active == before) {
        m_active = after;
    }

    this->sort();
}

StartPosObject* StartPosManager::getStartPosFromPoint(CCPoint const& point) {
    if (LevelEditorLayer::get()) {
        for (auto object : CCArrayExt<GameObject*>(LevelEditorLayer::get()->m_objects)) {
            if (object->m_objectID == 31) {
                if (object->getPosition() == point) {
                    return static_cast<StartPosObject*>(object);
                }

                if (!LevelEditorLayer::get()->m_editorInitialising && object->getPosition() == LevelEditorLayer::get()->m_editorUI->getGridSnappedPos(point)) {
                    this->replaceStartPos(point, LevelEditorLayer::get()->m_editorUI->getGridSnappedPos(point));
                    return static_cast<StartPosObject*>(object);
                }
            }
        }

        return nullptr;
    }

    if (PlayLayer::get()) {
        for (auto object : CCArrayExt<GameObject*>(PlayLayer::get()->m_objects)) {
            if (
                object->m_objectID == 31 && 
                object->getPosition() == point
            ) {
                return static_cast<StartPosObject*>(object);
            }
        }
    }
    return nullptr;
}

void StartPosManager::next() {
    for (auto pos : m_positions) {
        if (pos.x > m_active.x) {
            m_active = pos;
            return;
        }
    }
}

void StartPosManager::previous() {
    auto found = CCPointZero;
    for (auto pos : m_positions) {
        if (pos.x < m_active.x) {
            found = pos;
        }
    }

    m_active = found;
}

void StartPosManager::clear() {
    m_active = CCPointZero;
    m_positions.clear();
    m_initialized = false;
}

bool StartPosManager::isDefault() {
    if (m_positions.size() == 0) {
        return true;
    }
    return m_active == m_positions.at(m_positions.size() - 1);
}

bool StartPosManager::isLevelStart() {
    if (m_positions.size() == 0) {
        return false;
    }
    return m_active == m_positions.at(0);
}

CCPoint StartPosManager::getActive() {
    return m_active;
}

std::vector<CCPoint> StartPosManager::getPositions() {
    return m_positions;
}

void StartPosManager::setDefault() {
    m_active = m_positions.at(m_positions.size() - 1);
}

void StartPosManager::setFirst() {
    m_active = m_positions.at(0);
}