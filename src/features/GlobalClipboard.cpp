#include <Geode/DefaultInclude.hpp>
#include <Geode/modify/EditorUI.hpp>

using namespace geode::prelude;

std::string clip;

struct DeleteClipboard {
    void warning(CCObject*) {
        geode::createQuickPopup(
            "Warning",
            "Are you sure you would like to delete your copied objects?",
            "No", "Yes",
            [this](auto, bool btn2) {
                if (btn2) {
                    EditorUI::get()->m_clipboard = "";
                }
            }
        );
    }
};

class $modify(EditorUI) {
    bool init(LevelEditorLayer* lel) {
        if (!EditorUI::init(lel))
            return false;

        if (Mod::get()->getSettingValue<bool>("enable-global-clipboard"))
            m_clipboard = clip;

        this->updateButtons();

        /*
            TODO: put the clear button somewhere normal
        */
        /*auto ref = this->getChildByID("editor-buttons-menu")->getChildByID("paste-button");
        auto spr = CircleButtonSprite::createWithSpriteFrameName("GJ_deleteIcon_001.png");

        auto menu = CCMenu::create();
        auto deleteBtn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(DeleteClipboard::warning));

        menu->setPosition(ref->getPositionX() + 13, ref->getPositionY() + 14);
        deleteBtn->setScale(.375f);
        deleteBtn->m_baseScale = .375f;
        deleteBtn->setID("clear-paste-button"_spr);
        menu->addChild(deleteBtn);
        this->getChildByID("editor-buttons-menu")->addChild(menu);*/

        return true;
    }

    void destructor() { // can crash randomly cuz geode can't seem to hook destructors properly?
        clip = m_clipboard;

        EditorUI::~EditorUI();
    }
};