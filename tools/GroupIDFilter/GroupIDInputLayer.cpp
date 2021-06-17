#include "GroupIDInputLayer.hpp"

using namespace gd;
using namespace cocos2d;
using namespace gdmake;
using namespace gdmake::extra;

GroupIDInputLayer::IDFilter* g_pGroupFilter;
GroupIDInputLayer::IDFilter* g_pColorFilter;

void GroupIDInputLayer::setup() {
    auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    constexpr const char* inputFilter = "0123456789,|!-+? ";

    if (!g_pGroupFilter) g_pGroupFilter = new GroupIDInputLayer::IDFilter;
    if (!g_pColorFilter) g_pColorFilter = new GroupIDInputLayer::IDFilter;

    this->m_pButtonMenu->addChild(
        CCNodeConstructor<CCMenuItemSpriteExtra*>()
            .fromNode(CCMenuItemSpriteExtra::create(
                CCNodeConstructor()
                    .fromFrameName("GJ_resetBtn_001.png")
                    .scale(.8f)
                    .done(),
                this,
                (SEL_MenuHandler)&GroupIDInputLayer::reset
            ))
            .move(this->m_pLrSize / 2 - CCPoint { 25.0f, 25.0f })
            .done()
    );

    this->m_pLayer->addChild(
        CCNodeConstructor<CCLabelBMFont*>()
            .fromText("Has Groups:", "bigFont.fnt")
            .scale(.5f)
            .move(winSize.width / 2 - 50.0f, winSize.height / 2 + 50.0f)
            .done()
    );
    this->m_pLayer->addChild(
        CCNodeConstructor<InputNode*>()
            .fromNode(InputNode::create(this->m_pLrSize.width - 60.0f, "1,2,3,...", "chatFont.fnt", inputFilter, 50))
            .exec([](auto t) -> void { t->setString(g_pGroupFilter->filterString.c_str()); })
            .move(winSize.width / 2, winSize.height / 2 + 20.0f)
            .save(&m_pGroupInput)
            .done()
    );
    this->m_pLayer->addChild(
        CCNodeConstructor<CCMenu*>()
            .fromMenu()
            .add(CCNodeConstructor<CCMenuItemToggler*>()
                .fromNode(CCMenuItemToggler::createWithStandardSprites(
                    this, (SEL_MenuHandler)&GroupIDInputLayer::onStrict
                ))
                .exec([](auto t) -> void {
                    t->setUserData(&g_pGroupFilter->strict);
                    t->toggle(g_pGroupFilter->strict);
                })
                .scale(.6f)
                .save(&m_pGroupStrict)
                .done())
            .add(CCNodeConstructor<CCLabelBMFont*>()
                .fromText("Strict", "bigFont.fnt")
                .move(50.0f, 0.0f).scale(.55f).done()
            ).move(winSize.width / 2 + 30.0f, winSize.height / 2 + 50.0f).done()
    );

    this->m_pLayer->addChild(
        CCNodeConstructor<CCLabelBMFont*>()
            .fromText("Has Colors:", "bigFont.fnt")
            .scale(.5f)
            .move(winSize.width / 2 - 50.0f, winSize.height / 2 - 15.0f)
            .done()
    );
    this->m_pLayer->addChild(
        CCNodeConstructor<InputNode*>()
            .fromNode(InputNode::create(this->m_pLrSize.width - 60.0f, "1,2,3,...", "chatFont.fnt", inputFilter, 50))
            .exec([](auto t) -> void { t->setString(g_pColorFilter->filterString.c_str()); })
            .move(winSize.width / 2, winSize.height / 2 - 45.0f)
            .save(&m_pColorInput)
            .done()
    );

    this->registerWithTouchDispatcher();
    CCDirector::sharedDirector()->getTouchDispatcher()->incrementForcePrio(2);

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
                (SEL_MenuHandler)&GroupIDInputLayer::onClose
            ))
            .move(0.0f, - this->m_pLrSize.height / 2 + 25.0f)
            .done()
    );
}

void GroupIDInputLayer::onStrict(CCObject* pSender) {
    auto toggle = as<CCMenuItemToggler*>(pSender);
    
    *as<bool*>(toggle->getUserData()) = toggle->isToggled();
}

void GroupIDInputLayer::onClose(CCObject*) {
    g_pGroupFilter->parse(this->m_pGroupInput->getString(), this->m_pGroupStrict->isToggled());
    g_pColorFilter->parse(this->m_pColorInput->getString(), false);

    BrownAlertDelegate::onClose(nullptr);
}

void GroupIDInputLayer::reset(CCObject*) {
    this->m_pColorInput->setString("");
    this->m_pGroupInput->setString("");

    g_pGroupFilter->filterString = "";
    g_pColorFilter->filterString = "";

    g_pGroupFilter->filters.clear();
    g_pColorFilter->filters.clear();
}

std::vector<std::string> GroupIDInputLayer::splitString(std::string const& str, char split) {
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

GroupIDInputLayer::IDFilter* GroupIDInputLayer::getGroupFilter() {
    return g_pGroupFilter;
}

GroupIDInputLayer::IDFilter* GroupIDInputLayer::getColorFilter() {
    return g_pColorFilter;
}

bool GroupIDInputLayer::noFilters() {
    if (!g_pGroupFilter || !g_pColorFilter)
        return true;

    return g_pGroupFilter->filters.empty() && g_pColorFilter->filters.empty();
}

GroupIDInputLayer* GroupIDInputLayer::create() {
    auto ret = new GroupIDInputLayer();

    if (ret && ret->init(260.0f, 220.0f, "GJ_square01.png", "Advanced Filter")) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}
