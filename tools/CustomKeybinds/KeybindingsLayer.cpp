#include "../../BetterEdit.hpp"
#include "../CustomKeybinds/KeybindManager.hpp"
#include <InputNode.hpp>

class KeybindingsLayer : public FLAlertLayer {
    public:
        int m_nCurrentPage;
        int m_nItemCount;
        int m_nPageCount;
        cocos2d::CCDictionary* m_pPages;
        cocos2d::CCDictionary* m_pUnused;
        cocos2d::CCNode* m_pLeftArrow;
        cocos2d::CCNode* m_pRightArrow;

        void onClose(CCObject* pSender) {
            reinterpret_cast<void(__thiscall*)(KeybindingsLayer*, CCObject*)>(
                base + 0x49c60
            )(this, pSender);
        }

        void onPrevPage(CCObject* pSender) {
            reinterpret_cast<void(__thiscall*)(KeybindingsLayer*, CCObject*)>(
                base + 0x153cd0
            )(this, pSender);
        }

        void onNextPage(CCObject* pSender) {
            reinterpret_cast<void(__thiscall*)(KeybindingsLayer*, CCObject*)>(
                base + 0x153cc0
            )(this, pSender);
        }

        void goToPage(int page) {
            reinterpret_cast<void(__thiscall*)(KeybindingsLayer*, int)>(
                base + 0x153ce0
            )(this, page);
        }
};

static constexpr const float g_fItemHeight = 27.5f;
static constexpr const BoomListType kBoomListType_Keybind
    = static_cast<BoomListType>(0x421);

struct KeybindItem : public CCObject {
    KeybindCallback* bind;
    const char* text;

    KeybindItem(KeybindCallback* b) {
        bind = b;
        text = nullptr;
        this->autorelease();
    }

    KeybindItem(const char* t) {
        bind = nullptr;
        text = t;
        this->autorelease();
    }
};

class KeybindCell : public TableViewCell {
    protected:
        KeybindCallback* m_pBind;

		KeybindCell(const char* name, CCSize size) :
	        TableViewCell(name, size.width, size.height) {}

    public:
        void loadFromItem(KeybindItem* bind) {
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
        }

        void updateBGColor(int index) {
            this->m_pBGLayer->setColor({ 0, 0, 0 });
            this->m_pBGLayer->setOpacity(index % 2 ? 120 : 70);
        }

		static KeybindCell* create(const char* key, CCSize size) {
            auto pRet = new KeybindCell(key, size);

            if (pRet) {
                pRet->autorelease();
                return pRet;
            }

            CC_SAFE_DELETE(pRet);
            return nullptr;
        }
};

class KeybindListView : public CustomListView {
    protected:
        void setupList() override {
            this->m_fItemSeparation = g_fItemHeight;
            this->m_pTableView->m_fScrollLimitTop = g_fItemHeight - 10.0f;

            this->m_pTableView->reloadData();

            if (this->m_pEntries->count() == 1)
                this->m_pTableView->moveToTopWithOffset(this->m_fItemSeparation);
            
            this->m_pTableView->moveToTop();
        }
        
        TableViewCell* getListCell(const char* key) override {
            return KeybindCell::create(key, { this->m_fWidth, this->m_fItemSeparation });
        }
        
        void loadCell(TableViewCell* cell, unsigned int index) override {
            as<KeybindCell*>(cell)->loadFromItem(
                as<KeybindItem*>(this->m_pEntries->objectAtIndex(index))
            );
            as<KeybindCell*>(cell)->updateBGColor(index);
        }
    
    public:
        static KeybindListView* create(CCArray* binds, float width, float height) {
            auto pRet = new KeybindListView;

            if (pRet && pRet->init(binds, kBoomListType_Keybind, width, height)) {
                pRet->autorelease();
                return pRet;
            }

            CC_SAFE_DELETE(pRet);
            return nullptr;
        }
};

GDMAKE_HOOK(0x153670)
CCLabelBMFont* __fastcall KeybindingsLayer_addKeyPair(
    KeybindingsLayer* self, char* key, char* combo
) {
    return nullptr;
}

GDMAKE_HOOK(0x153cc0)
void __fastcall KeybindingsLayer_onNextPage(
    KeybindingsLayer* self, edx_t edx, CCObject* pSender
) {
    self->goToPage(self->m_nCurrentPage + 1);
}

GDMAKE_HOOK(0x153cd0)
void __fastcall KeybindingsLayer_onPrevPage(
    KeybindingsLayer* self, edx_t edx, CCObject* pSender
) {
    self->goToPage(self->m_nCurrentPage - 1);
}

