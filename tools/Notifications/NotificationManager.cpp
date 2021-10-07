#include "NotificationManager.hpp"

NotificationManager* g_manager;

bool NotificationManager::init() {
    this->m_pQueue = CCArray::create(),
    this->m_pQueue->retain();

    return true;
}

NotificationManager::~NotificationManager() {
    this->m_pQueue->release();
}

void NotificationManager::schedule(Notification* n) {
    n->setDelegate(this);
    this->m_pQueue->addObject(n);
    this->showNextNotificationInQueue();
}

void NotificationManager::notificationClosed(Notification*) {
    this->m_bShowingNotification = false;
    this->showNextNotificationInQueue();
}

void NotificationManager::showNextNotificationInQueue() {
    if (
        !this->m_bShowingNotification &&
        this->m_pQueue->count()
    ) {
        as<Notification*>(this->m_pQueue->objectAtIndex(0))->show();
        this->m_pQueue->removeObjectAtIndex(0);
        this->m_bShowingNotification = true;
    }
}

bool NotificationManager::initGlobal() {
    if (!g_manager) {
        g_manager = new NotificationManager;
        if (g_manager && g_manager->init()) {
            return true;
        }
        CC_SAFE_DELETE(g_manager);
        return false;
    }
    return true;
}

NotificationManager* NotificationManager::get() {
    return g_manager;
}


