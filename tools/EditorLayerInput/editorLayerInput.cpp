#include "editorLayerInput.hpp"
#include "LayerManager.hpp"
#include "LayerViewPopup.hpp"
#include "../VisibilityTab/loadVisibilityTab.hpp"

void EditorUI_CB::onNextFreeEditorLayer(CCObject*) {
    auto objs = this->m_pEditorLayer->getAllObjects();

    std::set<int> layers;

    CCARRAY_FOREACH_B_TYPE(objs, obj, GameObject) {
        layers.insert(obj->m_nEditorLayer);
        layers.insert(obj->m_nEditorLayer2);
    }

    int last = -1;
    for (auto const& layer : layers) {
        if (last + 1 != layer)
            break;
        last = layer;
    }

    this->m_pEditorLayer->setCurrentLayer(last + 1);
    this->m_pCurrentLayerLabel->setString(
        CCString::createWithFormat("%d", last + 1)->getCString()
    );

    LayerManager::get()->getLevel()->clearVisible();

    updateEditorLayerInputText(this);
}

void EditorUI_CB::onLockLayer(CCObject*) {
    if (LayerManager::get()->getLevel()->isMultiple()) {
        for (auto const& num : LayerManager::get()->getLevel()->m_vVisibleLayers) {
            auto layer = LayerManager::get()->getLayer(num);

            if (layer)
                layer->m_bLocked = !layer->m_bLocked;
        }
    } else {
        auto layer = LayerManager::get()->getLayer(this->m_pEditorLayer->m_nCurrentLayer);

        if (layer)
            layer->m_bLocked = !layer->m_bLocked;
    }

    updateEditorLayerInputText(this);
}

void EditorUI_CB::onShowLayerPopup(CCObject*) {
    LayerViewPopup::create()->show();
}

GDMAKE_HOOK(0x886b0, "_ZN8EditorUI11onGoToLayerEPN7cocos2d8CCObjectE")
void __fastcall EditorUI_onGoToLayer(gd::EditorUI* self, edx_t edx, cocos2d::CCObject* pSender) {
    GDMAKE_ORIG_V(self, edx, pSender);

    LayerManager::get()->getLevel()->clearVisible();

    updateEditorLayerInputText(self);
}

GDMAKE_HOOK(0x8d7e0, "_ZN8EditorUI11onGroupDownEPN7cocos2d8CCObjectE")
void __fastcall EditorUI_onGroupDown(gd::EditorUI* self, edx_t edx, cocos2d::CCObject* pSender) {
    GDMAKE_ORIG(self, edx, pSender);

    LayerManager::get()->getLevel()->clearVisible();

    updateEditorLayerInputText(self);
}

GDMAKE_HOOK(0x8d780, "_ZN8EditorUI9onGroupUpEPN7cocos2d8CCObjectE")
void __fastcall EditorUI_onGroupUp(gd::EditorUI* self, edx_t edx, cocos2d::CCObject* pSender) {
    GDMAKE_ORIG(self, edx, pSender);

    LayerManager::get()->getLevel()->clearVisible();

    updateEditorLayerInputText(self);
}

GDMAKE_HOOK(0x88790, "_ZN8EditorUI15onGoToBaseLayerEPN7cocos2d8CCObjectE")
void __fastcall EditorUI_onGoToBaseLayer(gd::EditorUI* self, edx_t edx, cocos2d::CCObject* pSender) {
    GDMAKE_ORIG(self, edx, pSender);

    LayerManager::get()->getLevel()->clearVisible();

    updateEditorLayerInputText(self);
}

void showLayerControls(EditorUI* self, bool show) {
    self->m_pCurrentLayerLabel->setVisible(false);

    CATCH_NULL(self->getChildByTag(LAYERINPUT_TAG))->setVisible(show);
    CATCH_NULL(self->getChildByTag(LAYERINPUTBG_TAG))->setVisible(show);
    CATCH_NULL(self->m_pEditGroupBtn->getParent()->getChildByTag(NEXTFREELAYER_TAG))->setVisible(show);
    CATCH_NULL(self->m_pEditGroupBtn->getParent()->getChildByTag(LOCKLAYER_TAG))->setVisible(show);
    CATCH_NULL(self->m_pEditGroupBtn->getParent()->getChildByTag(VIEWLAYERS_TAG))->setVisible(show);
}

void updateEditorLayerInputText(EditorUI* self) {
    auto i = self->getChildByTag(LAYERINPUT_TAG);

    self->m_pCurrentLayerLabel->setVisible(false);

    if (i)
        reinterpret_cast<gd::CCTextInputNode*>(i)->setString(
            self->m_pCurrentLayerLabel->getString()
        );

    auto layer = LayerManager::get()->getLayer(self->m_pEditorLayer->m_nCurrentLayer);
    auto btn = as<CCMenuItemSpriteExtra*>(self->m_pEditGroupBtn->getParent()->getChildByTag(LOCKLAYER_TAG));

    if (btn) {
        const char* lockspr = "GJ_lockGray_001.png";

        if (layer) 
            lockspr = layer->m_bLocked ? 
            "GJ_lock_001.png" :
            "GJ_lock_open_001.png";

        btn->setNormalImage(
            CCNodeConstructor()
                .fromFrameName(lockspr)
                .scale(.7f)
                .move(btn->getNormalImage()->getPosition())
                .alpha(layer ? 255 : 120)
                .done()
        );
        // for some reason setNormalImage
        // resets the anchor point
        if (lockspr == "GJ_lock_open_001.png")
            btn->getNormalImage()->setAnchorPoint({ 0.52f, 0.2f });
        else
            btn->getNormalImage()->setAnchorPoint({ 0.5f, 0.5f });
        btn->setEnabled(layer);
    }
}