GDMAKE_HOOK(0x153ce0)
void __fastcall KeybindingsLayer_goToPage(
    KeybindingsLayer* self, edx_t edx, int page
) {
    if (page < 0)
        page = 0;
    if (page > self->m_nPageCount)
        page = self->m_nPageCount;
    
    self->m_nCurrentPage = page;

    CCARRAY_FOREACH_B_TYPE(self->m_pPages->allKeys(), key, CCDictElement) {
        CCARRAY_FOREACH_B_TYPE(as<CCArray*>(key->getObject()), node, CCNode)
            node->setVisible(key->getIntKey() == page);
    }
}

GDMAKE_HOOK(0x152f40)
bool __fastcall KeybindingsLayer_init(KeybindingsLayer* self) {
    if (!self->initWithColor({ 0, 0, 0, 105 })) return false;

    auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    self->m_bNoElasticity = true;

    self->m_pLayer = cocos2d::CCLayer::create();
    self->addChild(self->m_pLayer);
    
    auto bg = cocos2d::extension::CCScale9Sprite::create("GJ_square01.png", { 0.0f, 0.0f, 80.0f, 80.0f });
    bg->setContentSize({ 420.0f, 280.0f });
    bg->setPosition(winSize / 2);
    self->m_pLayer->addChild(bg);

    auto title = CCLabelBMFont::create("Key Bindings", "bigFont.fnt");
    title->setPosition(winSize.width / 2, winSize.height / 2 + 140.0f - 24.0f);
    title->setScale(.8f);
    self->m_pLayer->addChild(title);
    
    // auto infoLabel = CCLabelBMFont::create(
    //     "Click a keybind to Customize it!",
    //     "goldFont.fnt"
    // );
    // infoLabel->setPosition(title->getPosition() + CCPoint { 0.0f, -20.0f });
    // infoLabel->setScale(.6f);
    // self->m_pLayer->addChild(infoLabel);

    auto input = InputNode::create(350.0f, "Search Keybinds");
    input->setPosition(title->getPosition() + CCPoint { 0.0f, -35.0f });
    input->getInputNode()->setPositionX(input->getInputNode()->getPositionX() - 170.0f);
    CCARRAY_FOREACH_B_TYPE(
        input->getInputNode()->getChildren(), c, CCNode
    ) c->setAnchorPoint({ .0f, .5f });
    input->setScale(.7f);
    self->m_pLayer->addChild(input);

    self->m_pPages = CCDictionary::create();
    self->m_pPages->retain();

    self->m_pUnused = CCDictionary::create();
    self->m_pUnused->retain();

    self->m_pButtonMenu = cocos2d::CCMenu::create();
    self->m_pLayer->addChild(self->m_pButtonMenu);

    auto arr = CCArray::create();

    arr->addObject(new KeybindItem("Gameplay"));
    for (auto bind : KeybindManager::get()->getCallbacks(kKBPlayLayer))
        arr->addObject(new KeybindItem(bind));
    arr->addObject(new KeybindItem("Global"));
    for (auto bind : KeybindManager::get()->getCallbacks(kKBGlobal))
        arr->addObject(new KeybindItem(bind));
    arr->addObject(new KeybindItem("Editor"));
    for (auto bind : KeybindManager::get()->getCallbacks(kKBEditor))
        arr->addObject(new KeybindItem(bind));

    auto list = KeybindListView::create(arr, 380.0f, 180.0f);
    list->setPosition(winSize / 2 - CCPoint { 190.0f, 120.0f });
    self->m_pLayer->addChild(list);

    auto closeBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png"),
        self,
        (SEL_MenuHandler)&KeybindingsLayer::onClose
    );
    closeBtn->setPosition(-210.0f + 5.0f, 140.0f - 5.0f);
    closeBtn->setSizeMult(1.5f);
    self->m_pButtonMenu->addChild(closeBtn);

    // self->m_pLeftArrow = CCMenuItemSpriteExtra::create(
    //     CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"),
    //     self,
    //     (SEL_MenuHandler)&KeybindingsLayer::onPrevPage
    // );
    // self->m_pLeftArrow->setPosition(- winSize.width / 2 + 24.0f, 0.0f);
    // self->m_pButtonMenu->addChild(self->m_pLeftArrow);

    // self->m_pRightArrow = CCMenuItemSpriteExtra::create(
    //     CCNodeConstructor()
    //         .fromFrameName("GJ_arrow_01_001.png")
    //         .flipX()
    //         .done(),
    //     self,
    //     (SEL_MenuHandler)&KeybindingsLayer::onNextPage
    // );
    // self->m_pRightArrow->setPosition(winSize.width / 2 - 24.0f, 0.0f);
    // self->m_pButtonMenu->addChild(self->m_pRightArrow);

    self->setKeypadEnabled(true);
    self->setTouchEnabled(true);

    return true;
}
