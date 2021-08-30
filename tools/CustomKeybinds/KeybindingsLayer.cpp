#include "../../BetterEdit.hpp"
#include "../CustomKeybinds/KeybindManager.hpp"
#include <InputNode.hpp>
#include "KeybindingsLayer.hpp"
#include "KeybindListView.hpp"
#include "Scrollbar.hpp"
#include "KeybindSettingsLayer.hpp"

static constexpr const int KBLDELEGATE_TAG = 0x44432;
static constexpr const int KBLLIST_TAG = 0x44431;
static constexpr const int KBLSCROLLBAR_TAG = 0x44430;
static constexpr const int KBLINPUT_TAG = 0x44429;
std::string searchQuery = "";

std::map<std::string, bool> KeybindingsLayerDelegate::m_mFoldedCategories = std::map<std::string, bool>();

void KeybindingsLayerDelegate::textChanged(CCTextInputNode* input) {
    if (input->getString() && strlen(input->getString()))
        searchQuery = input->getString();
    else
        searchQuery = "";
    
    this->m_pLayer->reloadList();
}

void KeybindingsLayerDelegate::FLAlert_Clicked(FLAlertLayer*, bool btn2) {
    if (btn2) {
        KeybindManager::get()->resetAllToDefaults();

        this->m_pLayer->reloadList();
    }
}

