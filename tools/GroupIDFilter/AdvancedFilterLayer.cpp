#include "AdvancedFilterLayer.hpp"
#include <HorizontalSelectMenu.hpp>

using namespace gd;
using namespace cocos2d;
using namespace gdmake;
using namespace gdmake::extra;

AdvancedFilterLayer::ObjFilter* g_pFilter;

#define VOIDP_AS(T, v) static_cast<T>(reinterpret_cast<intptr_t>(v))

void AdvancedFilterLayer::setup() {
    auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    constexpr const char* rangeFilter  = "0123456789- ";
    constexpr const char* rangeFilterF = "0123456789.- ";
    constexpr const char* rangeFilterC = "0123456789-bglineojdpwhtackrBGLINEOJDPWHTACKR ";

    if (!g_pFilter) g_pFilter = new AdvancedFilterLayer::ObjFilter;

    {   // Group ID
        this->m_pLayer->addChild(
            CCNodeConstructor<InputNode*>()
                .fromNode(InputNode::create(200.0f, "Groups (1,2,3,...)", "chatFont.fnt", "0123456789,|!-+? ", 100))
                .move(winSize.width / 2 - 45.0f, winSize.height / 2 + this->m_pLrSize.height / 2 - 60.0f)
                .exec([this](auto self) -> void { this->m_vInputs.push_back(self); })
                .udata(g_pFilter->groups)
                .done()
        );

        this->m_pButtonMenu->addChild(
            CCNodeConstructor<CCMenuItemToggler*>()
                .fromNode(CCMenuItemToggler::createWithStandardSprites(
                    this, (SEL_MenuHandler)&AdvancedFilterLayer::onStrict, .7f
                ))
                .move(75.0f, this->m_pLrSize.height / 2 - 60.0f)
                .udata(&g_pFilter->groups->strict)
                .exec([](auto self) -> void { self->toggle(g_pFilter->groups->strict); })
                .done()
        );
        this->m_pButtonMenu->addChild(
            CCNodeConstructor<CCLabelBMFont*>()
                .fromText("Strict", "bigFont.fnt")
                .move(120.0f, this->m_pLrSize.height / 2 - 60.0f)
                .scale(.525f)
                .done()
        );
    }

    {   // Colors
        this->m_pLayer->addChild(
            CCNodeConstructor<InputNode*>()
                .fromNode(InputNode::create(80.0f, "Color 1", "chatFont.fnt", rangeFilterC, 20))
                .move(winSize.width / 2 - 50.0f, winSize.height / 2 + this->m_pLrSize.height / 2 - 100.0f)
                .exec([this](auto self) -> void { this->m_vInputs.push_back(self); })
                .udata(&g_pFilter->color1)
                .done()
        );
        this->m_pLayer->addChild(
            CCNodeConstructor<InputNode*>()
                .fromNode(InputNode::create(80.0f, "Color 2", "chatFont.fnt", rangeFilterC, 20))
                .move(winSize.width / 2 + 50.0f, winSize.height / 2 + this->m_pLrSize.height / 2 - 100.0f)
                .exec([this](auto self) -> void { this->m_vInputs.push_back(self); })
                .udata(&g_pFilter->color2)
                .done()
        );
    }

    {   // Scale, Z Order
        this->m_pLayer->addChild(
            CCNodeConstructor<InputNode*>()
                .fromNode(InputNode::create(80.0f, "Scale", "chatFont.fnt", rangeFilterF, 20))
                .move(winSize.width / 2 - 50.0f, winSize.height / 2 + this->m_pLrSize.height / 2 - 140.0f)
                .exec([this](auto self) -> void { this->m_vInputs.push_back(self); })
                .udata(&g_pFilter->scale)
                .done()
        );
        this->m_pLayer->addChild(
            CCNodeConstructor<InputNode*>()
                .fromNode(InputNode::create(80.0f, "Z Order", "chatFont.fnt", rangeFilter, 20))
                .move(winSize.width / 2 + 50.0f, winSize.height / 2 + this->m_pLrSize.height / 2 - 140.0f)
                .exec([this](auto self) -> void { this->m_vInputs.push_back(self); })
                .udata(&g_pFilter->zOrder)
                .done()
        );
    }

    {   // High Detail, Low Detail
        this->m_pButtonMenu->addChild(
            CCNodeConstructor<CCMenuItemToggler*>()
                .fromNode(CCMenuItemToggler::createWithStandardSprites(
                    this, (SEL_MenuHandler)&AdvancedFilterLayer::onDetails, .7f
                ))
                .move(-100.0f, this->m_pLrSize.height / 2 - 180.0f)
                .exec([](auto self) -> void {
                    self->toggle(g_pFilter->detail == AdvancedFilterLayer::ObjFilter::Low);
                    self->setTag(4);
                })
                .udata(AdvancedFilterLayer::ObjFilter::Low)
                .done()
        );
        this->m_pButtonMenu->addChild(
            CCNodeConstructor<CCLabelBMFont*>()
                .fromText("Low Detail", "bigFont.fnt")
                .move(-50.0f, this->m_pLrSize.height / 2 - 180.0f)
                .scale(.525f)
                .done()
        );

        this->m_pButtonMenu->addChild(
            CCNodeConstructor<CCMenuItemToggler*>()
                .fromNode(CCMenuItemToggler::createWithStandardSprites(
                    this, (SEL_MenuHandler)&AdvancedFilterLayer::onDetails, .7f
                ))
                .move(0.0f, this->m_pLrSize.height / 2 - 180.0f)
                .exec([](auto self) -> void {
                    self->toggle(g_pFilter->detail == AdvancedFilterLayer::ObjFilter::High);
                    self->setTag(4);
                })
                .udata(AdvancedFilterLayer::ObjFilter::High)
                .done()
        );
        this->m_pButtonMenu->addChild(
            CCNodeConstructor<CCLabelBMFont*>()
                .fromText("High Detail", "bigFont.fnt")
                .move(50.0f, this->m_pLrSize.height / 2 - 180.0f)
                .scale(.525f)
                .done()
        );
    }

    for (auto input : this->m_vInputs)
        input->setString(as<Parseable*>(input->getUserData())->parseString.c_str());

    {   // OK & reset
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
}

void AdvancedFilterLayer::onStrict(CCObject* pSender) {
    auto toggle = as<CCMenuItemToggler*>(pSender);
    
    *as<bool*>(toggle->getUserData()) = toggle->isToggled();
}

void AdvancedFilterLayer::onDetails(CCObject* pSender) {
    auto pSenderT = as<CCMenuItemToggler*>(pSender);
    auto toggled = pSenderT->isToggled();

    CCARRAY_FOREACH_B_TYPE(this->getChildren(), c, CCMenuItemToggler)
        if (c->getTag() == 4)
            c->toggle(false);

    pSenderT->toggle(toggled);
    
    if (toggled)
        g_pFilter->detail = VOIDP_AS(AdvancedFilterLayer::ObjFilter::EDetail, pSenderT->getUserData());
    else
        g_pFilter->detail = AdvancedFilterLayer::ObjFilter::None;
}

void AdvancedFilterLayer::onClose(CCObject* pSender) {
    for (auto input : this->m_vInputs)
        as<Parseable*>(input->getUserData())->parse(input->getString());

    if (this->m_pSenderBtn)
        as<ButtonSprite*>(this->m_pSenderBtn->getNormalImage())
            ->updateBGImage(this->noFilter() ? "GJ_button_04.png" : "GJ_button_02.png");

    BrownAlertDelegate::onClose(nullptr);
}

void AdvancedFilterLayer::reset(CCObject*) {
    for (auto input : this->m_vInputs)
        input->setString("");

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

    if (ret && ret->init(320.0f, 260.0f, "GJ_square01.png", "Advanced Filter")) {
        ret->autorelease();
        ret->m_pSenderBtn = pSender;
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}
