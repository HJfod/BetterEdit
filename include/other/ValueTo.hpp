#pragma once

#include <cocos2d.h>

namespace better_edit {
    template <class T>
    class ValueTo : public cocos2d::CCActionInterval {
    protected:
        T& m_target;
        T m_to;
        T m_from;
        std::function<void(T&)> m_callback = nullptr;

        bool initWithDuration(float duration, T& target, T to, std::function<void(T&)> callback) {
            if (!cocos2d::CCActionInterval::initWithDuration(duration))
                return false;
            
            m_from = target;
            m_to = to;
            m_callback = callback;

            return true;
        }

        void startWithTarget(cocos2d::CCNode* target) override {
            cocos2d::CCActionInterval::startWithTarget(target);
        }
        
        cocos2d::CCActionInterval* reverse() override {
            return ValueTo<T>::create(m_fDuration, m_target, m_from, m_callback);
        }

        void update(float time) override {
            m_target = m_from + (m_to - m_from) * time;
            if (m_callback) {
                m_callback(m_target);
            }
        }

        ValueTo(T& target) : m_target(target) {}

    public:
        static ValueTo* create(float duration, T& target, T to, std::function<void(T&)> callback = nullptr) {
            auto ret = new ValueTo(target);
            if (ret && ret->initWithDuration(duration, target, to, callback)) {
                ret->autorelease();
                return ret;
            }
            CC_SAFE_DELETE(ret);
            return nullptr;
        }
    };
}
