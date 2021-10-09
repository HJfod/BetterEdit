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
    public:
        static const float s_fNotificationHeight;
    
    protected:
        NotificationDelegate* m_pDelegate = nullptr;
        bool m_bHiding = false;

        bool init(NotificationType, const char*);

    public:
        static Notification* create(NotificationType, const char* msg);

        void show();
        void waitToHide();
        bool isHiding();
        void hide();
        void setDelegate(NotificationDelegate*);
};
