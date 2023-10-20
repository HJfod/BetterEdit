#include "CustomEditMenu.hpp"
#include "other/KeybindUtils.hpp"
#include "MoveInfoPopup.hpp"

#define ADD_GROUP_KEYBINDS(_bind_, _name_, _desc_)                  \
    BindManager::get()->registerBindable(BindableAction(            \
        CCString::createWithFormat(_bind_, "down")->getCString(),   \
        CCString::createWithFormat(_name_, "Down")->getCString(),   \
        CCString::createWithFormat(_desc_, "Down")->getCString(),   \
        {},                                                         \
        Category::EDITOR_MOVE                                       \
    ), "robtop.geometry-dash/move-obj-down-small");                 \
                                                                    \
    BindManager::get()->registerBindable(BindableAction(            \
        CCString::createWithFormat(_bind_, "up")->getCString(),     \
        CCString::createWithFormat(_name_, "Up")->getCString(),     \
        CCString::createWithFormat(_desc_, "Up")->getCString(),     \
        {},                                                         \
        Category::EDITOR_MOVE                                       \
    ), "robtop.geometry-dash/move-obj-down-small");                 \
                                                                    \
    BindManager::get()->registerBindable(BindableAction(            \
        CCString::createWithFormat(_bind_, "right")->getCString(),  \
        CCString::createWithFormat(_name_, "Right")->getCString(),  \
        CCString::createWithFormat(_desc_, "Right")->getCString(),  \
        {},                                                         \
        Category::EDITOR_MOVE                                       \
    ), "robtop.geometry-dash/move-obj-down-small");                 \
                                                                    \
    BindManager::get()->registerBindable(BindableAction(            \
        CCString::createWithFormat(_bind_, "left")->getCString(),   \
        CCString::createWithFormat(_name_, "Left")->getCString(),   \
        CCString::createWithFormat(_desc_, "Left")->getCString(),   \
        {},                                                         \
        Category::EDITOR_MOVE                                       \
    ), "robtop.geometry-dash/move-obj-down-small");                 \

#define ADD_MOVE_GROUP(_size_, _spr_, _text_, _scale_, _bind_)      \
    addMoveGroup(                                                   \
        CustomEditCommand::##_size_##Up,                            \
        CustomEditCommand::##_size_##Down,                          \
        CustomEditCommand::##_size_##Left,                          \
        CustomEditCommand::##_size_##Right,                         \
        _spr_, _text_, _scale_, _bind_                              \
    );                                                              \

bool MoveButtonGroup::init() {
    m_pButtons = CCArray::create();
    m_pButtons->retain();

    return true;
}

MoveButtonGroup::~MoveButtonGroup() {
    m_pButtons->release();
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
    m_pButtons->addObject(btn);
}

void MoveButtonGroup::setVisible(bool visible) {
    CCObject* btn;
    CCARRAY_FOREACH(m_pButtons, btn)
        as<CCMenuItemSpriteExtra*>(btn)->setVisible(visible);
}

// I hate that I have to do it like this
class $modify(CustomEditMenu, EditButtonBar) {
    float m_fWidth;
    EditorUI* m_pUI;
    CCMenu* m_pMenu;
    CCArray* m_pMoveGroups;
    CCArray* m_pTransformBtns;
    int m_nMoveGroupPage = 0;

    CCMenuItemSpriteExtra* createButton(
        const char* spr,
        const char* sizeTxt,
        CustomEditCommand com,
        float scale,
        SEL_MenuHandler cb,
        const char* keybind
    ) {
        auto btn = m_fields->m_pUI->getSpriteButton(
            spr, cb, nullptr, .9f
        );
        btn->setTag(as<int>(com));

        if (sizeTxt) {
            auto label = CCLabelBMFont::create(sizeTxt, "bigFont.fnt");
            label->setScale(.35f);
            label->setZOrder(50);
            label->setPosition(btn->getContentSize().width / 2, 11.0f);
            btn->addChild(label);
        }

        as<ButtonSprite*>(btn->getNormalImage())->m_subSprite->setScale(scale);

        btn->getNormalImage()->setScale(.583f);

        if (keybind)
            KEYBIND_FOR_BUTTON(btn, keybind);

        return btn;
    }

