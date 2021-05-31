#include "TemplateMenu.hpp"

using namespace cocos2d;
using namespace cocos2d::extension;
using namespace gd;
using namespace gdmake;

CCSprite* makeEditSideBtn(const char* sprName) {
    auto bg = CCSprite::createWithSpriteFrameName("GJ_colorBtn_001.png");
    bg->setOpacity(100);

    auto aspr = CCSprite::createWithSpriteFrameName(sprName);

    aspr->setPosition(bg->getContentSize() / 2);
    aspr->setScale(.825f);

    bg->addChild(aspr);

    bg->setScale(.9f);

    return bg;
}

CCSprite* makeEditBtn_spr(CCSprite* spr) {
    auto bg = CCSprite::create("GJ_button_05.png");

    spr->setPosition(bg->getContentSize() / 2);
    spr->setScale(.825f);
    bg->addChild(spr);

    bg->setScale(.75f);

    bg->setContentSize(bg->getScaledContentSize());

    return bg;
}

CCSprite* makeEditBtn(TBlock const& temp) {
    auto bg = CCSprite::create("GJ_button_05.png");

    for (uint8_t ix = 0u; ix < 4u; ix++) {
        if (ix == 0u && !(temp.edges & temp.eLeft))
            continue;
        if (ix == 1u && !(temp.edges & temp.eTop))
            continue;
        if (ix == 2u && !(temp.edges & temp.eRight))
            continue;
        if (ix == 3u && !(temp.edges & temp.eBottom))
            continue;

        auto aspr = CCSprite::createWithSpriteFrameName("blockOutline_05_001.png");

        aspr->setPosition(bg->getContentSize() / 2);
        aspr->setScale(.825f);
        aspr->setRotation(static_cast<float>(90 * ix));

        bg->addChild(aspr);
    }

    bg->setScale(.75f);

    return bg;
}

