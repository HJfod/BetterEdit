#include "KeybindListView.hpp"
#include "KeybindEditPopup.hpp"
#include "KeybindingsLayer.hpp"

KeybindCell::KeybindCell(const char* name, CCSize size) :
    TableViewCell(name, size.width, size.height) {}

ButtonSprite* createKeybindBtnSprite(const char* text, bool gold = true) {
    auto sprName = "square02_small.png";

    // dumb way to check if sprite exists
    if (CCSprite::create("BE_square_001_small.png"))
        sprName = "BE_square_001_small.png";

    auto spr = ButtonSprite::create(
        text, gold ? 0 : 18, !gold,
        gold ? "goldFont.fnt" : "bigFont.fnt", sprName,
        0, gold ? .8f : .6f
    );

    spr->m_pBGSprite->setOpacity(85);
    spr->setScale(.6f);

    return spr;
}

void KeybindCell::loadFromItem(KeybindItem* bind) {
    m_pItem = bind;
    m_pBind = bind->bind;

    m_pBGLayer->setOpacity(255);

    auto name = bind->text;
    if (!name)
        name = m_pBind->name.c_str();

    auto nameLabel = CCLabelBMFont::create(name, "bigFont.fnt");
    nameLabel->limitLabelWidth(260.0f, .5f, .0f);
    nameLabel->setPosition(15.0f, this->m_fHeight / 2);
    nameLabel->setAnchorPoint({ 0.0f, 0.5f });
    if (!m_pBind) {
        nameLabel->setOpacity(180);
        nameLabel->setColor({ 180, 180, 180 });
    }
    this->m_pLayer->addChild(nameLabel);

    m_pMenu = CCMenu::create();
    m_pMenu->setPosition(m_fWidth / 2, m_fHeight / 2);
    this->m_pLayer->addChild(m_pMenu);

    if (m_pItem->delegate) {
        auto foldBtn = CCMenuItemToggler::create(
            createKeybindBtnSprite("-", false),
            createKeybindBtnSprite("+", false),
            this,
            menu_selector(KeybindCell::onFold)
        );
        foldBtn->toggle(m_pItem->delegate->m_mFoldedCategories[m_pItem->text]);
        foldBtn->setPosition(m_fWidth / 2 - 15.0f, 0.0f);
        this->m_pMenu->addChild(foldBtn);
    }

    this->updateMenu();
}

void KeybindCell::onFold(CCObject* pSender) {
    if (m_pItem->delegate) {
        m_pItem->delegate->m_mFoldedCategories[m_pItem->text] =
            !as<CCMenuItemToggler*>(pSender)->isToggled();

        as<KeybindingsLayer_CB*>(m_pItem->delegate->m_pLayer)->reloadList();
    }
}

void KeybindCell::onEdit(CCObject* pSender) {
    auto item = as<KeybindStoreItem*>(as<CCNode*>(pSender)->getUserObject());

    KeybindEditPopup::create(this, item)->show();
}

void KeybindCell::updateMenu() {
    if (!m_pBind)
        return;

    auto binds = KeybindManager::get()->getKeybindsForCallback(
        m_pItem->type, m_pBind
    );

    m_pMenu->removeAllChildren();

    auto x = this->m_fWidth / 2 - 10.0f;

    bool editable = false;
    for (auto & bind : binds) {
        // cba to fix Swipe Modifier rn
        if (bind.key == KEY_None) {
            auto label = CCLabelBMFont::create(bind.toString().c_str(), "goldFont.fnt");
            label->setScale(.5f);
            label->setPosition(
                m_fWidth - label->getScaledContentSize().width / 2 - 10.0f,
                m_fHeight / 2
            );
            m_pLayer->addChild(label);
        } else {
            auto spr = createKeybindBtnSprite(bind.toString().c_str());
            auto btn = CCMenuItemSpriteExtra::create(
                spr, this, menu_selector(KeybindCell::onEdit)
            );
            auto width = spr->getScaledContentSize().width;
            btn->setPosition(x - width / 2, 0.0f);
            btn->setUserObject(new KeybindStoreItem(bind));
            m_pMenu->addChild(btn);

            x -= width + 5.0f;

            editable = true;
        }
    }

    if (editable) {
        auto spr = createKeybindBtnSprite("+");
        auto btn = CCMenuItemSpriteExtra::create(
            spr, this, menu_selector(KeybindCell::onEdit)
        );
        btn->setPosition(x - spr->getScaledContentSize().width / 2, 0.0f);
        btn->setUserObject(nullptr);
        m_pMenu->addChild(btn);
    }
}

void KeybindCell::updateBGColor(int index) {
    this->m_pBGLayer->setColor({ 0, 0, 0 });
    this->m_pBGLayer->setOpacity(index % 2 ? 120 : 70);
}

KeybindCell* KeybindCell::create(const char* key, CCSize size) {
    auto pRet = new KeybindCell(key, size);

    if (pRet) {
        pRet->autorelease();
        return pRet;
    }

    CC_SAFE_DELETE(pRet);
    return nullptr;
}


void KeybindListView::setupList() {
    this->m_fItemSeparation = g_fItemHeight;
    this->m_pTableView->m_fScrollLimitTop = g_fItemHeight -
        min(1.0f, 9.f / this->m_pEntries->count()) * g_fItemHeight;

    this->m_pTableView->reloadData();

    if (this->m_pEntries->count() == 1)
        this->m_pTableView->moveToTopWithOffset(this->m_fItemSeparation);
    
    this->m_pTableView->moveToTop();
}

TableViewCell* KeybindListView::getListCell(const char* key) {
    return KeybindCell::create(key, { this->m_fWidth, this->m_fItemSeparation });
}

void KeybindListView::loadCell(TableViewCell* cell, unsigned int index) {
    as<KeybindCell*>(cell)->loadFromItem(
        as<KeybindItem*>(this->m_pEntries->objectAtIndex(index))
    );
    as<KeybindCell*>(cell)->updateBGColor(index);
}

KeybindListView* KeybindListView::create(CCArray* binds, float width, float height) {
    auto pRet = new KeybindListView;

    if (pRet && pRet->init(binds, kBoomListType_Keybind, width, height)) {
        pRet->autorelease();
        return pRet;
    }

    CC_SAFE_DELETE(pRet);
    return nullptr;
}
