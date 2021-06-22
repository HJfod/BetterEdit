#include "AdvancedFilterLayer.hpp"

using namespace gd;
using namespace cocos2d;
using namespace gdmake;
using namespace gdmake::extra;

AdvancedFilterLayer::ObjFilter* g_pFilter;

void AdvancedFilterLayer::setup() {
    auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    constexpr const char* inputFilter = "0123456789,|!-+? ";

    if (!g_pFilter) g_pFilter = new AdvancedFilterLayer::ObjFilter;

    this->m_pButtonMenu->addChild(
        CCNodeConstructor<CCMenuItemSpriteExtra*>()
            .fromNode(CCMenuItemSpriteExtra::create(
                CCNodeConstructor()
                    .fromFrameName("GJ_resetBtn_001.png")
                    .scale(.8f)
                    .done(),
                this,
                (SEL_MenuHandler)&AdvancedFilterLayer::reset
            ))
            .move(this->m_pLrSize / 2 - CCPoint { 25.0f, 25.0f })
            .done()
    );

    this->m_pButtonMenu->addChild(
        CCNodeConstructor<CCMenuItemSpriteExtra*>()
            .fromNode(CCMenuItemSpriteExtra::create(
                CCNodeConstructor<ButtonSprite*>()
                    .fromNode(ButtonSprite::create(
                        "OK", 0, 0, "goldFont.fnt", "GJ_button_01.png", 0, .8f
                    ))
                    .scale(.8f)
                    .done(),
                this,
                (SEL_MenuHandler)&AdvancedFilterLayer::onClose
            ))
            .move(0.0f, - this->m_pLrSize.height / 2 + 25.0f)
            .done()
    );
}

void AdvancedFilterLayer::onStrict(CCObject* pSender) {
    auto toggle = as<CCMenuItemToggler*>(pSender);
    
    *as<bool*>(toggle->getUserData()) = toggle->isToggled();
}

void AdvancedFilterLayer::onClose(CCObject*) {
    g_pFilter->groups->parse(this->m_pGroupInput->getString(), this->m_pGroupStrict->isToggled());

    if (this->m_pSenderBtn)
        as<ButtonSprite*>(this->m_pSenderBtn->getNormalImage())
            ->updateBGImage(this->noFilter() ? "GJ_button_04.png" : "GJ_button_02.png");

    BrownAlertDelegate::onClose(nullptr);
}

void AdvancedFilterLayer::reset(CCObject*) {
    this->m_pColorInput->setString("");
    this->m_pGroupInput->setString("");

    g_pFilter->clearFilters();
}

std::vector<std::string> AdvancedFilterLayer::splitString(std::string const& str, char split) {
    std::vector<std::string> res;

    if (!str.size())
        return res;

    std::string collect = "";
    for (auto c : str)
        if (c == split) {
            res.push_back(collect);
            collect = "";
        } else
            collect += c;
    
    res.push_back(collect);

    return res;
}

AdvancedFilterLayer::ObjFilter* AdvancedFilterLayer::getFilter() {
    return g_pFilter;
}

bool AdvancedFilterLayer::noFilter() {
    if (!g_pFilter)
        return true;

    return g_pFilter->isEmpty();
}

AdvancedFilterLayer* AdvancedFilterLayer::create(CCMenuItemSpriteExtra* pSender) {
    auto ret = new AdvancedFilterLayer();

    if (ret && ret->init(260.0f, 220.0f, "GJ_square01.png", "Advanced Filter")) {
        ret->autorelease();
        ret->m_pSenderBtn = pSender;
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}
