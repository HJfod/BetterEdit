#include "UndoHistoryManager.hpp"

UndoHistoryManager* g_manager;

void UndoHistoryManager::addAction(ActionObject* action) {
    this->m_pActions->addObject(action);
}

void UndoHistoryManager::removeAction(ActionObject* action) {
    this->m_pActions->removeObject(action);
}

CCArray* UndoHistoryManager::getActionHistory() {
    return this->m_pActions;
}


UndoHistoryManager::~UndoHistoryManager() {
    this->m_pActions->release();
}

bool UndoHistoryManager::init() {
    this->m_pActions = CCArray::create();
    this->m_pActions->retain();

    return true;
}

bool UndoHistoryManager::initGlobal() {
    if (!g_manager) {
        g_manager = new UndoHistoryManager;
        if (g_manager && g_manager->init()) {
            return true;
        }
        CC_SAFE_DELETE(g_manager);
        return false;
    }
    return true;
}

UndoHistoryManager* UndoHistoryManager::get() {
    return g_manager;
}

