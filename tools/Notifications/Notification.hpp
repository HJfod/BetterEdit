#pragma once

#include "../../BetterEdit.hpp"
#include "../CustomKeybinds/SuperMouseManager.hpp"

enum NotificationType {
    kNotificationTypeInfo,
    kNotificationTypeError,
};

class Notification;
class NotificationManager;

struct NotificationDelegate {
    virtual void notificationClosed(Notification*) {}
};

class Notification : public CCNode, public SuperMouseDelegate {
    public:
        static const float s_fNotificationHeight;
        using Click = std::function<void()>;
    
    protected:
        CCScale9Sprite* m_pBGSprite;
        NotificationDelegate* m_pDelegate = nullptr;
        CCMenuItemSpriteExtra* m_pCloseBtn;
        bool m_bHiding = false;
        bool m_bHideAutomatically = true;
        float m_fHideTime = 2.f;
        Click m_notificationClicked = nullptr;
        CCNode* m_pTarget = nullptr;

        friend class NotificationManager;

        bool init(NotificationType, std::string const&, bool, Click, CCNode*);

        void onHide(CCObject*); 

        void mouseMoveSuper(CCPoint const&) override;
        void mouseLeaveSuper(CCPoint const&) override;
        bool mouseDownSuper(MouseButton, CCPoint const&) override;
        bool mouseUpSuper(MouseButton, CCPoint const&) override;

    public:
        static Notification* create(
            NotificationType type,
            std::string const& msg,
            bool hideAuto = true,
            Click = nullptr,
            CCNode* target = nullptr
        );
        static Notification* create(
            NotificationType,
            std::string const& msg,
            Click cb,
            CCNode* target = nullptr
        );

        void show();
        void waitToHide();
        bool isHiding();
        void hide();
        void setDelegate(NotificationDelegate*);
};