    void addMoveGroup(
        CustomEditCommand up, CustomEditCommand down,
        CustomEditCommand left, CustomEditCommand right,
        const char* sprNum, const char* sprText, float sprScale,
        const char* keybind
    ) {
        auto winSize = CCDirector::sharedDirector()->getWinSize();

        auto center = CCPoint {
            (m_fields->m_fWidth - 20.f) / 4.f
                * (m_fields->m_pMoveGroups->count() % 4 - 1.5f) +
                winSize.width / 2,
            0.0f
        };

        auto group = MoveButtonGroup::create();

        auto upBtn = createButton(
            CCString::createWithFormat("edit_upBtn%s_001.png", sprNum)->getCString(),
            sprText, up, sprScale, menu_selector(EditorUI::moveObjectCall),
            keybind ? CCString::createWithFormat(keybind, "up")->getCString() : nullptr
        );
        upBtn->setPosition(center + CCPoint { 0.f, 12.f });
        upBtn->setVisible(m_fields->m_pMoveGroups->count() < 4);
        m_fields->m_pMenu->addChild(upBtn);
        group->addButton(upBtn);

        auto downBtn = createButton(
            CCString::createWithFormat("edit_downBtn%s_001.png", sprNum)->getCString(),
            sprText, down, sprScale, menu_selector(EditorUI::moveObjectCall),
            keybind ? CCString::createWithFormat(keybind, "down")->getCString() : nullptr
        );
        downBtn->setPosition(center + CCPoint { 0.f, -12.f });
        downBtn->setVisible(m_fields->m_pMoveGroups->count() < 4);
        m_fields->m_pMenu->addChild(downBtn);
        group->addButton(downBtn);

        auto leftBtn = createButton(
            CCString::createWithFormat("edit_leftBtn%s_001.png", sprNum)->getCString(),
            sprText, left, sprScale, menu_selector(EditorUI::moveObjectCall),
            keybind ? CCString::createWithFormat(keybind, "left")->getCString() : nullptr
        );
        leftBtn->setPosition(center + CCPoint { -24.f, 0.f });
        leftBtn->setVisible(m_fields->m_pMoveGroups->count() < 4);
        m_fields->m_pMenu->addChild(leftBtn);
        group->addButton(leftBtn);

        auto rightBtn = createButton(
            CCString::createWithFormat("edit_rightBtn%s_001.png", sprNum)->getCString(),
            sprText, right, sprScale, menu_selector(EditorUI::moveObjectCall),
            keybind ? CCString::createWithFormat(keybind, "right")->getCString() : nullptr
        );
        rightBtn->setPosition(center + CCPoint { 24.f, 0.f });
        rightBtn->setVisible(m_fields->m_pMoveGroups->count() < 4);
        m_fields->m_pMenu->addChild(rightBtn);
        group->addButton(rightBtn);

        m_fields->m_pMoveGroups->addObject(group);
    }

    void addTransformButton(
        CustomEditCommand command, const char* spr, bool addToDict,
        const char* keybind = nullptr,
        SEL_MenuHandler cb = menu_selector(EditorUI::transformObjectCall)
    ) {
        auto winSize = CCDirector::sharedDirector()->getWinSize();

        auto btn = createButton(
            spr, nullptr, command, 1.0f, cb, keybind
        );
        if (addToDict) {
            m_fields->m_pUI->m_editButtonDict->setObject(
                btn, CCString::createWithFormat("%i", command)->getCString()
            );
        }
        btn->setPositionY(-40.0f);
        m_fields->m_pMenu->addChild(btn);
        m_fields->m_pTransformBtns->addObject(btn);

        int ix = 0;
        CCObject* btnObj;
        CCARRAY_FOREACH(m_fields->m_pTransformBtns, btnObj) {
            auto len = 28.0f;
            as<CCMenuItemSpriteExtra*>(btnObj)->setPositionX(
                len * ((ix++) - m_fields->m_pTransformBtns->count() / 2.f) +
                winSize.width / 2
            );
        }
    }

