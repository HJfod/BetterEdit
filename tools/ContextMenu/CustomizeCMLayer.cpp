#include "CustomizeCMLayer.hpp"
#include "../FLAlertLayerFix/FLAlertLayerFix.hpp"

int g_nSelectedMode = 0;

bool CustomizeCMLayer::init() {
    if (!CCNode::init())
        return false;
    
    auto winSize = CCDirector::sharedDirector()->getWinSize();

	this->m_pBG = CCSprite::create("GJ_gradientBG.png");
	auto bgSize = this->m_pBG->getTextureRect().size;
	this->m_pBG->setAnchorPoint({ 0.0f, 0.0f });
	this->m_pBG->setScaleX((winSize.width + 10.0f) / bgSize.width);
	this->m_pBG->setScaleY((winSize.height + 50.0f) / bgSize.height);
	this->m_pBG->setPosition({ -5.0f, -5.0f });
	this->m_pBG->setColor({ 2, 7, 20 });
    this->m_pBG->setOpacity(220);
	this->addChild(this->m_pBG);

    this->m_nSelectedMode = g_nSelectedMode;

    this->m_vModes = {
        ContextMenu::kStateNoneSelected,
        ContextMenu::kStateOneSelected,
        ContextMenu::kStateManySelected,
    };

    this->m_pContextMenu = ContextMenu::create();
    this->m_pContextMenu->setDisabled(true);
    SuperKeyboardManager::get()->popDelegate(this->m_pContextMenu);
    this->m_pContextMenu->setScale(1.5f);
    this->m_pContextMenu->generate(this->m_vModes[this->m_nSelectedMode]);
    this->m_pContextMenu->setPosition(
        winSize / 2 - this->m_pContextMenu->getContentSize() / 2
    );
    this->addChild(this->m_pContextMenu);

    this->m_pButtonMenu = CCMenu::create();
    this->m_pButtonMenu->setPosition(winSize / 2);
    this->addChild(this->m_pButtonMenu);

    this->m_pSelectedModeLabel = CCLabelBMFont::create("", "bigFont.fnt");
    this->m_pSelectedModeLabel->setPosition(winSize.width / 2, winSize.height - 50.0f);
	this->addChild(this->m_pSelectedModeLabel);

    this->m_pButtonMenu->addChild(CCNodeConstructor<CCMenuItemSpriteExtra*>()
        .fromNode(CCMenuItemSpriteExtra::create(
            CCNodeConstructor()
                .fromFrameName("GJ_arrow_01_001.png")
                .scale(.5f)
                .done(),
            this,
            (SEL_MenuHandler)&CustomizeCMLayer::onSelectMode
        ))
        .tag(-1)
        .move(- 60.0f, winSize.height / 2 - 50.f)
        .done()
    );
    this->m_pButtonMenu->addChild(CCNodeConstructor<CCMenuItemSpriteExtra*>()
        .fromNode(CCMenuItemSpriteExtra::create(
            CCNodeConstructor()
                .fromFrameName("GJ_arrow_01_001.png")
                .scale(.5f)
                .flipX()
                .done(),
            this,
            (SEL_MenuHandler)&CustomizeCMLayer::onSelectMode
        ))
        .tag(1)
        .move(60.0f, winSize.height / 2 - 50.f)
        .done()
    );

    this->setSuperMouseHitOffset(winSize / 2);
    this->setSuperMouseHitSize(winSize);

    this->updateLabels();

    return true;
}

std::ostream& operator<<(std::ostream& stream, CCRect const& size);

CCMenuItem* CustomizeCMLayer::itemUnderMouse(CCPoint const& mpos) {
    CCARRAY_FOREACH_B_TYPE(this->m_pButtonMenu->getChildren(), btn, CCMenuItem) {
        auto pos = this->m_pButtonMenu->convertToWorldSpace(btn->getPosition());
        auto size = btn->getScaledContentSize();
        auto rect = CCRect {
            pos.x - size.width / 2,
            pos.y - size.height / 2,
            size.width,
            size.height
        };

        if (rect.containsPoint(mpos))
            return btn;
    }
    return nullptr;
}

