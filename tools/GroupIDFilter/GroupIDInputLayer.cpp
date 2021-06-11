#include "GroupIDInputLayer.hpp"

using namespace gd;
using namespace cocos2d;
using namespace gdmake;
using namespace gdmake::extra;

GroupIDInputLayer::IDFilter g_vGroupFilter;
GroupIDInputLayer::IDFilter g_vColorFilter;

static std::vector<int> vsti(std::vector<std::string> const& v) {
    std::vector<int> res;

    for (auto i : v)
        res.push_back(std::stoi(i));
    
    return res;
}

void GroupIDInputLayer::setup() {
    auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    this->m_pButtonMenu->addChild(
        CCNodeConstructor<CCMenuItemSpriteExtra*>()
            .fromNode(CCMenuItemSpriteExtra::create(
                CCNodeConstructor()
                    .fromFrameName("GJ_resetBtn_001.png")
                    .scale(.7f)
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
            .move(winSize.width / 2, winSize.height / 2 + 50.0f)
            .done()
    );
    this->m_pLayer->addChild(
        CCNodeConstructor<InputNode*>()
            .fromNode(InputNode::create(this->m_pLrSize.width - 60.0f, "1,2,3,...", "chatFont.fnt", "0123456789,/! ", 50))
            .move(winSize.width / 2, winSize.height / 2 + 20.0f)
            .save(&m_pGroupInput)
            .done()
    );
    this->m_pLayer->addChild(
        CCNodeConstructor<CCMenu*>()
            .fromMenu()
            .add(CCNodeConstructor<CCMenuItemToggler*>()
                .fromNode(CCMenuItemToggler::createWithStandardSprites(this, nullptr))
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
            .move(winSize.width / 2, winSize.height / 2 - 15.0f)
            .done()
    );
    this->m_pLayer->addChild(
        CCNodeConstructor<InputNode*>()
            .fromNode(InputNode::create(this->m_pLrSize.width - 60.0f, "1,2,3,...", "chatFont.fnt", "0123456789, ", 50))
            .move(winSize.width / 2, winSize.height / 2 - 45.0f)
            .save(&m_pColorInput)
            .done()
    );
    this->m_pLayer->addChild(
        CCNodeConstructor<CCMenu*>()
            .fromMenu()
            .add(CCNodeConstructor<CCMenuItemToggler*>()
                .fromNode(CCMenuItemToggler::createWithStandardSprites(this, nullptr))
                .scale(.6f)
                .save(&m_pColorStrict)
                .done())
            .add(CCNodeConstructor<CCLabelBMFont*>()
                .fromText("Strict", "bigFont.fnt")
                .move(50.0f, 0.0f).scale(.55f).done()
            ).move(winSize.width / 2 + 30.0f, winSize.height / 2 - 15.0f).done()
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

void GroupIDInputLayer::onClose(CCObject*) {
    g_vGroupFilter = this->parseString(this->m_pGroupInput->getString(), this->m_pGroupStrict->isToggled());
    g_vColorFilter = this->parseString(this->m_pColorInput->getString(), this->m_pColorStrict->isToggled());

    BrownAlertDelegate::onClose(nullptr);
}

void GroupIDInputLayer::reset(CCObject*) {
    this->m_pColorInput->setString("");
    this->m_pGroupInput->setString("");
}

GroupIDInputLayer::IDFilter GroupIDInputLayer::parseString(std::string const& str, bool strict) {
    IDFilter res;

    res.strict = strict;

    for (auto f : splitString(str, ',')) {
        
    }

    return res;
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

GroupIDInputLayer::IDFilter const& GroupIDInputLayer::getGroupFilter() {
    return g_vGroupFilter;
}

GroupIDInputLayer::IDFilter const& GroupIDInputLayer::getColorFilter() {
    return g_vColorFilter;
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
