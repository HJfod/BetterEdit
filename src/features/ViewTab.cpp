#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/binding/ButtonSprite.hpp>
#include <Geode/binding/EditButtonBar.hpp>
#include <Geode/utils/cocos.hpp>
#include <hjfod.custom-keybinds/include/Keybinds.hpp>

#include "VisibilityToggle.hpp"
#include "RotateSaws.hpp"

using namespace geode::prelude;
using namespace keybinds;

constexpr const int VIEWBUTTONBAR_TAG = 0x234592;

struct $modify(VisibilityTabUI, EditorUI) {
    CCNode* viewModeBtn;

    bool init(LevelEditorLayer* lel) {
        if (!EditorUI::init(lel))
            return false;
        
        auto winSize = CCDirector::get()->getWinSize();
        
        // make a bit space for new style menu since the old one is a tiny bit cramped
        if (auto side = getChildOfType<CCSprite>(this, 1)) {
            side->setPositionX(96.f);
        }
        for (auto& child : CCArrayExt<CCNode>(m_pChildren)) {
            if (auto bar = typeinfo_cast<EditButtonBar*>(child)) {
                bar->setPositionX(bar->getPositionX() + 5.f);
            }
        }

        if (auto menu = this->getChildByID("toolbar-categories-menu")) {
            m_fields->viewModeBtn = CCMenuItemSpriteExtra::create(
                CCNode::create(), this, menu_selector(EditorUI::toggleMode)
            );
            m_fields->viewModeBtn->setID("view-button"_spr);
            m_fields->viewModeBtn->setTag(4);
            menu->addChild(m_fields->viewModeBtn);

            this->updateModeSprites();

            menu->setContentSize({ 90.f, 90.f });
            menu->setPositionX(47.f);
            menu->setLayout(RowLayout::create()
                ->setCrossAxisOverflow(false)
                ->setGrowCrossAxis(true)
            );
        }

        this->template addEventListener<InvokeBindFilter>([=](InvokeBindEvent* event) {
            if (event->isDown()) {
                this->toggleMode(m_fields->viewModeBtn);
            }
            return ListenerResult::Propagate;
        }, "view-mode"_spr);

        /*
            Create buttons
        */
        auto btns = CCArray::create();

        btns->addObject(VisibilityToggle::create(
            "v_rotate.png"_spr,
            []() -> bool {
                return shouldRotateSaw();
            },
            [&](bool s, auto p) -> void {
                onRotateSaws(p);
            }
        ));

        btns->addObject(VisibilityToggle::create(
            "v_ldm.png"_spr,
            []() -> bool { return Mod::get()->getSettingValue<bool>("hide-ldm"); },
            [&](bool b, auto) -> void { Mod::get()->setSettingValue<bool>("hide-ldm", b); }
        ));

        /*
            Button bar
        */
        auto buttonBar = EditButtonBar::create(
            btns,
            { CCDirector::sharedDirector()->getWinSize().width / 2, 86.0f },
            m_tabsArray->count(), false,
            GameManager::sharedState()->getIntGameVariable("0049"),
            GameManager::sharedState()->getIntGameVariable("0050")
        );
        buttonBar->setTag(VIEWBUTTONBAR_TAG);
        buttonBar->setVisible(m_selectedTab == 4);

        addChild(buttonBar, 10);

        return true;
    }

    void showUI(bool show) {
        EditorUI::showUI(show);
        m_fields->viewModeBtn->setVisible(show);
        showVisibilityTab(show);
    }

    void toggleMode(CCObject* sender) {
        EditorUI::toggleMode(sender);
        this->resetUI();
        this->updateModeSprites();

        auto viewBtnBar = getChildByTag(VIEWBUTTONBAR_TAG);

        if(viewBtnBar != nullptr)
            viewBtnBar->setVisible(m_selectedMode == 4);
    }

    void updateModeSprite(CCNode* node, int tag, const char* spr) {
        // resetUI resets the button sprites so we may have to change them back 
        // to squarish ones again
        if (node) {
            auto bg = m_selectedMode == tag ? "GJ_button_02.png" : "GJ_button_01.png";
            auto btn = static_cast<CCMenuItemSpriteExtra*>(node);
            if (auto bspr = typeinfo_cast<ButtonSprite*>(btn->getNormalImage())) {
                // remove GD's texture
                bspr->setTexture(nullptr);
                bspr->setTextureRect({ 0, 0, 0, 0 });
                bspr->updateBGImage(bg);
            }
            else {
                btn->setNormalImage(ButtonSprite::create(
                    CCSprite::createWithSpriteFrameName(spr),
                    0x32, true, 0x32, bg, .6f
                ));
            }
        }
    }

    void updateModeSprites() {
        this->updateModeSprite(m_buildModeBtn, 2, "tab-create.png"_spr);
        this->updateModeSprite(m_deleteModeBtn, 1, "tab-delete.png"_spr);
        this->updateModeSprite(m_editModeBtn, 3, "tab-edit.png"_spr);
        this->updateModeSprite(m_fields->viewModeBtn, 4, "tab-view.png"_spr);
    }

    void showVisibilityTab(bool show) {
        typeinfo_cast<EditButtonBar*>(getChildByTag(VIEWBUTTONBAR_TAG))
            ->setVisible(show && m_selectedMode == 4);

        m_buildModeBtn->getParent()->getChildByTag(4)
            ->setVisible(show);
    }

    void updateVisibilityTab() {
        //if (BetterEdit::getDisableVisibilityTab())
        //    return;
        
        auto bbar = static_cast<EditButtonBar*>(getChildByTag(VIEWBUTTONBAR_TAG));

        if (!bbar) return;

        //bbar->reloadItemsInNormalSize(); // broken?
        bbar->reloadItems(
            GameManager::sharedState()->getIntGameVariable("0049"),
            GameManager::sharedState()->getIntGameVariable("0050")
        );
        
        CCObject* btn;
        CCARRAY_FOREACH(bbar->m_buttonArray, btn) {
            typeinfo_cast<VisibilityToggle*>(btn)->updateState();
        }
    }
};

class $modify(EditorPauseLayer) {
    void onResume(CCObject* pSender) {
        EditorPauseLayer::onResume(pSender);

        static_cast<VisibilityTabUI*>(EditorUI::get())->updateVisibilityTab();
    }
};

$execute {
    BindManager::get()->registerBindable(BindableAction(
        "view-mode"_spr,
        "View Mode",
        "Toggle the View Tab",
        { Keybind::create(KEY_Four) },
        Category::EDITOR_UI
    ), "robtop.geometry-dash/delete-mode");
}
