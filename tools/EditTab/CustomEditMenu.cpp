#include "CustomEditMenu.hpp"
#include "../CustomKeybinds/loadEditorKeybindIndicators.hpp"
#include "MoveInfoPopup.hpp"

#define ADD_MOVE_GROUP(_size_, _spr_, _text_, _scale_, _bind_)  \
    this->addMoveGroup(                                 \
        kEditCommand##_size_##Up,                       \
        kEditCommand##_size_##Down,                     \
        kEditCommand##_size_##Left,                     \
        kEditCommand##_size_##Right,                    \
        _spr_, _text_, _scale_, _bind_                  \
    );                                                  \

bool MoveButtonGroup::init() {
    this->m_pButtons = CCArray::create();
    this->m_pButtons->retain();

    return true;
}

MoveButtonGroup::~MoveButtonGroup() {
    this->m_pButtons->release();
}

MoveButtonGroup* MoveButtonGroup::create() {
    auto ret = new MoveButtonGroup;

    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}

void MoveButtonGroup::addButton(CCMenuItemSpriteExtra* btn) {
    this->m_pButtons->addObject(btn);
}

void MoveButtonGroup::setVisible(bool visible) {
    CCARRAY_FOREACH_B_TYPE(this->m_pButtons, btn, CCMenuItemSpriteExtra)
        btn->setVisible(visible);
}



CCMenuItemSpriteExtra* CustomEditMenu::createButton(
    const char* spr,
    const char* sizeTxt,
    EditCommand com,
    float scale,
    SEL_MenuHandler cb,
    const char* keybind
) {
    auto btn = this->m_pUI->getSpriteButton(
        spr, cb, nullptr, .9f
    );
    btn->setTag(com);

    if (sizeTxt) {
        auto label = CCLabelBMFont::create(sizeTxt, "bigFont.fnt");
        label->setScale(.35f);
        label->setZOrder(50);
        label->setPosition(btn->getContentSize().width / 2, 11.0f);
        btn->addChild(label);
    }

    as<ButtonSprite*>(btn->getNormalImage())
        ->m_pSubSprite->setScale(scale);

    btn->getNormalImage()->setScale(.583f);

    if (keybind)
        addKeybindIndicator(this->m_pUI, btn, keybind);
    
    return btn;
}

void CustomEditMenu::addMoveGroup(
    EditCommand up, EditCommand down,
    EditCommand left, EditCommand right,
    const char* sprNum, const char* sprText, float sprScale,
    const char* keybind
) {
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    auto center = CCPoint {
        (this->m_fWidth - 20.f) / 4.f
            * (this->m_pMoveGroups->count() % 4 - 1.5f) +
            winSize.width / 2,
        0.0f
    };

    auto group = MoveButtonGroup::create();

    auto upBtn = this->createButton(
        CCString::createWithFormat("edit_upBtn%s_001.png", sprNum)->getCString(),
        sprText, up, sprScale, menu_selector(EditorUI::moveObjectCall),
        CCString::createWithFormat(keybind, "up")->getCString()
    );
    upBtn->setPosition(center + CCPoint { 0.f, 12.f });
    upBtn->setVisible(this->m_pMoveGroups->count() < 4);
    this->m_pMenu->addChild(upBtn);
    group->addButton(upBtn);

    auto downBtn = this->createButton(
        CCString::createWithFormat("edit_downBtn%s_001.png", sprNum)->getCString(),
        sprText, down, sprScale, menu_selector(EditorUI::moveObjectCall),
        CCString::createWithFormat(keybind, "down")->getCString()
    );
    downBtn->setPosition(center + CCPoint { 0.f, -12.f });
    downBtn->setVisible(this->m_pMoveGroups->count() < 4);
    this->m_pMenu->addChild(downBtn);
    group->addButton(downBtn);

    auto leftBtn = this->createButton(
        CCString::createWithFormat("edit_leftBtn%s_001.png", sprNum)->getCString(),
        sprText, left, sprScale, menu_selector(EditorUI::moveObjectCall),
        CCString::createWithFormat(keybind, "left")->getCString()
    );
    leftBtn->setPosition(center + CCPoint { -24.f, 0.f });
    leftBtn->setVisible(this->m_pMoveGroups->count() < 4);
    this->m_pMenu->addChild(leftBtn);
    group->addButton(leftBtn);

    auto rightBtn = this->createButton(
        CCString::createWithFormat("edit_rightBtn%s_001.png", sprNum)->getCString(),
        sprText, right, sprScale, menu_selector(EditorUI::moveObjectCall),
        CCString::createWithFormat(keybind, "right")->getCString()
    );
    rightBtn->setPosition(center + CCPoint { 24.f, 0.f });
    rightBtn->setVisible(this->m_pMoveGroups->count() < 4);
    this->m_pMenu->addChild(rightBtn);
    group->addButton(rightBtn);

    this->m_pMoveGroups->addObject(group);
}

