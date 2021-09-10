#include "KeybindListView.hpp"
#include "KeybindEditPopup.hpp"
#include "KeybindingsLayer.hpp"
#include "KeybindRepeatPopup.hpp"
#include "MoreKeybindsPopup.hpp"

KeybindCell::KeybindCell(const char* name, CCSize size) :
    TableViewCell(name, size.width, size.height) {}

ButtonSprite* createKeybindBtnSprite(const char* text, bool gold, const char* sprite) {
    auto sprName = "square02_small.png";

    // dumb way to check if sprite exists
    if (CCSprite::create("BE_square_001_small.png"))
        sprName = "BE_square_001_small.png";

    if (sprite) {
        auto spr = ButtonSprite::create(
            CCSprite::createWithSpriteFrameName(sprite),
            18, true, 1.2f, 0, sprName,
            true, 0
        );

        spr->setScale(.45f);
        spr->m_pSubBGSprite->setOpacity(85);
        spr->m_pSubSprite->setScale(1.0f);
        spr->m_pSubSprite->setOpacity(200);

        return spr;
    }

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

    auto name = bind->text.c_str();
    if (!bind->text.size())
        name = m_pBind->name.c_str();

    auto nameLabel = CCLabelBMFont::create(name, "bigFont.fnt");
    nameLabel->limitLabelWidth(140.0f, .5f, .0f);
    nameLabel->setPosition(15.0f, this->m_fHeight / 2);
    nameLabel->setAnchorPoint({ 0.0f, 0.5f });
    if (!m_pBind) {
        nameLabel->setOpacity(180);
        nameLabel->setColor({ 180, 180, 180 });
    } else {
        if (m_pBind->modifier)
            nameLabel->setColor(cc3x(0x8fa));
    }
    this->m_pLayer->addChild(nameLabel);

    m_pMenu = CCMenu::create();
    m_pMenu->setPosition(m_fWidth / 2, m_fHeight / 2);
    this->m_pLayer->addChild(m_pMenu);

    if (m_pItem->text.size()) {
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
    if (m_pItem->text.size()) {
        m_pItem->delegate->m_mFoldedCategories[m_pItem->text] =
            !as<CCMenuItemToggler*>(pSender)->isToggled();

        as<KeybindingsLayer_CB*>(m_pItem->delegate->m_pLayer)->reloadList();
    }
}

void KeybindCell::onEdit(CCObject* pSender) {
    auto item = as<KeybindStoreItem*>(as<CCNode*>(pSender)->getUserObject());

    this->m_pItem->delegate->m_pLayer->detachInput();

    KeybindEditPopup::create(this, item)->show();
}

void KeybindCell::onMore(CCObject* pSender) {
    MoreKeybindsPopup::create(this, as<CCMenuItemSpriteExtra*>(pSender))->show();
}

void KeybindCell::onRepeat(CCObject*) {
    this->m_pItem->delegate->m_pLayer->detachInput();

    KeybindRepeatPopup::create(this)->show();
}

void KeybindCell::onReset(CCObject*) {
    FLAlertLayer::create(
        this,
        "Reset Keybind",
        "Cancel", "Reset",
        "Are you sure you want to <cr>reset</c> <cc>"_s + this->m_pBind->name + "</c>?"
    )->show();
}

void KeybindCell::onSelect(CCObject*) {
    if (this->m_pItem->selectKeybind.key != KEY_None) {
        KeybindManager::get()->clearKeybinds(
            this->m_pItem->type,
            this->m_pBind
        );
        KeybindManager::get()->addKeybind(
            this->m_pItem->type,
            this->m_pBind,
            this->m_pItem->selectKeybind
        );
    }
    this->m_pItem->delegate->m_pLayer->onFinishSelect(nullptr);
}

void KeybindCell::FLAlert_Clicked(FLAlertLayer*, bool btn2) {
    if (btn2) {
        KeybindManager::get()->resetToDefault(
            this->m_pItem->type, this->m_pBind
        );

        this->updateMenu();
    }
}

void KeybindCell::updateMenu() {
    if (!m_pBind)
        return;

    auto x = this->m_fWidth / 2 - 10.0f;

    if (m_pItem->selectMode) {
        for (auto & bind : KeybindManager::get()->getKeybindsForCallback(
            m_pItem->type, m_pBind
        )) 
            if (bind.key == KEY_None) return;
        
        auto spr = createKeybindBtnSprite("Select");
        auto btn = CCMenuItemSpriteExtra::create(
            spr, this, menu_selector(KeybindCell::onSelect)
        );
        auto width = spr->getScaledContentSize().width;
        btn->setPosition(x - width / 2, 0.0f);
        m_pMenu->addChild(btn);

        return;
    }

    auto binds = KeybindManager::get()->getKeybindsForCallback(
        m_pItem->type, m_pBind
    );

    m_pMenu->removeAllChildren();

    bool editable = !binds.size();
    bool resettable = false;
    bool dotDotDot = false;
    for (auto & bind : binds) {
        auto spr = createKeybindBtnSprite(bind.toString().c_str());
        auto width = spr->getScaledContentSize().width;
        if (x - width - 5.0f < 25.0f) {
            spr->release();
            if (!dotDotDot) {
                dotDotDot = true;
                auto sprDot = createKeybindBtnSprite("...");
                auto btn = CCMenuItemSpriteExtra::create(
                    sprDot, this, menu_selector(KeybindCell::onMore)
                );
                auto widthDot = sprDot->getScaledContentSize().width;
                btn->setPosition(x - widthDot / 2, 0.0f);
                btn->setUserObject(new KeybindStoreItem(bind));
                m_pMenu->addChild(btn);

                x -= widthDot + 5.0f;
            }
        } else {
            auto btn = CCMenuItemSpriteExtra::create(
                spr, this, menu_selector(KeybindCell::onEdit)
            );
            btn->setPosition(x - width / 2, 0.0f);
            btn->setUserObject(new KeybindStoreItem(bind));
            m_pMenu->addChild(btn);

            x -= width + 5.0f;
        }

        editable = true;
    
        if (
            std::find(
                m_pBind->defaults.begin(),
                m_pBind->defaults.end(),
                bind
            ) == m_pBind->defaults.end()
        )
            resettable = true;
    }
    if (!binds.size() && m_pBind->defaults.size())
        resettable = true;

    if (!m_pBind->defaults.size())
        resettable = false;

    if (!resettable)
        resettable = !(
            m_pBind->repeat == BetterEdit::getKeybindRepeatEnabled() &&
            m_pBind->repeatInterval == BetterEdit::getKeybindRepeatInterval() &&
            m_pBind->repeatStart == BetterEdit::getKeybindRepeatStart()
        );

    if (editable) {
        if (!dotDotDot) {
            auto spr = createKeybindBtnSprite("+");
            auto btn = CCMenuItemSpriteExtra::create(
                spr, this, menu_selector(KeybindCell::onEdit)
            );
            btn->setPosition(x - spr->getScaledContentSize().width / 2, 0.0f);
            btn->setUserObject(nullptr);
            m_pMenu->addChild(btn);
            x -= spr->getScaledContentSize().width + 5.0f;
        }
    
        if (m_pBind->repeatable) {
            auto spr = createKeybindBtnSprite(nullptr, true, "GJ_timeIcon_001.png");
            if (m_pBind->repeatChanged)
                spr->m_pSubSprite->setColor({ 160, 255, 200 });
            auto btn = CCMenuItemSpriteExtra::create(
                spr, this, menu_selector(KeybindCell::onRepeat)
            );
            btn->setPosition(x - spr->getScaledContentSize().width / 2, 0.0f);
            x -= spr->getScaledContentSize().width + 5.0f;
            m_pMenu->addChild(btn);
        }

        if (resettable) {
            auto spr = createKeybindBtnSprite(nullptr, true, "edit_cwBtn_001.png");
            auto btn = CCMenuItemSpriteExtra::create(
                spr, this, menu_selector(KeybindCell::onReset)
            );
            btn->setPosition(x - spr->getScaledContentSize().width / 2, 0.0f);
            x -= spr->getScaledContentSize().width + 5.0f;
            m_pMenu->addChild(btn);
        }
    }
}

void KeybindCell::updateBGColor(int index) {
    this->m_pBGLayer->setColor({ 0, 0, 0 });
    this->m_pBGLayer->setOpacity(index % 2 ? 120 : 70);
}

KeybindCell* KeybindCell::create(const char* key, CCSize size) {
    auto pRet = new KeybindCell(key, size);

    if (pRet) {
        // mfg

        // this call to autorelease gave me
        // 3 days of endless pain
        // note to self:
        // never call autorelease on list cells

        // pRet->autorelease();
        return pRet;
    }

    CC_SAFE_DELETE(pRet);
    return nullptr;
}


void KeybindListView::setupList() {
    this->m_fItemSeparation = g_fItemHeight;

    this->m_pTableView->reloadData();

    this->m_pTableView->m_fScrollLimitTop = g_fItemHeight *
        (this->m_pTableView->m_pContentLayer->getScaledContentSize().height / 1500.0f);

    // if (this->m_pEntries->count() == 1)
        // this->m_pTableView->moveToTopWithOffset(this->m_fItemSeparation);
    
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
