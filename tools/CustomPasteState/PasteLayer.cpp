#include "PasteLayer.hpp"
#include <GUI/CCControlExtension/CCScale9Sprite.h>

using namespace gd;
using namespace gdmake;
using namespace gdmake::extra;
using namespace cocos2d;
using namespace cocos2d::extension;

std::set<PasteLayer::State>* g_pStates = nullptr;
bool g_bPasteState = false;

template <typename T>
bool bool_cast(T const v) { return static_cast<bool>(reinterpret_cast<int>(v)); }

void PasteLayer::setup() {
    this->m_pButtonMenu->addChild(
        CCNodeConstructor<CCScale9Sprite*>()
            .fromNode(CCScale9Sprite::create(
                "square02b_001.png", { 0.0f, 0.0f, 80.0f, 80.0f }
            ))
            .color({ 0, 0, 0 })
            .alpha(75)
            .csize(140.0f, 180.0f)
            .move(-60.0f, 0.0f)
            .z(-100)
            .done()
    );

    // this->addStateToggle("Obj ID", PasteLayer::ObjID);
    this->addStateToggle("Group IDs", PasteLayer::Groups);
    this->addStateToggle("Z Layer", PasteLayer::ZLayer);
    this->addStateToggle("Z Order", PasteLayer::ZOrder);
    this->addStateToggle("Editor Layer", PasteLayer::EditorLayer);
    this->addStateToggle("Editor Layer 2", PasteLayer::EditorLayer2);
    this->addStateToggle("Dont Fade", PasteLayer::DontFade);
    this->addStateToggle("Dont Enter", PasteLayer::DontEnter);
    this->addStateToggle("Position X", PasteLayer::PositionX);
    this->addStateToggle("Position Y", PasteLayer::PositionY);
    this->addStateToggle("Rotation", PasteLayer::Rotation);
    this->addStateToggle("Scale", PasteLayer::Scale);
    this->addStateToggle("High Detail", PasteLayer::HighDetail);

    this->m_pButtonMenu->addChild(
        CCNodeConstructor<CCMenuItemSpriteExtra*>()
            .fromNode(CCMenuItemSpriteExtra::create(
                CCNodeConstructor<ButtonSprite*>()
                    .fromNode(ButtonSprite::create(
                        "Select All", 80, true, "bigFont.fnt", "GJ_button_04.png", 0, .8f
                    ))
                    .scale(.8f)
                    .done(),
                this,
                (SEL_MenuHandler)&PasteLayer::onToggleAll
            ))
            .move(80.0f, 20.0f)
            .udata(1)
            .done()
    );
    this->m_pButtonMenu->addChild(
        CCNodeConstructor<CCMenuItemSpriteExtra*>()
            .fromNode(CCMenuItemSpriteExtra::create(
                CCNodeConstructor<ButtonSprite*>()
                    .fromNode(ButtonSprite::create(
                        "Deselect All", 80, true, "bigFont.fnt", "GJ_button_04.png", 0, .7f
                    ))
                    .scale(.8f)
                    .done(),
                this,
                (SEL_MenuHandler)&PasteLayer::onToggleAll
            ))
            .move(80.0f, -20.0f)
            .udata(0)
            .done()
    );

    this->m_pButtonMenu->addChild(
        CCNodeConstructor<CCMenuItemSpriteExtra*>()
            .fromNode(CCMenuItemSpriteExtra::create(
                CCNodeConstructor<ButtonSprite*>()
                    .fromNode(ButtonSprite::create(
                        "Paste", 0, 0, "goldFont.fnt", "GJ_button_01.png", 0, .8f
                    ))
                    .scale(.8f)
                    .done(),
                this,
                (SEL_MenuHandler)&PasteLayer::onPasteState
            ))
            .move(0.0f, - this->m_pLrSize.height / 2 + 25.0f)
            .done()
    );
}

void PasteLayer::addStateToggle(const char* text, PasteLayer::State state) {
    auto toggle = CCMenuItemToggler::createWithStandardSprites(
        this, (SEL_MenuHandler)&PasteLayer::onToggle, .4f
    );
    toggle->setPosition(-115.0f, 78.0f - 14.0f * this->m_nToggleCount);
    toggle->toggle(g_pStates->count(state));
    toggle->setUserData(as<void*>(state));
    this->m_pButtonMenu->addChild(toggle);

    auto label = CCLabelBMFont::create(text, "bigFont.fnt");
    label->limitLabelWidth(230.0f, .4f, .2f);
    label->setPosition(-100.0f + label->getScaledContentSize().width / 2, 78.0f - 14.0f * this->m_nToggleCount);
    this->m_pButtonMenu->addChild(label);

    this->m_vToggles.push_back(toggle);

    this->m_nToggleCount++;
}

void PasteLayer::onToggleAll(CCObject* pSender) {
    for (auto toggle : this->m_vToggles)
        // you might think that checking the state of the toggle
        // is entirely pointless

        // however, if i dont do this, pressing the button again
        // will shadow-toggle everything due to the fact that
        // onToggle gets called first
        if (toggle->isToggled() != bool_cast(as<CCNode*>(pSender)->getUserData()))
            toggle->toggleWithCallback(as<CCNode*>(pSender)->getUserData());
}

void PasteLayer::onToggle(CCObject* pSender) {
    auto toggle = as<CCMenuItemToggler*>(pSender);
    auto state = static_cast<PasteLayer::State>(as<int>(toggle->getUserData()));

    if (toggle->isToggled())
        g_pStates->erase(state);
    else
        g_pStates->insert(state);
}

void PasteLayer::onPasteState(CCObject* pSender) {
    auto lel = LevelEditorLayer::get();

    g_bPasteState = true;

    lel->pasteAtributeState(
        lel->getEditorUI()->m_pSelectedObject,
        lel->getEditorUI()->m_pSelectedObjects
    );

    g_bPasteState = false;

    lel->getEditorUI()->updateObjectInfoLabel();

    this->onClose(pSender);
}

std::set<PasteLayer::State> * PasteLayer::getStates() {
    return g_pStates;
}

bool PasteLayer::wantsToPasteState() {
    return g_bPasteState;
}

PasteLayer* PasteLayer::create() {
    auto pRet = new PasteLayer();

    if (g_pStates == nullptr)
        g_pStates = new std::set<PasteLayer::State>;

    if (pRet && pRet->init(320.0f, 270.0f, "GJ_square01.png", "Paste State")) {
        pRet->autorelease();
        return pRet;
    }

    CC_SAFE_DELETE(pRet);
    return nullptr;
}