KeybindingsLayerDelegate* KeybindingsLayerDelegate::create(KeybindingsLayer_CB* layer) {
    auto ret = new KeybindingsLayerDelegate;

    if (ret && ret->init()) {
        ret->setTag(KBLDELEGATE_TAG);
        ret->m_pLayer = layer;
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}


bool matchSearchQuery(KeybindType type, KeybindCallback* bind) {
    if (stringToLower(bind->name).find(stringToLower(searchQuery)) != std::string::npos)
        return true;
    
    std::string keyStr = "";
    for (auto const& key : KeybindManager::get()->getKeybindsForCallback(type, bind))
        keyStr += key.toString() + ", ";

    return stringToLower(keyStr).find(stringToLower(searchQuery)) != std::string::npos;
}

void KeybindingsLayer_CB::reloadList() {
    this->m_pLayer->removeChildByTag(KBLLIST_TAG);

    auto winSize = CCDirector::sharedDirector()->getWinSize();
    auto delegate = as<KeybindingsLayerDelegate*>(this->getChildByTag(KBLDELEGATE_TAG));

    auto arr = CCArray::create();

    arr->addObject(new KeybindItem("Gameplay", delegate));
    if (!delegate->m_mFoldedCategories["Gameplay"])
        for (auto bind : KeybindManager::get()->getCallbacks(kKBPlayLayer)) {
            if (
                !searchQuery.size() || matchSearchQuery(kKBPlayLayer, bind)
            )
                arr->addObject(new KeybindItem(bind, kKBPlayLayer, delegate));
        }

    // arr->addObject(new KeybindItem("Global", delegate));
    // if (!delegate->m_mFoldedCategories["Global"])
    //     for (auto bind : KeybindManager::get()->getCallbacks(kKBGlobal)) {
    //         if (
    //             !searchQuery.size() || matchSearchQuery(kKBGlobal, bind)
    //         )
    //             arr->addObject(new KeybindItem(bind, kKBGlobal, delegate));
    //     }

    arr->addObject(new KeybindItem("Editor", delegate));
    if (!delegate->m_mFoldedCategories["Editor"])
        for (auto bind : KeybindManager::get()->getCallbacks(kKBEditor)) {
            if (
                !searchQuery.size() || matchSearchQuery(kKBEditor, bind)
            )
                arr->addObject(new KeybindItem(bind, kKBEditor, delegate));
        }

    auto list = KeybindListView::create(arr, 340.0f, 180.0f);
    list->setPosition(winSize / 2 - CCPoint { 170.0f, 120.0f });
    list->setTag(KBLLIST_TAG);
    this->m_pLayer->addChild(list);

    CATCH_NULL(as<Scrollbar*>(this->m_pLayer->getChildByTag(KBLSCROLLBAR_TAG)))
        ->setList(list);
}

void KeybindingsLayer_CB::onResetAll(CCObject*) {
    FLAlertLayer::create(
        as<KeybindingsLayerDelegate*>(this->getChildByTag(KBLDELEGATE_TAG)),
        "Reset Keybinds",
        "Cancel", "Reset",
        "Are you sure you want to <cr>reset</c> ALL <cy>keybinds</c> "
        "to <cl>default</c>?"
    )->show();
}

void KeybindingsLayer_CB::onGlobalSettings(CCObject*) {
    this->detachInput();

    KeybindSettingsLayer::create()->show();
}

void KeybindingsLayer_CB::detachInput() {
    auto input = as<InputNode*>(this->m_pLayer->getChildByTag(KBLINPUT_TAG));

    if (input) {
        input->getInputNode()->m_pTextField->detachWithIME();
        input->getInputNode()->detachWithIME();
    }
}

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

    auto delegate = KeybindingsLayerDelegate::create(as<KeybindingsLayer_CB*>(self));
    self->addChild(delegate);
    
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
    input->setTag(KBLINPUT_TAG);
    input->setString(searchQuery.c_str());
    CCARRAY_FOREACH_B_TYPE(
        input->getInputNode()->getChildren(), c, CCNode
    ) c->setAnchorPoint({ .0f, .5f });
    input->setScale(.8f);
    input->getInputNode()->setDelegate(delegate);
    self->m_pLayer->addChild(input);

    self->m_pPages = CCDictionary::create();
    self->m_pPages->retain();

    self->m_pUnused = CCDictionary::create();
    self->m_pUnused->retain();

    self->m_pButtonMenu = cocos2d::CCMenu::create();
    self->m_pLayer->addChild(self->m_pButtonMenu);

    auto bar = Scrollbar::create(nullptr);
    bar->setPosition(winSize.width / 2 + 190.0f, winSize.height / 2 - 30.0f);
    bar->setTag(KBLSCROLLBAR_TAG);
    self->m_pLayer->addChild(bar, 800);

    as<KeybindingsLayer_CB*>(self)->reloadList();

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
        topItem->setZOrder(500);
        self->m_pLayer->addChild(topItem);

        auto bottomItem = CCSprite::createWithSpriteFrameName("GJ_commentTop_001.png");
        bottomItem->setPosition({
            winSize.width / 2,
            winSize.height / 2 - 115.0f
        });
        bottomItem->setZOrder(500);
        bottomItem->setFlipY(true);
        self->m_pLayer->addChild(bottomItem);

        auto sideItem = CCSprite::createWithSpriteFrameName("GJ_commentSide_001.png");
        sideItem->setPosition({
            winSize.width / 2 - 173.5f,
            winSize.height / 2 - 29.0f
        });
        sideItem->setZOrder(500);
        sideItem->setScaleY(5.0f);
        self->m_pLayer->addChild(sideItem);

        auto sideItemRight = CCSprite::createWithSpriteFrameName("GJ_commentSide_001.png");
        sideItemRight->setPosition({
            winSize.width / 2 + 173.5f,
            winSize.height / 2 - 29.0f
        });
        sideItemRight->setZOrder(500);
        sideItemRight->setScaleY(5.0f);
        sideItemRight->setFlipX(true);
        self->m_pLayer->addChild(sideItemRight);
    }

    auto resetBtn = CCMenuItemSpriteExtra::create(
        CCNodeConstructor<ButtonSprite*>()
            .fromButtonSprite(
                "Reset", "GJ_button_05.png", "bigFont.fnt"
            )
            .scale(.6f)
            .done(),
        self,
        menu_selector(KeybindingsLayer_CB::onResetAll)
    );
    resetBtn->setPosition(210.0f - 40.0f, 140.0f - 25.0f);
    self->m_pButtonMenu->addChild(resetBtn);

    auto settingsBtn = CCMenuItemSpriteExtra::create(
        CCNodeConstructor()
            .fromFrameName("GJ_optionsBtn_001.png")
            .scale(.6f)
            .done(),
        self,
        menu_selector(KeybindingsLayer_CB::onGlobalSettings)
    );
    settingsBtn->setPosition(- 210.0f + 5.0f, - 140.0f + 5.0f);
    self->m_pButtonMenu->addChild(settingsBtn);

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