TemplateButton* TemplateButton::create(
    TBlock const& block,
    TemplateMenu* target,
    bool isVariant
) {
    auto ret = new TemplateButton();
    auto spr = makeEditBtn(block);

    if (ret && ret->initWithNormalSprite(
        spr, nullptr, nullptr,
        target,
        isVariant ?
            (SEL_MenuHandler)&TemplateMenu::onSaveBlockVariant :
            (SEL_MenuHandler)&TemplateMenu::onSaveBlock
    ) && ret->CCMenuItemSpriteExtra::init(spr)) {
        ret->m_pSpr = spr;
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}

void alignItemsToGrid(std::vector<CCMenuItemSpriteExtra*> items, float padding, unsigned int rowmax, cocos2d::CCPoint pos) {
    auto vmenu = CCMenu::create();

    for (auto ix = 0u; ix < static_cast<unsigned int>(ceil(items.size() / static_cast<double>(rowmax))); ix++) {
        auto hmenu = CCMenu::create();

        for (auto i = ix * rowmax; i < rowmax * ix + rowmax; i++)
            if (i < items.size())
                hmenu->addChild(items[i]);
        
        hmenu->alignItemsHorizontallyWithPadding(padding);
        hmenu->setContentSize(items[ix]->getScaledContentSize());

        vmenu->addChild(hmenu);
    }

    vmenu->alignItemsVerticallyWithPadding(padding);

    auto ix = 0u;
    for (auto item : items) {
        item->setPosition(
            item->getPositionX() + pos.x,
            item->getParent()->getPositionY() + pos.y
        );
        item->removeFromParent();
    }

    vmenu->release();
}

struct TemplateVariantObject : public cocos2d::CCObject {
    std::vector<TBlock> m_vBlocks;
};

bool TemplateMenu::init(CCArray* buttons, int idk, bool idkb, int rowCount, int columnCount, CCPoint pos) {
    if (!gd::EditButtonBar::init(buttons, idk, idkb, rowCount, columnCount, pos))
        return false;
    
    this->m_nMode = TMode::kTModeNormal;
    this->m_obSize = CCSize { 200.0f, 60.0f };
    
    this->m_pEditMenu = CCMenu::create();
    this->m_pEditMenu->setPosition(this->m_obPosition);

    this->setupEditMenu();

    this->addChild(this->m_pEditMenu);

    this->m_pEditMenu->setVisible(false);

    this->m_pVariantsMenu = CCMenu::create();
    this->m_pVariantsMenu->setPosition(
        this->m_obPosition.x, this->m_obPosition.y - offsetTop
    );

    auto variantsTitle = CCLabelBMFont::create("Edit Variants", "goldFont.fnt");
    variantsTitle->setPosition(0, offsetTop - 5.0f);
    variantsTitle->setScale(.6f);
    this->m_pVariantsMenu->addChild(variantsTitle);

    this->m_pVariantsBtns = CCMenu::create();
    this->m_pVariantsBtns->setPosition(0, 0);

    auto variantsCloseSpr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    variantsCloseSpr->setScale(.5f);

    this->m_pVariantsCloseBtn = CCMenuItemSpriteExtra::create(
        variantsCloseSpr,
        this,
        (SEL_MenuHandler)&TemplateMenu::onCloseVariantMenu
    );

    this->m_pVariantsMenu->addChild(this->m_pVariantsBtns);
    this->m_pVariantsMenu->addChild(this->m_pVariantsCloseBtn);
    this->m_pVariantsMenu->setVisible(false);

    this->addChild(this->m_pVariantsMenu, 999);
    
    return true;
}

void TemplateMenu::onSaveBlock(cocos2d::CCObject* pSender) {
    this->onSaveBlockVariant(pSender);
}

void TemplateMenu::onSaveBlockVariant(cocos2d::CCObject* pSender) {
    auto ui = GameManager::sharedState()->getEditorLayer()->getEditorUI();
    auto sel = ui->getSelectedObjects();

    if (sel->count()) {
        std::string saveStr = "";

        auto centre = ui->getGroupCenter(sel, true);

        ui->deselectAll();

        auto spr = CCSprite::create();
        for (auto ix = 0u; ix < sel->count(); ix++) {
            saveStr += extra::as<GameObject*>(sel->objectAtIndex(ix))->getSaveString() + ";";

            auto o_obj = extra::as<GameObject*>(sel->objectAtIndex(ix));
            auto obj = new GameObject(*o_obj);
            obj->setPosition(obj->getPosition() - centre);
            std::cout << obj->getRealPosition().x << ", " << obj->getRealPosition().y << "\n";
            obj->setRealPosition(obj->getRealPosition() - centre);
            std::cout << obj->getRealPosition().x << ", " << obj->getRealPosition().y << "\n";
            spr->addChild(obj);
        }

        std::cout << spr->getPositionX() << "; " << spr->getPositionY() << "\n";

        extra::as<CCMenuItemSpriteExtra*>(pSender)
            ->setNormalImage(makeEditBtn_spr(spr));
    } else
        FLAlertLayer::create(
            nullptr,
            "No Objects Selected",
            "OK", nullptr,
            "No objects selected! <cr>Select</c> some objects to create a template."
        )->show();
}

void TemplateMenu::onCloseVariantMenu(cocos2d::CCObject*) {
    this->switchMode(TMode::kTModeEdit);
}

void TemplateMenu::onShowVariantMenu(cocos2d::CCObject* pSender) {
    this->showVariantMenu(
        extra::as<TemplateVariantObject*>(
            extra::as<CCNode*>(pSender)->getUserObject()
        )->m_vBlocks
    );
}

void TemplateMenu::onSaveEdit(CCObject* pSender) {
    this->switchMode(TMode::kTModeNormal);
}

void TemplateMenu::onCancelEdit(CCObject* pSender) {
    this->switchMode(TMode::kTModeNormal);
}

void TemplateMenu::showVariantMenu(std::vector<TBlock> variants) {
    this->m_pVariantsBtns->removeAllChildrenWithCleanup(true);

    float size = 0.0f;
    for (auto v : variants) {
        auto btn = CCMenuItemSpriteExtra::create(
            makeEditBtn(v),
            this,
            (SEL_MenuHandler)&TemplateMenu::onSaveBlockVariant
        );

        this->m_pVariantsBtns->addChild(btn);

        size += btn->getScaledContentSize().width + 5.0f;
    }
    
    this->m_pVariantsBtns->alignItemsHorizontallyWithPadding(5.0f);

    this->m_pVariantsCloseBtn->setPosition(
        - size / 2 - 25.0f, 0.0f
    );

    this->switchMode(TMode::kTModeEditVariants);
}

void TemplateMenu::setupEditMenu() {
    auto label = CCLabelBMFont::create("Create Template", "goldFont.fnt");
    label->setPosition(0, -5.0f);
    label->setScale(.55f);
    this->m_pEditMenu->addChild(label);

    auto saveBtn = CCMenuItemSpriteExtra::create(
        makeEditSideBtn("GJ_completesIcon_001.png"),
        this,
        (SEL_MenuHandler)&TemplateMenu::onSaveEdit
    );
    saveBtn->setPosition(165.0f, -60.0f);
    this->m_pEditMenu->addChild(saveBtn);

    auto cancelBtn = CCMenuItemSpriteExtra::create(
        makeEditSideBtn("GJ_deleteIcon_001.png"),
        this,
        (SEL_MenuHandler)&TemplateMenu::onCancelEdit
    );
    cancelBtn->setPosition(165.0f, -20.0f);
    this->m_pEditMenu->addChild(cancelBtn);

    std::vector<CCMenuItemSpriteExtra*> btns;

    for (auto temp : std::vector<std::vector<uint8_t>> {
        { TBlock::eLeft | TBlock::eTop | TBlock::eRight | TBlock::eBottom },
        { TBlock::eLeft, TBlock::eTop, TBlock::eRight, TBlock::eBottom, },
        {
            TBlock::eLeft | TBlock::eTop,
            TBlock::eTop | TBlock::eRight,
            TBlock::eRight | TBlock::eBottom,
            TBlock::eLeft | TBlock::eBottom,
        },
        { TBlock::eLeft | TBlock::eRight, TBlock::eTop | TBlock::eBottom, },
        {
            TBlock::eLeft | TBlock::eTop | TBlock::eRight,
            TBlock::eTop | TBlock::eRight | TBlock::eBottom,
            TBlock::eRight | TBlock::eBottom | TBlock::eLeft,
            TBlock::eBottom | TBlock::eLeft | TBlock::eTop,
        },
    }) {
        auto btn = CCMenuItemSpriteExtra::create(
            makeEditBtn(TBlock { temp[0] }),
            this,
            (SEL_MenuHandler)&TemplateMenu::onSaveBlock
        );

        btns.push_back(btn);

        if (temp.size() > 1) {
            auto menu = CCMenu::create();

            auto plusSpr = CCSprite::createWithSpriteFrameName("GJ_chatBtn_02_001.png");
            plusSpr->setScale(.5f);

            auto vobj = new TemplateVariantObject();

            for (auto t : temp)
                vobj->m_vBlocks.push_back(TBlock { t });
            vobj->autorelease();

            auto plusBtn = CCMenuItemSpriteExtra::create(
                plusSpr,
                this,
                (SEL_MenuHandler)&TemplateMenu::onShowVariantMenu
            );

            plusBtn->setUserObject(vobj);

            menu->addChild(plusBtn);

            menu->setPosition(btn->getScaledContentSize());

            btn->addChild(menu);
        }
    }

    alignItemsToGrid(btns, 7.5f, 7u, { 0.0f, -offsetTop });

    for (auto btn : btns)
        this->m_pEditMenu->addChild(btn);
}

void TemplateMenu::switchMode(TemplateMenu::TMode mode) {
    this->m_nMode = mode;

    this->m_pScrollLayer->setVisible(mode == TMode::kTModeNormal);
    this->m_pEditMenu->setVisible(mode == TMode::kTModeEdit);
    this->m_pVariantsMenu->setVisible(mode == TMode::kTModeEditVariants);
}

TemplateMenu* TemplateMenu::create(CCArray* buttons, CCPoint pos, int idk, bool idkb, int rowCount, int columnCount) {
    auto ret = new TemplateMenu();

    if (ret && ret->init(buttons, idk, idkb, rowCount, columnCount, pos)) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}
