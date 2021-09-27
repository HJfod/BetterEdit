#include "../ContextMenu.hpp"
#include "../CustomizeCMLayer.hpp"
#include "../../EditorLayerInput/editorLayerInput.hpp"

#define MORD(x, d) (this->m_pMenu ? this->m_pMenu->x : d)

bool KeybindContextMenuItem::init(ContextMenu* menu, keybind_id const& id) {
    if (!ContextMenuItem::init(menu))
        return false;
    
    this->m_sID = id;

    this->m_pLabel = this->createLabel();
    this->m_pLabel->setColor(to3B(m_pMenu->m_colText));
    this->addChild(this->m_pLabel);

    auto target = KeybindManager::get()->getTargetByID(id);

    this->m_pCallback = target.bind;

    if (target.bind)
        this->m_pLabel->setString(target.bind->name.c_str());
    else
        this->m_pLabel->setString("Invalid\nkeybind");

    return true;
}

void KeybindContextMenuItem::visit() {
    this->m_pLabel->setPosition(
        this->getContentSize() / 2
    );
    this->m_pLabel->limitLabelWidth(
        this->getScaledContentSize().width - 10.0f,
        this->m_pMenu ? this->m_pMenu->m_fFontScale : .4f,
        .02f
    );

    ContextMenuItem::visit();
}

void KeybindContextMenuItem::activate() {
    KeybindManager::get()->invokeCallback(
        this->m_sID, LevelEditorLayer::get()->getEditorUI(), nullptr
    );
    SuperMouseManager::get()->releaseCapture(this);
    if (this->m_pMenu)
        this->m_pMenu->hide();
}

KeybindContextMenuItem* KeybindContextMenuItem::create(
    ContextMenu* menu, keybind_id const& id
) {
    auto ret = new KeybindContextMenuItem;

    if (ret && ret->init(menu, id)) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}

