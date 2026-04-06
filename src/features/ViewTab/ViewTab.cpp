#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/GameObject.hpp>
#include <Geode/modify/DrawGridLayer.hpp>
#include <Geode/modify/FMODAudioEngine.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/binding/ButtonSprite.hpp>
#include <Geode/binding/EditButtonBar.hpp>
#include <Geode/binding/GameManager.hpp>
#include <Geode/binding/GJGroundLayer.hpp>
#include <Geode/utils/cocos.hpp>
#include <utils/BEMenuItemToggler.hpp>
#include <utils/Editor.hpp>
#include <utils/HolyUB.hpp>
#include <features/supporters/Pro.hpp>
#include <alphalaneous.editortab_api/include/EditorTabAPI.hpp>

using namespace geode::prelude;

class $modify(GameObjectExtra, GameObject) {
    $override
    void updateVisibility() {
        this->setVisible(m_bVisible || !this->shouldHide());
    }
    $override
    void setVisible(bool visible) {
        if (this->shouldHide()) {
            visible = false;
        }
        GameObject::setVisible(visible);
    }
    bool shouldHide() const {
        return
            m_isHighDetail &&
            // Only in the editor
            EditorUI::get() && 
            Mod::get()->template getSavedValue<bool>("hide-ldm", false);
    }
};

struct $modify(ViewTabUI, EditorUI) {
    struct Fields {
        CCNode* viewModeBtn;
        ListenerHandle onUIHide;
    };

    static void onModify(auto& self) {
        (void)self.setHookPriority("EditorUI::selectObject",  Priority::VeryLate);
        (void)self.setHookPriority("EditorUI::selectObjects", Priority::VeryLate);
        (void)self.setHookPriority("EditorUI::toggleMode", Priority::Early);
    }

    void updateModeSprite(CCNode* node, ZStringView id, const char* spr) {
        // resetUI resets the button sprites so we may have to change them back 
        // to squarish ones again
        if (node) {
            auto bg = alpha::editor_tabs::getCurrentMode() == Ok(id) ? "GJ_button_02.png" : "GJ_button_01.png";
            auto btn = static_cast<CCMenuItemSpriteExtra*>(node);
            btn->setNormalImage(ButtonSprite::create(
                CCSprite::createWithSpriteFrameName(spr), 50, true, 50, bg, .6f
            ));
        }
    }
    void updateModeSprites() {
        // This is just to make sure the view tab is actually enabled so we 
        // don't change these when it's disabled
        if (alpha::editor_tabs::nodeForTab("view"_spr)) {
            this->updateModeSprite(m_buildModeBtn, alpha::editor_tabs::BUILD, "tab-create.png"_spr);
            this->updateModeSprite(m_deleteModeBtn, alpha::editor_tabs::DELETE, "tab-delete.png"_spr);
            this->updateModeSprite(m_editModeBtn, alpha::editor_tabs::EDIT, "tab-edit.png"_spr);
            this->updateModeSprite(m_fields->viewModeBtn, "view"_spr, "tab-view.png"_spr);
        }
    }

    ButtonSprite* createViewToggleSpr(const char* frame, bool selected) {
        auto top = CCSprite::createWithSpriteFrameName(frame);
        return ButtonSprite::create(
            top, 50, 0, 50, .8f, true, (selected ? "GJ_button_02.png" : "GJ_button_01.png"), true
        );
    }
    BEMenuItemToggler* createViewToggle(
        const char* frame,
        std::function<bool()> get,
        std::function<void(bool)> set,
        std::function<bool()> shouldEnable = nullptr
    ) {
        auto off = createViewToggleSpr(frame, false);
        auto on  = createViewToggleSpr(frame, true);
        auto toggler = BEMenuItemToggler::create(off, on, get, set, shouldEnable);
        return toggler;
    }
    BEMenuItemToggler* createViewToggleGV(
        const char* frame, const char* gv,
        std::function<bool()> shouldEnable = nullptr
    ) {
        return this->createViewToggle(
            frame,
            [gv]() { return GameManager::get()->getGameVariable(gv); },
            [this, gv](bool enabled) {
                GameManager::get()->setGameVariable(gv, enabled);
                m_editorLayer->updateOptions();
                static_cast<ViewTabUI*>(EditorUI::get())->updateViewTab();
            },
            shouldEnable
        );
    }
    BEMenuItemToggler* createViewToggleMSV(
        const char* frame, const char* modSavedValue,
        bool defaultValue = false, std::function<bool()> shouldEnable = nullptr
    ) {
        return this->createViewToggle(
            frame,
            [modSavedValue, defaultValue]() {
                return Mod::get()->template getSavedValue<bool>(modSavedValue, defaultValue);
            },
            [modSavedValue](bool enabled) {
                Mod::get()->setSavedValue(modSavedValue, enabled);
                static_cast<ViewTabUI*>(EditorUI::get())->updateViewTab();
            },
            shouldEnable
        );
    }
    BEMenuItemToggler* createViewToggleMS(
        const char* frame, const char* modSettingKey,
        std::function<bool()> shouldEnable = nullptr
    ) {
        return this->createViewToggle(
            frame,
            [modSettingKey]() { return Mod::get()->getSettingValue<bool>(modSettingKey); },
            [modSettingKey](bool enabled) {
                Mod::get()->setSettingValue(modSettingKey, enabled);
                static_cast<ViewTabUI*>(EditorUI::get())->updateViewTab();
            },
            shouldEnable
        );
    }

