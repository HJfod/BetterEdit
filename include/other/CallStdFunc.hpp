#pragma once

#include <Geode/DefaultInclude.hpp>
#include <Geode/cocos/include/cocos2d.h>

namespace better_edit {
    class CallStdFunc : public cocos2d::CCActionInstant {
    protected:
        std::function<void()> m_function;

        bool init(std::function<void()> func);

        void update(float) override;
    
    public:
        static CallStdFunc* create(std::function<void()> func);

        CCFiniteTimeAction* reverse() override;
    };
}
