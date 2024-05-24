#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
#include <Geode/modify/GameObject.hpp>
#include <Geode/modify/DrawGridLayer.hpp>
#include <Geode/modify/FMODAudioEngine.hpp>

#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/binding/ButtonSprite.hpp>
#include <Geode/binding/EditButtonBar.hpp>
#include <Geode/utils/cocos.hpp>
#include <geode.custom-keybinds/include/Keybinds.hpp>
#include <utils/HandleUIHide.hpp>

using namespace geode::prelude;
using namespace keybinds;

template <class F>
class CCFunction : public CCObject {
protected:
    MiniFunction<F> m_func;

public:
    template <class F2>
    static CCFunction* create(F2&& func) {
        auto ret = new CCFunction();
        ret->m_func = func;
        ret->autorelease();
        return ret;
    }

    template <class... Args>
    auto invoke(Args&&... args) {
        return m_func(std::forward<Args>(args)...);
    }
};

class $modify(GameObjectExtra, GameObject) {
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
        return m_isHighDetail && Mod::get()->template getSavedValue<bool>("hide-ldm", false);
    }
};

struct $modify(ViewTabUI, EditorUI) {
    struct Fields {
        CCNode* viewModeBtn;
        OnUIHide onUIHide;
    };

    static void onModify(auto& self) {
        (void)self.setHookPriority("EditorUI::selectObject",  3000);
        (void)self.setHookPriority("EditorUI::selectObjects", 3000);
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
                    CCSprite::createWithSpriteFrameName(spr), 50, true, 50, bg, .6f
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

    ButtonSprite* createViewToggleSpr(const char* frame, bool selected) {
        auto top = CCSprite::createWithSpriteFrameName(frame);
        return ButtonSprite::create(
            top, 50, 0, 50, 1, true, (selected ? "GJ_button_02.png" : "GJ_button_01.png"), true
        );
    }
    CCMenuItemToggler* createViewToggle(const char* frame, auto get, auto set) {
        auto off = createViewToggleSpr(frame, false);
        auto on  = createViewToggleSpr(frame, true);
        auto toggler = CCMenuItemToggler::create(off, on, this, menu_selector(ViewTabUI::onViewToggle));
        toggler->setUserObject("getter", CCFunction<bool()>::create(get));
        toggler->setUserObject("setter", CCFunction<void(bool)>::create(set));
        return toggler;
    }
    CCMenuItemToggler* createViewToggleGV(const char* frame, const char* gv, MiniFunction<void(bool)> postSet = nullptr) {
        auto off = createViewToggleSpr(frame, false);
        auto on  = createViewToggleSpr(frame, true);
        auto toggler = CCMenuItemToggler::create(off, on, this, menu_selector(ViewTabUI::onViewToggle));
        toggler->setUserObject("getter", CCFunction<bool()>::create([gv]() {
            return GameManager::get()->getGameVariable(gv);
        }));
        toggler->setUserObject("setter", CCFunction<void(bool)>::create([this, gv, postSet](bool enabled) {
            GameManager::get()->setGameVariable(gv, enabled);
            postSet ? postSet(enabled) : m_editorLayer->updateOptions();
        }));
        return toggler;
    }
    CCMenuItemToggler* createViewToggleMSV(const char* frame, const char* modSavedValue, MiniFunction<void(bool)> postSet = nullptr) {
        auto off = createViewToggleSpr(frame, false);
        auto on  = createViewToggleSpr(frame, true);
        auto toggler = CCMenuItemToggler::create(off, on, this, menu_selector(ViewTabUI::onViewToggle));
        toggler->setUserObject("getter", CCFunction<bool()>::create([modSavedValue]() {
            return Mod::get()->template getSavedValue<bool>(modSavedValue);
        }));
        toggler->setUserObject("setter", CCFunction<void(bool)>::create([modSavedValue, postSet](bool enabled) {
            Mod::get()->setSavedValue(modSavedValue, enabled);
            if (postSet) {
                postSet(enabled);
            }
        }));
        return toggler;
    }
    void onViewToggle(CCObject* sender) {
        auto toggle = static_cast<CCMenuItemToggler*>(sender);
        auto setter = static_cast<CCFunction<void(bool)>*>(toggle->getUserObject("setter"));
        setter->invoke(!toggle->isToggled());
    }

    void updateViewTab() {
        if (auto bbar = static_cast<EditButtonBar*>(this->getChildByID("view-tab"_spr))) {
            bbar->reloadItems(
                GameManager::get()->getIntGameVariable("0049"),
                GameManager::get()->getIntGameVariable("0050")
            );
            for (auto toggle : CCArrayExt<CCMenuItemToggler*>(bbar->m_buttonArray)) {
                auto func = static_cast<CCFunction<bool()>*>(toggle->getUserObject("getter"));
                toggle->toggle(func->invoke());
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
        
        auto winSize = CCDirector::get()->getWinSize();
        
        // Make a bit space for new style menu since the old one is a tiny bit cramped
        if (auto left = getChildOfType<CCSprite>(this, 1), right = getChildOfType<CCSprite>(this, 2); left && right) {
            left->setPositionX(winSize.width - right->getPositionX());
        }
        for (auto& child : CCArrayExt<CCNode*>(m_pChildren)) {
            if (auto bar = typeinfo_cast<EditButtonBar*>(child)) {
                bar->setPositionX(bar->getPositionX() + 5);
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

            menu->setContentSize({ 90, 90 });
            if (auto otherSide = this->getChildByID("toolbar-toggles-menu")) {
                menu->setPositionX(winSize.width - otherSide->getPositionX());
            }
            menu->setLayout(RowLayout::create()
                ->setCrossAxisOverflow(false)
                ->setGrowCrossAxis(true)
            );
        }

    #ifdef GEODE_IS_WINDOWS
        this->template addEventListener<InvokeBindFilter>([=](InvokeBindEvent* event) {
            if (event->isDown() && m_editorLayer->m_playbackMode == PlaybackMode::Not) {
                this->toggleMode(m_fields->viewModeBtn);
            }
            return ListenerResult::Propagate;
        }, "view-mode"_spr);
    #endif

        // Create buttons
        auto btns = CCArray::create();

        // For some reason using a class that inherits from CCMenuItemToggler 
        // for the view toggles was causing super weird crashes, so will have 
        // to make due with this :/

        btns->addObject(this->createViewToggleGV("v_rotate.png"_spr, "0118", [this](bool) {
            m_editorLayer->updatePreviewAnim();
        }));
        btns->addObject(this->createViewToggleGV("v_particles.png"_spr, "0117", [this](bool) {
            m_editorLayer->updatePreviewParticles();
        }));
        btns->addObject(this->createViewToggleGV("v_shaders.png"_spr, "0158"));
        btns->addObject(this->createViewToggleMSV("v_ldm.png"_spr, "hide-ldm", [this](bool) {
            for (auto obj : CCArrayExt<GameObjectExtra*>(m_editorLayer->m_objects)) {
                obj->updateVisibility();
            }
        }));
        btns->addObject(this->createViewToggleGV("v_prevmode.png"_spr, "0036", [this](bool) {
            // Let's not be funny and ruin everyone's levels
            if (m_editorLayer->m_playbackMode != PlaybackMode::Not) {
                m_editorLayer->resetMovingObjects();
                this->onStopPlaytest(m_playtestBtn);
            }
            m_editorLayer->updateEditorMode();
        }));
        btns->addObject(this->createViewToggle(
            "v_bpm_line.png"_spr,
            [] { return GameManager::get()->m_showSongMarkers; },
            [](bool enable) {
                GameManager::get()->m_showSongMarkers = enable;
            }
        ));
        btns->addObject(this->createViewToggleMSV("v_pos_line.png"_spr, "pos-line"));
        btns->addObject(this->createViewToggleGV("v_dur_line.png"_spr, "0058"));
        btns->addObject(this->createViewToggleGV("v_eff_line.png"_spr, "0043"));
        btns->addObject(this->createViewToggleGV("v_ground.png"_spr, "0037", [this](bool enable) {
            m_editorLayer->m_groundLayer->setVisible(enable);
        }));
        btns->addObject(this->createViewToggleGV("v_grid.png"_spr, "0038"));
        btns->addObject(this->createViewToggleMSV("v_dash.png"_spr, "show-dash-lines"));

        auto buttonBar = EditButtonBar::create(
            btns,
            ccp(winSize.width / 2 - 5, CCDirector::get()->getScreenBottom() + m_toolbarHeight - 6),
            m_tabsArray->count(), false,
            GameManager::get()->getIntGameVariable("0049"),
            GameManager::get()->getIntGameVariable("0050")
        );
        buttonBar->setID("view-tab"_spr);
        // Need to set a tag too so GD doesn't accidentally grab this tab 
        // when doing getChildByTag for its tabs
        buttonBar->setTag(-1);
        buttonBar->setVisible(m_selectedMode == 4);
        this->addChild(buttonBar, 10);

        this->updateViewTab();

        m_fields->onUIHide.setFilter(this);
        m_fields->onUIHide.bind([this](UIShowEvent* ev) {
            m_fields->viewModeBtn->setVisible(ev->show);
            this->getChildByID("view-tab"_spr)->setVisible(ev->show && m_selectedMode == 4);
            m_buildModeBtn->getParent()->getChildByTag(4)->setVisible(ev->show);
        });

        return true;
    }

    void toggleMode(CCObject* sender) {
        EditorUI::toggleMode(sender);
        if (auto viewBtnBar = this->getChildByID("view-tab"_spr)) {
            // this->resetUI();
            this->updateModeSprites();
            
            viewBtnBar->setVisible(m_selectedMode == 4);
        }
    }

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
            size_t objCount = objs->count();
            for (size_t i = 0; i < objCount; i++) {
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

class $modify(EditorPauseLayer) {
    $override
    void onResume(CCObject* pSender) {
        EditorPauseLayer::onResume(pSender);
        static_cast<ViewTabUI*>(EditorUI::get())->updateViewTab();
    }
};

#ifdef GEODE_IS_WINDOWS
$execute {
    BindManager::get()->registerBindable(BindableAction(
        "view-mode"_spr,
        "View Mode",
        "Toggle the View Tab",
        { Keybind::create(KEY_Four) },
        Category::EDITOR_UI
    ), "robtop.geometry-dash/delete-mode");
}
#endif