ContextMenuItem* CustomizeCMLayer::contextItemUnderMouse(CCPoint const& mpos) {
    CCARRAY_FOREACH_B_TYPE(this->m_pContextMenu->getChildren(), btn, ContextMenuItem) {
        auto pos = this->m_pButtonMenu->convertToWorldSpace(btn->getPosition());
        auto size = btn->getScaledContentSize();
        auto rect = CCRect {
            pos.x - size.width / 2,
            pos.y - size.height / 2,
            size.width,
            size.height
        };

        if (rect.containsPoint(mpos))
            return btn;
    }
    return nullptr;
}

bool CustomizeCMLayer::mouseDownSuper(MouseButton mb, CCPoint const& pos) {
    auto btn = this->itemUnderMouse(pos);
    if (btn) {
        btn->selected();
        this->m_pClickedBtn = btn;
    }
    auto item = this->contextItemUnderMouse(pos);
    if (item) {
        item->mouseDownSuper(mb, pos);
        this->m_pHoverItem = item;
    }
    return true;
}

bool CustomizeCMLayer::mouseUpSuper(MouseButton mb, CCPoint const& pos) {
    auto btn = this->itemUnderMouse(pos);
    if (btn) btn->activate();
    this->m_pClickedBtn = nullptr;
    this->m_pHoverItem = nullptr;
    auto item = this->contextItemUnderMouse(pos);
    if (item) item->mouseUpSuper(mb, pos);
    return true;
}

void CustomizeCMLayer::mouseMoveSuper(CCPoint const& pos) {
    if (this->m_bSuperMouseDown) {
        auto btn = this->itemUnderMouse(pos);
        if (btn != this->m_pClickedBtn) {
            if (this->m_pClickedBtn)
                this->m_pClickedBtn->unselected();
            if (btn)
                btn->selected();
            this->m_pClickedBtn = btn;
        }
    }
    auto item = this->contextItemUnderMouse(pos);
    if (item != this->m_pHoverItem) {
        if (this->m_pHoverItem)
            this->m_pHoverItem->mouseMoveSuper(pos);
        if (item)
            item->mouseMoveSuper(pos);
        this->m_pHoverItem = item;
    }
}

bool CustomizeCMLayer::mouseScrollSuper(float, float) {
    return true;
}

CustomizeCMLayer::~CustomizeCMLayer() {
    SuperMouseManager::get()->releaseCapture(this);

    g_nSelectedMode = this->m_nSelectedMode;
}

bool CustomizeCMLayer::keyDownSuper(enumKeyCodes key) {
    if (key == KEY_Escape) {
        this->hide();
    }

    return true;
}

bool CustomizeCMLayer::keyUpSuper(enumKeyCodes) {
    return true;
}

void CustomizeCMLayer::updateLabels() {
    this->m_pSelectedModeLabel->setString(
        ContextMenu::stateToString(this->m_vModes.at(this->m_nSelectedMode)).c_str()
    );
    this->m_pSelectedModeLabel->limitLabelWidth(80.f, .5f, .1f);
}

void CustomizeCMLayer::onSelectMode(CCObject* pSender) {
    this->m_nSelectedMode += pSender->getTag();

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    if (this->m_nSelectedMode < 0)
        this->m_nSelectedMode = m_vModes.size() - 1;
    if (this->m_nSelectedMode > static_cast<int>(m_vModes.size() - 1))
        this->m_nSelectedMode = 0;
    
    this->m_pContextMenu->generate(this->m_vModes[this->m_nSelectedMode]);
    this->m_pContextMenu->setPosition(
        winSize / 2 - this->m_pContextMenu->getContentSize() / 2
    );
    this->updateLabels();
}

void CustomizeCMLayer::show() {
    auto scene = CCDirector::sharedDirector()->getRunningScene();
    fixLayering(this);
    scene->addChild(this);
    this->m_pBG->setOpacity(0);
    this->m_pBG->runAction(CCEaseOut::create(
        CCFadeTo::create(.1f, 240), 2.f
    ));
}

void CustomizeCMLayer::hide() {
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    this->m_pBG->runAction(CCSequence::create(
        CCFadeTo::create(.1f, 0),
        CCCallFunc::create(this, callfunc_selector(CustomizeCMLayer::removeFromParent)),
        nullptr
    ));
}

CustomizeCMLayer* CustomizeCMLayer::create() {
    auto ret = new CustomizeCMLayer;

    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}