    void onMoveGroupPage(CCObject* pSender) {
        m_fields->m_nMoveGroupPage += reinterpret_cast<int>(as<CCNode*>(pSender)->getUserData());

        auto max = m_fields->m_pMoveGroups->count() / 4;

        if (m_fields->m_nMoveGroupPage < 0)
            m_fields->m_nMoveGroupPage = max - 1;
        
        if (m_fields->m_nMoveGroupPage > static_cast<int>(max) - 1)
            m_fields->m_nMoveGroupPage = 0;

        CCObject* obj;
        int ix = 0;
        CCARRAY_FOREACH(m_fields->m_pMoveGroups, obj) 
            as<MoveButtonGroup*>(obj)->setVisible((ix++) / 4 == m_fields->m_nMoveGroupPage);
    }

    bool init(EditorUI* ui) {
        if (!EditButtonBar::init(
            CCArray::create(),
            0, false,
            GameManager::sharedState()->getIntGameVariable("0049"),
            GameManager::sharedState()->getIntGameVariable("0050"),
            { CCDirector::sharedDirector()->getWinSize().width / 2, 86.0f }
        ))
            return false;
        
        m_fields->m_pUI = ui;
        m_fields->m_pMenu = CCMenu::create();
        m_fields->m_pMoveGroups = CCArray::create();
        m_fields->m_pMoveGroups->retain();
        m_fields->m_pTransformBtns = CCArray::create();
        m_fields->m_pTransformBtns->retain();

        m_fields->m_fWidth = CCDirector::sharedDirector()->getWinSize().width - 240.0f;

        addTransformButton(
            CustomEditCommand::FlipX, "edit_flipXBtn_001.png", true
        );
        addTransformButton(
            CustomEditCommand::FlipY, "edit_flipYBtn_001.png", true
        );
        addTransformButton(
            CustomEditCommand::RotateCW, "edit_cwBtn_001.png", true
        );
        addTransformButton(
            CustomEditCommand::RotateCCW, "edit_ccwBtn_001.png", true
        );
        addTransformButton(
            CustomEditCommand::RotateCW45, "edit_rotate45rBtn_001.png", true,
            "rotate-45-cw"_spr
        );
        addTransformButton(
            CustomEditCommand::RotateCCW45, "edit_rotate45lBtn_001.png", true,
            "rotate-45-ccw"_spr
        );
        addTransformButton(
            CustomEditCommand::RotateFree, "edit_freeRotateBtn_001.png", true,
            nullptr,
            menu_selector(EditorUI::activateRotationControl)
        );
        addTransformButton(
            CustomEditCommand::RotateSnap, "edit_rotateSnapBtn_001.png", true,
            "rotate-snap"_spr
        );
        addTransformButton(
            CustomEditCommand::Scale, "edit_scaleBtn_001.png", true,
            "how-scale-control"_spr,
            menu_selector(EditorUI::activateScaleControl)
        );

        ADD_MOVE_GROUP(Small,   "",  nullptr, 1.0f, nullptr); // Don't double up on robtop's keybinds
        ADD_MOVE_GROUP(,        "2", nullptr, 1.0f, nullptr);
        ADD_MOVE_GROUP(Tiny,    "",  nullptr, .8f,  "move-obj-%s-tiny"_spr);
        ADD_MOVE_GROUP(Big,     "3", nullptr, 1.0f, "move-obj-%s-big"_spr);
        ADD_MOVE_GROUP(Half,    "2", "1/2",   1.0f, "move-obj-%s-half"_spr);
        ADD_MOVE_GROUP(Quarter, "2", "1/4",   1.0f, "move-obj-%s-quarter"_spr);
        ADD_MOVE_GROUP(Eigth,   "2", "1/8",   1.0f, "move-obj-%s-eigth"_spr);
        ADD_MOVE_GROUP(QUnit,   "",  "1/2",   .8f,  "move-obj-%s-unit"_spr);

        auto winSize = CCDirector::sharedDirector()->getWinSize();
        
        CCSprite* rightSprite = CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");
        rightSprite->setScale(0.5f);
        rightSprite->setFlipX(true);

        CCMenuItemSpriteExtra* rightBtn = CCMenuItemSpriteExtra::create(rightSprite, this, (SEL_MenuHandler)&CustomEditMenu::onMoveGroupPage);
        rightBtn->setUserData(reinterpret_cast<void*>(1));
        rightBtn->setPosition(winSize.width / 2 + (m_fields->m_fWidth) / 2.f, 0.0f);
        m_fields->m_pMenu->addChild(rightBtn);

        CCSprite* leftSprite = CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");
        leftSprite->setScale(0.5f);

        CCMenuItemSpriteExtra* leftBtn = CCMenuItemSpriteExtra::create(leftSprite, this, (SEL_MenuHandler)&CustomEditMenu::onMoveGroupPage);
        leftBtn->setUserData(reinterpret_cast<void*>(-1));
        leftBtn->setPosition(winSize.width / 2 - (m_fields->m_fWidth) / 2.f, 0.0f);
        m_fields->m_pMenu->addChild(leftBtn);
        
        CCSprite* infoSprite = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
        infoSprite->setScale(0.6f);
        infoSprite->setFlipX(true);

        CCMenuItemSpriteExtra* infoBtn = CCMenuItemSpriteExtra::create(infoSprite, this, (SEL_MenuHandler)&CustomEditMenu::onInfo);
        infoBtn->setUserData(reinterpret_cast<void*>(1));
        infoBtn->setPosition(winSize.width / 2 + (m_fields->m_fWidth) / 2.f - 10.f, -35.f);

        m_fields->m_pMenu->addChild(infoBtn);

        m_fields->m_pMenu->setPosition(0, 60.f);
        addChild(m_fields->m_pMenu);

        setZOrder(100);

        return true;
    }

