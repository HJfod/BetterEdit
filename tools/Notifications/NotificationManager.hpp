#pragma once

#include "../../BetterEdit.hpp"
#include "Notification.hpp"

class NotificationManager : public NotificationDelegate {
    protected:
        CCArray* m_pQueue;
        bool m_bShowingNotification = false;

        bool init();
        void showNextNotificationInQueue();

        void notificationClosed(Notification*) override;

        virtual ~NotificationManager();

    public:
        static bool initGlobal();
        static NotificationManager* get();

        void schedule(Notification*);
};