#define MOVE_ITEM(_par_, _ix_, _amount_)                                \
    if (getChild<CCNode*>(_par_, _ix_)) {                               \
        auto pos = getChild<CCNode*>(_par_, _ix_)->getPosition();       \
        getChild<CCNode*>(_par_, _ix_)->setPositionX(pos.x + _amount_); \
    }

void loadEditorLayerInput(EditorUI* self) {
    auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    auto ed = EUITextDelegate::create(self);

    self->m_pCurrentLayerLabel->setVisible(false);
    self->m_pCurrentLayerLabel->setPositionX(
        self->m_pCurrentLayerLabel->getPositionX() - 14.0f
    );

    auto spr = cocos2d::extension::CCScale9Sprite::create(
        "square02b_001.png", { 0.0f, 0.0f, 80.0f, 80.0f }
    );

    spr->setScale(.3f);
    spr->setColor({ 0, 0, 0 });
    spr->setOpacity(100);
    spr->setContentSize({ 115.0f, 75.0f });
    spr->setTag(LAYERINPUTBG_TAG);
    spr->setPosition(self->m_pCurrentLayerLabel->getPosition());

    auto eLayerInput = gd::CCTextInputNode::create("All", ed, "bigFont.fnt", 40.0f, 30.0f);

    eLayerInput->setPosition(self->m_pCurrentLayerLabel->getPosition());
    eLayerInput->setLabelPlaceholderColor({ 120, 120, 120 });
    eLayerInput->setAllowedChars("0123456789");
    eLayerInput->setAnchorPoint({ 0, 0 });
    eLayerInput->setScale(.7f);
    eLayerInput->setDelegate(ed);
    eLayerInput->setTag(LAYERINPUT_TAG);
    eLayerInput->setString(self->m_pCurrentLayerLabel->getString());

    self->addChild(spr);
    self->addChild(eLayerInput);

    self->addChild(ed);

    auto menu = self->m_pEditGroupBtn->getParent();

    auto goToAllBtn = getChild<CCMenuItemSpriteExtra*>(menu, 17);

    // CATCH_NULL(getChild<CCMenuItemSpriteExtra*>(menu, 17))->setPositionX(-90);
    // CATCH_NULL(getChild<CCMenuItemSpriteExtra*>(menu, 16))->setPositionX(-70);
    // CATCH_NULL(getChild<CCMenuItemSpriteExtra*>(menu, 15))->setPositionX(-13);

    MOVE_ITEM(menu, 17, -5.0f);
    MOVE_ITEM(menu, 16, -13.0f);
    MOVE_ITEM(menu, 15, -15.0f);

    menu->addChild(
        CCNodeConstructor<CCMenuItemSpriteExtra*>()
            .fromNode(CCMenuItemSpriteExtra::create(
                CCNodeConstructor()
                    .fromFrameName("GJ_arrow_02_001.png")
                    .scale(.54f)
                    .alpha(175)
                    .flipX()
                    .done(),
                self,
                (SEL_MenuHandler)&EditorUI_CB::onNextFreeEditorLayer
            ))
            .move(9.0f, goToAllBtn->getPositionY())
            .tag(NEXTFREELAYER_TAG)
            .done()
    );

    menu->addChild(
        CCNodeConstructor<CCMenuItemSpriteExtra*>()
            .fromNode(CCMenuItemSpriteExtra::create(
                CCNodeConstructor()
                    .fromFrameName("GJ_lock_open_001.png")
                    .scale(.7f)
                    .done(),
                self,
                (SEL_MenuHandler)&EditorUI_CB::onLockLayer
            ))
            .move(-106.0f, goToAllBtn->getPositionY())
            .tag(LOCKLAYER_TAG)
            .done()
    );

    menu->addChild(
        CCNodeConstructor<CCMenuItemSpriteExtra*>()
            .fromNode(CCMenuItemSpriteExtra::create(
                CCNodeConstructor()
                    .fromFrameName("GJ_plus2Btn_001.png")
                    .scale(.7f)
                    .done(),
                self,
                (SEL_MenuHandler)&EditorUI_CB::onShowLayerPopup
            ))
            .move(-130.0f, goToAllBtn->getPositionY())
            .tag(VIEWLAYERS_TAG)
            .done()
    );

    updateEditorLayerInputText(self);
}

bool testSelectObjectLayer(GameObject* obj) {
    auto layer1 = LayerManager::get()->getLayer(obj->m_nEditorLayer);
    auto layer2 = LayerManager::get()->getLayer(obj->m_nEditorLayer2);

    auto current = LevelEditorLayer::get()->m_nCurrentLayer;

    if (obj->m_nEditorLayer == current ||
        obj->m_nEditorLayer2 == current)
        return true;

    bool check2 = false;

    if (obj->m_nEditorLayer == 0 &&
        obj->m_nEditorLayer2 > 0) check2 = true;
    
    if (!check2 && layer1 && (!layer1->m_bVisible || layer1->m_bLocked))
        return false;

    if (check2 && layer2 && (!layer2->m_bVisible || layer2->m_bLocked))
        return false;

    if (current != -1 && obj->m_nEditorLayer != current &&
        obj->m_nEditorLayer2 != current)
        return false;

    return true;
}
