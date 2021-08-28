#include "KeybindListView.hpp"
#include "KeybindEditPopup.hpp"

KeybindCell::KeybindCell(const char* name, CCSize size) :
    TableViewCell(name, size.width, size.height) {}

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

    this->updateMenu();
}

ButtonSprite* createKeybindBtnSprite(const char* text) {
    auto spr = ButtonSprite::create(
        text, 0, 0,
        "goldFont.fnt", "square02_small.png",
        0, .8f
    );

    spr->m_pBGSprite->setOpacity(85);
    spr->setScale(.6f);

    return spr;
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

    for (auto & bind : binds) {
        auto spr = createKeybindBtnSprite(bind.toString().c_str());
        auto btn = CCMenuItemSpriteExtra::create(
            spr, this, menu_selector(KeybindCell::onEdit)
        );
        auto width = spr->getScaledContentSize().width;
        btn->setPosition(x - width / 2, 0.0f);
        btn->setUserObject(new KeybindStoreItem(bind));
        m_pMenu->addChild(btn);

        x -= width + 5.0f;
    }

    auto spr = createKeybindBtnSprite("+");
    auto btn = CCMenuItemSpriteExtra::create(
        spr, this, menu_selector(KeybindCell::onEdit)
    );
    btn->setPosition(x - spr->getScaledContentSize().width / 2, 0.0f);
    btn->setUserObject(nullptr);
    m_pMenu->addChild(btn);
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
    this->m_pTableView->m_fScrollLimitTop = g_fItemHeight - 8.0f;

    this->m_pRawEntries = CCArray::create();
    // TODO: call release
    this->m_pRawEntries->retain();

    CCARRAY_FOREACH_B(this->m_pEntries, e)
        this->m_pRawEntries->addObject(e);

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

void KeybindListView::textChanged(CCTextInputNode* input) {
    if (input->getString() && strlen(input->getString()))
        this->search(input->getString());
    else
        this->search("");
}

void KeybindListView::search(std::string const& query) {
    m_pEntries->removeAllObjects();

    CCARRAY_FOREACH_B_TYPE(m_pRawEntries, entry, KeybindItem) {
        m_pEntries->addObject(entry);
    }

    if (query.size())
        CCARRAY_FOREACH_B_TYPE(m_pEntries, entry, KeybindItem) {
            if (entry->bind) {
                if (entry->bind->name.find(query) == std::string::npos) {
                    std::cout << "r";
                    m_pEntries->removeObject(entry);
                }
            }
        }
    
    std::cout << "eload: ";
    std::cout << m_pEntries->count() << "\n";

    this->m_pTableView->reloadData();

    if (this->m_pEntries->count() == 1)
        this->m_pTableView->moveToTopWithOffset(this->m_fItemSeparation);
    
    this->m_pTableView->moveToTop();
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