    void updateViewTab() {
        if (auto tabs = alpha::editor_tabs::nodeForTab("view"_spr)) {
            if (auto bbar = static_cast<EditButtonBar*>(tabs.unwrap().data())) {
                for (auto toggle : CCArrayExt<BEMenuItemToggler*>(bbar->m_buttonArray)) {
                    toggle->updateState();
                }
            }
        }
    }

    $override
    bool init(LevelEditorLayer* lel) {
        if (!EditorUI::init(lel))
            return false;
        
        if (!Mod::get()->template getSettingValue<bool>("view-menu")) {
            return true;
        }

        alpha::editor_tabs::changeModeSprites(false);
        
        auto winSize = CCDirector::get()->getWinSize();
        
        // Make a bit space for new style menu since the old one is a tiny bit cramped
        if (auto left = this->getChildByType<CCSprite>(1), right = this->getChildByType<CCSprite>(2); left && right) {
            left->setPositionX(winSize.width - right->getPositionX());
        }
        for (auto& child : CCArrayExt<CCNode*>(m_pChildren)) {
            if (auto bar = typeinfo_cast<EditButtonBar*>(child)) {
                bar->setPositionX(bar->getPositionX() + 5);
            }
        }

        alpha::editor_tabs::addTab(
            "view"_spr, "view"_spr,
            [this, winSize] {
                // Create buttons
                std::vector<Ref<CCNode>> btns;

                // For some reason using a class that inherits from CCMenuItemToggler 
                // for the view toggles was causing super weird crashes, so will have 
                // to make due with this :/

                btns.push_back(this->createViewToggle(
                    "v-rotation.png"_spr,
                    [] { return GameManager::get()->getGameVariable("0118"); },
                    [this](bool) {
                        fakeEditorPauseLayer(m_editorLayer)->togglePreviewAnim(nullptr);
                    }
                ));
                btns.push_back(this->createViewToggle(
                    "v-particles.png"_spr,
                    [] { return GameManager::get()->getGameVariable("0117"); },
                    [this](bool) { m_editorLayer->updatePreviewParticles(); }
                ));
                btns.push_back(this->createViewToggleGV("v-shaders.png"_spr, "0158"));
                btns.push_back(this->createViewToggle(
                    "v-ldm.png"_spr,
                    [] { return Mod::get()->getSavedValue("hide-ldm", false); },
                    [this](bool) {
                        for (auto obj : CCArrayExt<GameObjectExtra*>(m_editorLayer->m_objects)) {
                            obj->updateVisibility();
                        }
                    }
                ));
                btns.push_back(this->createViewToggle(
                    "v-preview-mode.png"_spr,
                    [] { return GameManager::get()->getGameVariable("0036"); },
                    [this](bool) {
                        // Let's not be funny and ruin everyone's levels
                        if (m_editorLayer->m_playbackMode != PlaybackMode::Not) {
                            // Why was this being called separately? `onStopPlaytest` already calls it
                            // m_editorLayer->resetMovingObjects();
                            this->onStopPlaytest(m_playtestBtn);
                        }
                        m_editorLayer->updateEditorMode();
                    }
                ));
                btns.push_back(this->createViewToggle(
                    "v-bpm-lines.png"_spr,
                    [] { return GameManager::get()->m_showSongMarkers; },
                    [](bool enable) {
                        GameManager::get()->m_showSongMarkers = enable;
                    }
                ));
                btns.push_back(this->createViewToggleMSV("v-position-line.png"_spr, "pos-line"));
                btns.push_back(this->createViewToggleGV("v-duration-lines.png"_spr, "0058"));
                btns.push_back(this->createViewToggleGV("v-effect-lines.png"_spr, "0043"));
                btns.push_back(this->createViewToggle(
                    "v-ground.png"_spr,
                    [] { return GameManager::get()->getGameVariable("0037"); },
                    [this](bool enable) {
                        m_editorLayer->m_groundLayer->setVisible(enable);
                    }
                ));
                btns.push_back(this->createViewToggleGV("v-grid.png"_spr, "0038"));
                btns.push_back(this->createViewToggleMSV("v-dash-lines.png"_spr, "show-dash-lines"));
                btns.push_back(this->createViewToggleGV("v-hitboxes.png"_spr, "0045"));

                // todo: toggling any of these currently does not update the rest of the buttons
                auto indToggle = this->createViewToggleMS("v-indicators.png"_spr, "trigger-indicators");
                btns.push_back(indToggle);
                btns.push_back(this->createViewToggleMSV(
                    "v-indicators-trigger-to-trigger.png"_spr,
                    "trigger-indicators-trigger-to-trigger",
                    false,
                    [indToggle] { return indToggle->isToggled(); }
                ));
                // todo: show this one as disabled if there are too many objects
                auto showAll = this->createViewToggleMSV(
                    "v-indicators-all.png"_spr,
                    "trigger-indicators-show-all",
                    false,
                    [indToggle] { return indToggle->isToggled(); }
                );
                btns.push_back(showAll);
                btns.push_back(this->createViewToggleMSV(
                    "v-indicators-cluster-outline.png"_spr,
                    "trigger-indicators-cluster-outlines",
                    false,
                    [indToggle, showAll] { return indToggle->isToggled() && showAll->isToggled(); }
                ));
                btns.push_back(this->createViewToggleMSV(
                    "v-indicators-blocky.png"_spr,
                    "trigger-indicators-blocky",
                    false,
                    [indToggle] { return indToggle->isToggled(); }
                ));
                return alpha::editor_tabs::createEditButtonBar(btns);
            },
            [] {
                return CCSprite::createWithSpriteFrameName("tab-view.png"_spr);
            },
            [this](bool shown, CCNode*) {
                if (shown) {
                    this->updateViewTab();
                }
            }
        );

        if (auto menu = this->getChildByID("toolbar-categories-menu")) {
            m_fields->viewModeBtn = CCMenuItemSpriteExtra::create(
                CCNode::create(), this, menu_selector(ViewTabUI::onToggleView)
            );
            m_fields->viewModeBtn->setID("view-button"_spr);
            m_fields->viewModeBtn->setTag(4);
            menu->addChild(m_fields->viewModeBtn);

            this->updateModeSprites();

            menu->setContentSize({ 90, 90 });
            if (auto otherSide = this->getChildByID("toolbar-toggles-menu")) {
                menu->setPositionX(winSize.width - otherSide->getPositionX());
            }
            menu->setLayout(RowLayout::create()
                ->setCrossAxisOverflow(false)
                ->setGrowCrossAxis(true)
            );
        }

        this->addEventListener(
            KeybindSettingPressedEventV3(Mod::get(), "keybind-view-mode"),
            [=, this](Keybind const&, bool down, bool, double) {
                if (down && m_editorLayer->m_playbackMode == PlaybackMode::Not) {
                    this->onToggleView(nullptr);
                }
            }
        );

        this->updateViewTab();

        alpha::editor_tabs::addModeSwitchCallback([this](ZStringView) {
            this->updateModeSprites();
        });

        m_fields->onUIHide = UIShowEvent(this).listen([this](bool show) {
            m_buildModeBtn->getParent()->getChildByTag(4)->setVisible(show);
        });

        return true;
    }

