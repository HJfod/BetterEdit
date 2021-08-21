#pragma once

#include <GDMake.h>

using namespace gd;
using namespace cocos2d;

#define MAKE_INFOBUTTON(title, desc, scale, x, y, menu)            \
    auto mc_infoButton = InfoButton::create(title, desc, scale);   \
    mc_infoButton->setPosition(x, y);                              \
    menu->addChild(mc_infoButton);

class InfoButton : public CCMenuItemSpriteExtra {
    protected:
        std::string m_sTitle;
        std::string m_sDescription;

        bool init(std::string const& title, std::string const& desc, float scale);

        void activate() override;

    public:
        static InfoButton* create(std::string const& title, std::string const& desc, float scale);
};
