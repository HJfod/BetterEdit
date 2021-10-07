#pragma once

#include "../../BetterEdit.hpp"

enum NotificationType {
    kNotificationTypeInfo,
    kNotificationTypeError,
};

class Notification;

struct NotificationDelegate {
    virtual void notificationClosed(Notification*) {}
};

class Notification : public CCNode {
    protected:
        NotificationDelegate* m_pDelegate = nullptr;

        bool init(NotificationType, const char*);

    public:
        static Notification* create(NotificationType, const char* msg);

        void show();
        void waitToHide();
        void hide();
        void setDelegate(NotificationDelegate*);
};
