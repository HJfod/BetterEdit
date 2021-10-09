#include "NotificationManager.hpp"

NotificationManager* g_manager;

bool NotificationManager::init() {
    this->m_pQueue = CCArray::create(),
    this->m_pQueue->retain();
    this->m_pVisible = CCArray::create(),
    this->m_pVisible->retain();

    return true;
}

NotificationManager::~NotificationManager() {
    this->m_pQueue->release();
    this->m_pVisible->release();
}

void NotificationManager::schedule(Notification* n) {
    n->setDelegate(this);
    this->m_pQueue->addObject(n);
    this->showNextNotificationInQueue();
}

void NotificationManager::notificationClosed(Notification* n) {
    this->showNextNotificationInQueue();
    this->m_pVisible->removeObject(n);
    CCARRAY_FOREACH_B_TYPE(this->m_pVisible, n, Notification) {
        if (!n->isHiding()) {
            n->runAction(CCEaseExponentialInOut::create(
                CCMoveBy::create(.3f, { 0, -Notification::s_fNotificationHeight - 10.f })
            ));
        }
    }
}

void NotificationManager::showNextNotificationInQueue() {
    if (this->m_pQueue->count()) {
        if (this->m_pVisible->count() < 5) {
            auto n = as<Notification*>(this->m_pQueue->objectAtIndex(0));
            n->show();
            n->setPositionY(
                n->getPositionY() +
                    this->m_pVisible->count() *
                    (Notification::s_fNotificationHeight + 10.f)
            );

            this->m_pVisible->addObject(n);
            this->m_pQueue->removeObjectAtIndex(0);
        }
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


