#include "../../BetterEdit.hpp"
#include "../CustomKeybinds/KeybindManager.hpp"
#include <InputNode.hpp>
#include "KeybindListView.hpp"

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
    
    auto input = InputNode::create(425.0f, "Search Keybinds");
    input->setPosition(title->getPosition() + CCPoint { 0.0f, -35.0f });
    input->getInputNode()->setPositionX(input->getInputNode()->getPositionX() - 200.0f);
    CCARRAY_FOREACH_B_TYPE(
        input->getInputNode()->getChildren(), c, CCNode
    ) c->setAnchorPoint({ .0f, .5f });
    input->setScale(.8f);
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
        arr->addObject(new KeybindItem(bind, kKBPlayLayer));
    arr->addObject(new KeybindItem("Global"));
    for (auto bind : KeybindManager::get()->getCallbacks(kKBGlobal))
        arr->addObject(new KeybindItem(bind, kKBGlobal));
    arr->addObject(new KeybindItem("Editor"));
    for (auto bind : KeybindManager::get()->getCallbacks(kKBEditor))
        arr->addObject(new KeybindItem(bind, kKBEditor));

    auto list = KeybindListView::create(arr, 340.0f, 180.0f);
    list->setPosition(winSize / 2 - CCPoint { 170.0f, 120.0f });
    self->m_pLayer->addChild(list);

    input->getInputNode()->setDelegate(list);

    self->registerWithTouchDispatcher();
    CCDirector::sharedDirector()->getTouchDispatcher()->incrementForcePrio(2);

    {
        // auto topGradient = CCSprite::createWithSpriteFrameName("d_gradient_c_01_001.png");
        // topGradient->setPosition({
        //     winSize.width / 2,
        //     winSize.height / 2 + 45.0f
        // });
        // topGradient->setFlipY(true);
        // topGradient->setScaleX(11.5f);
        // topGradient->setColor(cc3x(0x953));
        // self->m_pLayer->addChild(topGradient);

        // auto bottomGradient = CCSprite::createWithSpriteFrameName("d_gradient_c_01_001.png");
        // bottomGradient->setPosition({
        //     winSize.width / 2,
        //     winSize.height / 2 - 105.0f
        // });
        // bottomGradient->setScaleX(11.5f);
        // bottomGradient->setColor(cc3x(0x953));
        // self->m_pLayer->addChild(bottomGradient);

        auto topItem = CCSprite::createWithSpriteFrameName("GJ_commentTop_001.png");
        topItem->setPosition({
            winSize.width / 2,
            winSize.height / 2 + 55.0f
        });
        self->m_pLayer->addChild(topItem);

        auto bottomItem = CCSprite::createWithSpriteFrameName("GJ_commentTop_001.png");
        bottomItem->setPosition({
            winSize.width / 2,
            winSize.height / 2 - 115.0f
        });
        bottomItem->setFlipY(true);
        self->m_pLayer->addChild(bottomItem);

        auto sideItem = CCSprite::createWithSpriteFrameName("GJ_commentSide_001.png");
        sideItem->setPosition({
            winSize.width / 2 - 173.5f,
            winSize.height / 2 - 29.0f
        });
        sideItem->setScaleY(5.0f);
        self->m_pLayer->addChild(sideItem);

        auto sideItemRight = CCSprite::createWithSpriteFrameName("GJ_commentSide_001.png");
        sideItemRight->setPosition({
            winSize.width / 2 + 173.5f,
            winSize.height / 2 - 29.0f
        });
        sideItemRight->setScaleY(5.0f);
        sideItemRight->setFlipX(true);
        self->m_pLayer->addChild(sideItemRight);
    }

    auto closeBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png"),
        self,
        (SEL_MenuHandler)&KeybindingsLayer::onClose
    );
    closeBtn->setPosition(-210.0f + 5.0f, 140.0f - 5.0f);
    closeBtn->setSizeMult(1.5f);
    self->m_pButtonMenu->addChild(closeBtn);

    self->setKeypadEnabled(true);
    self->setTouchEnabled(true);

    return true;
}
