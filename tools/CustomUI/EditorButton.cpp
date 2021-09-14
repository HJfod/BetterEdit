#include "EditorButton.hpp"

EditorButtonMap g_map = EditorButtonMap({
    { "gd.edit.copy",                   { "GJ_copyBtn2_001.png" } },
    { "gd.edit.paste",                  { "GJ_pasteBtn2_001.png" } },
    { "gd.edit.copy_and_paste",         { "GJ_duplicateObjectBtn2_001.png" } },
    { "betteredit.edit_special",        { "GJ_editObjBtn4_001.png" } },
    { "betteredit.edit_group",          { "GJ_groupIDBtn2_001.png" } },
    { "betteredit.edit_object",         { "GJ_editObjBtn3_001.png" } },
    { "betteredit.copy_values",         { "GJ_copyStateBtn_001.png" } },
    { "betteredit.paste_state",         { "GJ_pasteStateBtn_001.png" } },
    { "betteredit.paste_color",         { "GJ_pasteColorBtn_001.png" } },
    { "betteredit.toggle_hsv_picker",   { "GJ_editHSVBtn2_001.png" } },
    { "betteredit.go_to_layer",         { "GJ_goToLayerBtn_001.png" } },
    { "gd.edit.deselect",               { "GJ_deSelBtn2_001.png" } },
});

EditorButtonMap const& getEditorButtons() {
    return g_map;
}

bool EditorButton::initWithID(EditorUI* ui, keybind_id const& id) {
    if (!g_map.count(id))
        return false;
    this->m_obItem = g_map[id];

    auto spr = CCSprite::createWithSpriteFrameName(item.spriteName);

    if (!CCMenuItemSprite::initWithNormalSprite(spr, nullptr, nullptr, this, nullptr))
        return false;
    if (!CCMenuItemSpriteExtra::init(spr))
        return false;

    this->m_sID = id;
    this->m_pUI = ui;

    return true;
}

void EditorButton::activate() {
    if (this->m_pUI) {
        KeybindManager::get()->invokeCallback(
            this->m_obItem.keybind
            this->m_pUI,
            nullptr
        );
    }
}

void EditorButton::setEnabled(bool enable) {
    this->CCMenuItemSpriteExtra::setEnabled(enable);
    this->setColor(enable ? cc3x(0xf) : cc3x(0xa6));
    this->setOpacity(enable ? 255 : 175);
}

EditorButton* EditorButton::createFromID(EditorUI* ui, keybind_id const& id) {
    auto ret = EditorButton;

    if (ret && ret->initWithID(ui, id)) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_RELEASE(ret);
    return nullptr;
}
