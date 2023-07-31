#pragma once

#include "../Macros.hpp"
#include <cocos2d.h>

namespace better_edit {
    class BE_DLL Clip : public cocos2d::CCClippingNode {
    protected:
        cocos2d::CCLayerColor* m_stencil;

        bool init(bool invert);

        void updateClippingRect();

    public:
        static Clip* create(bool invert = false);

        void setContentSize(cocos2d::CCSize const& size) override;
    };
}