    void onInfo(CCObject*) {
        TODO: MoveInfoPopup::create()->show();
    }

    static CustomEditMenu* create(EditorUI* ui) {
        auto ret = new CustomEditMenu;

        if (ret && ret->init(ui)) {
            ret->autorelease();
            return ret;
        }

        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};

class $modify(EditorUI) {
    void createMoveMenu() {
        m_editButtonDict = CCDictionary::create();
        m_editButtonDict->retain();
        m_editButtonBar = CustomEditMenu::create(this);
        addChild(m_editButtonBar);
    }
};

$execute {
    ADD_GROUP_KEYBINDS("move-obj-%s-unit"_spr, "Move Object %s Quarter of a Unit", "Moves the Object %s Quarter a Unit");
    ADD_GROUP_KEYBINDS("move-obj-%s-eigth"_spr, "Move Object %s One-Eigth", "Moves the Object %s One-Eigth of a Standard Block");
    ADD_GROUP_KEYBINDS("move-obj-%s-quarter"_spr, "Move Object %s One-Quarter", "Moves the Object %s One-Quarter of a Standard Block");
    ADD_GROUP_KEYBINDS("move-obj-%s-half"_spr, "Move Object %s Half", "Moves the Object %s Half of a Standard Block");
    ADD_GROUP_KEYBINDS("move-obj-%s-big"_spr, "Move Object %s Big", "Moves the Object %s 5 Blocks");
    ADD_GROUP_KEYBINDS("move-obj-%s-tiny"_spr, "Move Object %s Tiny", "Moves the Object %s Half a Unit");
}