void CustomEditMenu::addTransformButton(
    EditCommand command, const char* spr, bool addToDict,
    const char* keybind,
    SEL_MenuHandler cb
) {
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    auto btn = this->createButton(
        spr, nullptr, command, 1.0f, cb, keybind
    );
    if (addToDict) {
        this->m_pUI->m_pEditButtonDict->setObject(
            btn, CCString::createWithFormat("%i", command)->getCString()
        );
    }
    btn->setPositionY(-40.0f);
    this->m_pMenu->addChild(btn);
    this->m_pTransformBtns->addObject(btn);

    CCARRAY_FOREACH_B_BASE(this->m_pTransformBtns, btn, CCMenuItemSpriteExtra*, ix) {
        auto len = 28.0f;
        btn->setPositionX(
            len * (ix - this->m_pTransformBtns->count() / 2.f) +
            winSize.width / 2
        );
    }
}

bool CustomEditMenu::init(EditorUI* ui) {
    auto arr = CCArray::create();
    arr->retain();
    if (!EditButtonBar::init(
        arr,
        0, false,
        GameManager::sharedState()->getIntGameVariable("0049"),
        GameManager::sharedState()->getIntGameVariable("0050"),
        { CCDirector::sharedDirector()->getWinSize().width / 2, 86.0f }
    ))
        return false;
    
    arr->release();
    
    this->m_pUI = ui;
    this->m_pMenu = CCMenu::create();
    this->m_pMoveGroups = CCArray::create();
    this->m_pMoveGroups->retain();
    this->m_pTransformBtns = CCArray::create();
    this->m_pTransformBtns->retain();

    this->m_fWidth = CCDirector::sharedDirector()->getWinSize().width - 240.0f;

    this->addTransformButton(
        kEditCommandFlipX, "edit_flipXBtn_001.png", true,
        "gd.edit.flip_x"
    );
    this->addTransformButton(
        kEditCommandFlipY, "edit_flipYBtn_001.png", true,
        "gd.edit.flip_y"
    );
    this->addTransformButton(
        kEditCommandRotateCW, "edit_cwBtn_001.png", true,
        "gd.edit.rotate_cw"
    );
    this->addTransformButton(
        kEditCommandRotateCCW, "edit_ccwBtn_001.png", true,
        "gd.edit.rotate_ccw"
    );
    this->addTransformButton(
        kEditCommandRotateCW45, "edit_rotate45rBtn_001.png", true,
        "betteredit.rotate_45_cw"
    );
    this->addTransformButton(
        kEditCommandRotateCCW45, "edit_rotate45lBtn_001.png", true,
        "betteredit.rotate_45_ccw"
    );
    this->addTransformButton(
        kEditCommandRotateFree, "edit_freeRotateBtn_001.png", true,
        "gd.edit.toggle_rotate",
        menu_selector(EditorUI::activateRotationControl)
    );
    this->addTransformButton(
        kEditCommandRotateSnap, "edit_rotateSnapBtn_001.png", true,
        "betteredit.rotate_snap"
    );
    this->addTransformButton(
        kEditCommandScale, "edit_scaleBtn_001.png", true,
        "betteredit.show_scale_control",
        menu_selector(EditorUI::activateScaleControl)
    );

    ADD_MOVE_GROUP(Small,   "",  nullptr, 1.0f, "gd.edit.move_obj_small_%s");
    ADD_MOVE_GROUP(,        "2", nullptr, 1.0f, "gd.edit.move_obj_%s");
    ADD_MOVE_GROUP(Tiny,    "",  nullptr, .8f,  "gd.edit.move_obj_tiny_%s");
    ADD_MOVE_GROUP(Big,     "3", nullptr, 1.0f, "gd.edit.move_obj_big_%s");
    ADD_MOVE_GROUP(Half,    "2", "1/2",   1.0f, "betteredit.move_obj_half_%s");
    ADD_MOVE_GROUP(Quarter, "2", "1/4",   1.0f, "betteredit.move_obj_quarter_%s");
    ADD_MOVE_GROUP(Eigth,   "2", "1/8",   1.0f, "betteredit.move_obj_eigth_%s");
    ADD_MOVE_GROUP(QUnit,   "",  "1/2",   .8f,  "betteredit.move_obj_unit_%s");

    this->m_pMenu->addChild(CCNodeConstructor<CCMenuItemSpriteExtra*>()
        .fromNode(CCMenuItemSpriteExtra::create(
            CCNodeConstructor()
                .fromFrameName("GJ_arrow_02_001.png")
                .scale(.5f)
                .done(),
            this,
            (SEL_MenuHandler)&CustomEditMenu::onMoveGroupPage
        ))
        .udata(-1)
        .move(this->m_obPosition.x - this->m_fWidth / 2, 0.f)
        .done()
    );
    this->m_pMenu->addChild(CCNodeConstructor<CCMenuItemSpriteExtra*>()
        .fromNode(CCMenuItemSpriteExtra::create(
            CCNodeConstructor()
                .fromFrameName("GJ_arrow_02_001.png")
                .scale(.5f)
                .flipX()
                .done(),
            this,
            (SEL_MenuHandler)&CustomEditMenu::onMoveGroupPage
        ))
        .udata(1)
        .move(this->m_obPosition.x + this->m_fWidth / 2, 0.f)
        .done()
    );
    this->m_pMenu->addChild(CCNodeConstructor<CCMenuItemSpriteExtra*>()
        .fromNode(CCMenuItemSpriteExtra::create(
            CCNodeConstructor()
                .fromFrameName("GJ_infoIcon_001.png")
                .scale(.6f)
                .flipX()
                .done(),
            this,
            (SEL_MenuHandler)&CustomEditMenu::onInfo
        ))
        .udata(1)
        .move(this->m_obPosition.x + this->m_fWidth / 2 - 10.f, -35.f)
        .done()
    );

    this->m_pMenu->setPosition(0, 60.f);
    this->addChild(this->m_pMenu);

    this->setZOrder(100);

    return true;
}

void CustomEditMenu::onMoveGroupPage(CCObject* pSender) {
    this->m_nMoveGroupPage += as<int>(as<CCNode*>(pSender)->getUserData());

    auto max = this->m_pMoveGroups->count() / 4;

    if (this->m_nMoveGroupPage < 0)
        this->m_nMoveGroupPage = max - 1;
    
    if (this->m_nMoveGroupPage > static_cast<int>(max) - 1)
        this->m_nMoveGroupPage = 0;

    CCARRAY_FOREACH_B_BASE(this->m_pMoveGroups, group, MoveButtonGroup*, ix) {
        group->setVisible(ix / 4 == this->m_nMoveGroupPage);
    }
}

void CustomEditMenu::onInfo(CCObject*) {
    MoveInfoPopup::create()->show();
}

CustomEditMenu::~CustomEditMenu() {
    CC_SAFE_RELEASE(this->m_pMoveGroups);
    CC_SAFE_RELEASE(this->m_pTransformBtns);
}

CustomEditMenu* CustomEditMenu::create(EditorUI* ui) {
    auto ret = new CustomEditMenu;

    if (ret && ret->init(ui)) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}