    void onToggleView(CCObject*) {
        alpha::editor_tabs::switchMode("view"_spr);
        this->updateModeSprites();
    }

    $override
    void toggleMode(CCObject* sender) {
        EditorUI::toggleMode(sender);
        this->updateModeSprites();
    }

    #ifdef GEODE_IS_MACOS // toggleMode is inlined into onPlaytest on macOS
    $override
    void onPlaytest(CCObject* sender) {
        auto playbackMode = m_editorLayer->m_playbackMode;
        EditorUI::onPlaytest(sender);
        if (!m_isPaused && playbackMode != PlaybackMode::Playing) {
            this->updateModeSprites();
        }
    }
    #endif

    $override
    void selectObject(GameObject* obj, bool filter) {
        if (!static_cast<GameObjectExtra*>(obj)->shouldHide()) {
            EditorUI::selectObject(obj, filter);
        }
    }
    $override
    void selectObjects(CCArray* objs, bool ignoreFilters) {
        // filter out LDM objects
        if (objs) {
            unsigned int objCount = objs->count();
            for (unsigned int i = 0; i < objCount; i++) {
                auto obj = static_cast<GameObjectExtra*>(objs->objectAtIndex(i));
                if (obj->shouldHide()) {
                    objs->removeObjectAtIndex(i, false);
                }
                i -= 1;
                objCount -= 1;
            }
        }
        EditorUI::selectObjects(objs, ignoreFilters);
    }
};

class $modify(DrawGridLayer) {
    $override
    void draw() {
        bool origBool = m_editorLayer->m_previewMode;
        m_editorLayer->m_previewMode = Mod::get()->template getSavedValue<bool>("pos-line");
        DrawGridLayer::draw();
        m_editorLayer->m_previewMode = origBool;
    }
};

class $modify(LevelEditorLayer) {
    void updateOptions() {
        LevelEditorLayer::updateOptions();
        if (m_editorUI) static_cast<ViewTabUI*>(m_editorUI)->updateViewTab();
    }
};
