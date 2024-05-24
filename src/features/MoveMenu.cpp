#include <Geode/modify/EditButtonBar.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <utils/EditCommandExt.hpp>
#include <utils/HandleUIHide.hpp>
#include <utils/Pro.hpp>

using namespace geode::prelude;

static constexpr size_t MOVE_MENU_GROUPS_PER_PAGE = 4;

class MoveGroup : public CCMenu {
protected:
    bool init(
        std::string const& name,
        CCMenuItemSpriteExtra* up,
        CCMenuItemSpriteExtra* down,
        CCMenuItemSpriteExtra* left,
        CCMenuItemSpriteExtra* right
    ) {
        if (!CCMenu::init())
            return false;
        
        this->ignoreAnchorPointForPosition(false);
        this->setAnchorPoint({ .5f, .5f });
        this->setContentSize({ 60, 30 });

        up->setScale(.5f);
        down->setScale(.5f);
        left->setScale(.5f);
        right->setScale(.5f);
        up->m_baseScale = .5f;
        down->m_baseScale = .5f;
        left->m_baseScale = .5f;
        right->m_baseScale = .5f;

        this->addChildAtPosition(up,    Anchor::Top,    ccp( 0, -5));
        this->addChildAtPosition(down,  Anchor::Bottom, ccp( 0,  5));
        this->addChildAtPosition(left,  Anchor::Left,   ccp( 10, 0));
        this->addChildAtPosition(right, Anchor::Right,  ccp(-10, 0));

        if (!name.empty()) {
            auto label = CCLabelBMFont::create(name.c_str(), "bigFont.fnt");
            label->setScale(.25f);
            this->addChildAtPosition(label, Anchor::Top, ccp(0, 10));
        }

        return true;
    }

public:
    static MoveGroup* create(
        std::string const& name,
        CCMenuItemSpriteExtra* up,
        CCMenuItemSpriteExtra* down,
        CCMenuItemSpriteExtra* left,
        CCMenuItemSpriteExtra* right
    ) {
        auto ret = new MoveGroup();
        if (ret && ret->init(name, up, down, left, right)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};

class CustomEditMenu : public CCNode {
protected:
    EditorUI* m_editorUI;
    OnUIHide m_onUIHide;
    CCNode* m_groupRow;
    CCMenu* m_groupPageControlsMenu;
    CCMenu* m_bottomRow;
    std::vector<Ref<MoveGroup>> m_groups;
    size_t m_movePage = 0;

    bool init(EditorUI* ui) {
        if (!CCNode::init())
            return false;

        auto winSize = CCDirector::get()->getWinSize();
        this->setContentSize(ccp(winSize.width / 2 - 5, CCDirector::get()->getScreenBottom() + ui->m_toolbarHeight - 6));
        this->setAnchorPoint(ccp(.5f, 0));

        m_editorUI = ui;
        m_onUIHide.setFilter(UIShowFilter(ui));
        m_onUIHide.bind([this](UIShowEvent* ev) {
            this->updateMenu(ev->show);
        });

        m_groupRow = CCNode::create();
        m_groupRow->setLayout(RowLayout::create()->setGap(3));
        m_groupRow->getLayout()->ignoreInvisibleChildren(true);
        m_groupRow->setContentWidth(m_obContentSize.width - 40);
        m_groupRow->setAnchorPoint(ccp(.5f, .5f));
        this->addChildAtPosition(m_groupRow, Anchor::Center, ccp(0, 10));

        m_groupPageControlsMenu = CCMenu::create();
        m_groupPageControlsMenu->ignoreAnchorPointForPosition(false);
        m_groupPageControlsMenu->setAnchorPoint(ccp(.5f, .5f));
        m_groupPageControlsMenu->setContentSize(m_obContentSize);

        auto groupLastPageSpr = CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");
        groupLastPageSpr->setScale(.5f);
        auto groupLastPageBtn = CCMenuItemSpriteExtra::create(
            groupLastPageSpr, this, menu_selector(CustomEditMenu::onMovePage)
        );
        groupLastPageBtn->setTag(-1);
        m_groupPageControlsMenu->addChildAtPosition(groupLastPageBtn, Anchor::Left, ccp(10, 0));

        auto groupNextPageSpr = CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");
        groupNextPageSpr->setScale(.5f);
        groupNextPageSpr->setFlipX(true);
        auto groupNextPageBtn = CCMenuItemSpriteExtra::create(
            groupNextPageSpr, this, menu_selector(CustomEditMenu::onMovePage)
        );
        groupNextPageBtn->setTag(1);
        m_groupPageControlsMenu->addChildAtPosition(groupNextPageBtn, Anchor::Right, ccp(-10, 0));

        this->addChildAtPosition(m_groupPageControlsMenu, Anchor::Center, ccp(0, 10));

        m_bottomRow = CCMenu::create();
        m_bottomRow->setLayout(
            RowLayout::create()
                ->setDefaultScaleLimits(.1f, .5f)
        );
        m_bottomRow->setContentWidth(m_obContentSize.width);
        this->addChildAtPosition(m_bottomRow, Anchor::Bottom, ccp(0, 15));

        this->updateMenu();
        
        return true;
    }

    void onMovePage(CCObject* sender) {
        if (m_groups.empty()) return;
        auto lastPage = (m_groups.size() - 1) / MOVE_MENU_GROUPS_PER_PAGE;
        if (sender->getTag() > 0) {
            m_movePage = m_movePage < lastPage ? m_movePage + 1 : 0;
        }
        else {
            m_movePage = m_movePage > 0 ? m_movePage - 1 : lastPage;
        }
        this->updatePage();
    }
    void updatePage() {
        // Just in case updateButtonLayout() somehow removes some groups, these 
        // make sure there's no crash
        if (m_groups.empty()) return;
        auto lastPage = (m_groups.size() - 1) / MOVE_MENU_GROUPS_PER_PAGE;
        if (m_movePage > lastPage) {
            m_movePage = lastPage;
        }

        // Show only the buttons on this page
        for (size_t i = 0; i < m_groups.size(); i += 1) {
            auto pagedIndex = m_movePage * MOVE_MENU_GROUPS_PER_PAGE;
            m_groups.at(i)->setVisible(i >= pagedIndex && i < pagedIndex + MOVE_MENU_GROUPS_PER_PAGE);
        }
        m_groupRow->updateLayout();
    }

public:
    static CustomEditMenu* create(EditorUI* ui) {
        auto ret = new CustomEditMenu();
        if (ret && ret->init(ui)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
    static CustomEditMenu* get(EditorUI* ui, bool create = false) {
        if (!ui) return nullptr;

        if (auto menu = static_cast<CustomEditMenu*>(ui->getChildByID("custom-move-menu"_spr))) {
            menu->updateMenu();
            return menu;
        }

        if (!Mod::get()->template getSettingValue<bool>("new-edit-menu") || isProUIEnabled()) {
            return nullptr;
        }

        if (create) {
            auto winSize = CCDirector::get()->getWinSize();

            auto nuevo = CustomEditMenu::create(ui);
            nuevo->setID("custom-move-menu"_spr);
            nuevo->setPosition(winSize.width / 2, 5);
            ui->addChild(nuevo, 10);

            return nuevo;
        }
        return nullptr;
    }

    void updateMenu(bool show = true) {
        // Hide the original
        m_editorUI->m_editButtonBar->setVisible(false);

        // If we're not showing, we don't need to update the menu layout, just return
        if (!show) {
            this->setVisible(false);
            return;
        }

        // First element is mod ID, second is group
        std::vector<std::pair<std::string, std::string>> groups;
        std::vector<std::pair<std::string, Ref<CCMenuItemSpriteExtra>>> btnsWithIDs;

        auto findBtnByID = [&](auto id) {
            for (auto& [comp, btn] : btnsWithIDs) {
                if (comp == id) {
                    return btn;
                }
            }
            return Ref<CCMenuItemSpriteExtra>(nullptr);
        };
        
        // Move all buttons from the normal edit button bar to this one
        for (auto btn : CCArrayExt<CCMenuItemSpriteExtra*>(m_editorUI->m_editButtonBar->m_buttonArray)) {
            auto ref = Ref(btn);
            btn->removeFromParent();

            // If this button has already been added, skip
            auto id = btn->getID();
            if (m_bottomRow->getChildByID(id)) {
                continue;
            }

            auto modID = std::string();
            // Remove any mod ID prefix to support other mods that may add 
            // their own move buttons (also needed for BE's own extra move 
            // buttons)
            if (id.find('/') != std::string::npos) {
                modID = id.substr(0, id.find('/') + 1);
                id = id.substr(id.find('/') + 1);
            }

            // If this is a move button, extract the group out of it
            if (id.starts_with("move-")) {
                // Remove the "move-" from the group
                auto group = id.substr(id.find('-') + 1);
                // Remove whatever direction it is from the group
                group = group.substr(group.find('-') + 1);

                // If this group has already been added, skip
                for (auto& g : m_groups) {
                    if (g->getID() == group) {
                        goto this_group_has_already_been_handled_fully_before;
                    }
                }
                for (auto& [_, g] : groups) {
                    if (g == group) {
                        goto this_group_has_already_been_added_to_the_tba_list;
                    }
                }

                // Add this group to the list
                groups.push_back(std::make_pair(modID, group));
            }

        this_group_has_already_been_added_to_the_tba_list:;

            // We want to preserve the order of non-move buttons, so put 
            // everything in the list
            btnsWithIDs.push_back(std::make_pair(btn->getID(), btn));

        this_group_has_already_been_handled_fully_before:;
        }
        m_editorUI->m_editButtonBar->m_buttonArray->removeAllObjects();

        // Group up move buttons
        for (auto [modID, group] : std::move(groups)) {
            // Try to find all the directions of the buttons; if any are missing, 
            // the move buttons will go into the bottom row
            auto up    = findBtnByID(modID + "move-up-" + group);
            auto down  = findBtnByID(modID + "move-down-" + group);
            auto left  = findBtnByID(modID + "move-left-" + group);
            auto right = findBtnByID(modID + "move-right-" + group);
            if (up && down && left && right) {
                std::string name;
                switch (hash(group.c_str())) {
                    case hash("half-button"):    name = "1/2";   break;
                    case hash("quarter-button"): name = "1/4";   break;
                    case hash("eighth-button"):  name = "1/8";   break;
                    case hash("unit-button"):    name = "Pixel"; break;
                    default: break;
                }
                auto created = MoveGroup::create(name, up, down, left, right);
                created->setID(group);
                m_groups.push_back(created);
                m_groupRow->addChild(created);
            }
        }

        // Add the rest of the buttons to the bottom row
        for (auto [_, leftover] : std::move(btnsWithIDs)) {
            // If this button hasn't been added to any parent yet, add it to 
            // the bottom row
            if (!leftover->getParent()) {
                m_bottomRow->addChild(leftover);
            }
        }

        this->updatePage();
        m_bottomRow->updateLayout();

        this->setVisible(m_editorUI->m_selectedMode == 3);
        this->setScale(m_editorUI->m_editButtonBar->getScale());
    }
};

class $modify(EditorUI) {
    $override
    bool init(LevelEditorLayer* editor) {
        if (!EditorUI::init(editor))
            return false;
        
        // The reason this is all done in init() rather than createMoveMenu() 
        // is that we want NodeIDs to have added the IDs first

        this->addMoveButton("move-up-quarter-button"_spr,    "edit_upBtn5_001.png",    EditCommandExt::QuarterUp);
        this->addMoveButton("move-down-quarter-button"_spr,  "edit_downBtn5_001.png",  EditCommandExt::QuarterDown);
        this->addMoveButton("move-left-quarter-button"_spr,  "edit_leftBtn5_001.png",  EditCommandExt::QuarterLeft);
        this->addMoveButton("move-right-quarter-button"_spr, "edit_rightBtn5_001.png", EditCommandExt::QuarterRight);

        this->addMoveButton("move-up-eighth-button"_spr,    "edit_upBtn5_001.png",    EditCommandExt::EighthUp);
        this->addMoveButton("move-down-eighth-button"_spr,  "edit_downBtn5_001.png",  EditCommandExt::EighthDown);
        this->addMoveButton("move-left-eighth-button"_spr,  "edit_leftBtn5_001.png",  EditCommandExt::EighthLeft);
        this->addMoveButton("move-right-eighth-button"_spr, "edit_rightBtn5_001.png", EditCommandExt::EighthRight);

        this->addMoveButton("move-up-unit-button"_spr,    "edit_upBtn_001.png",    EditCommandExt::UnitUp);
        this->addMoveButton("move-down-unit-button"_spr,  "edit_downBtn_001.png",  EditCommandExt::UnitDown);
        this->addMoveButton("move-left-unit-button"_spr,  "edit_leftBtn_001.png",  EditCommandExt::UnitLeft);
        this->addMoveButton("move-right-unit-button"_spr, "edit_rightBtn_001.png", EditCommandExt::UnitRight);

        m_editButtonBar->reloadItems(
            GameManager::get()->getIntGameVariable("0049"),
            GameManager::get()->getIntGameVariable("0050")
        );

        // Create the custom edit menu
        (void)CustomEditMenu::get(this, true);

        return true;
    }

    void addMoveButton(const char* id, const char* spr, EditCommand command) {
        auto btn = this->getSpriteButton(spr, menu_selector(EditorUI::moveObjectCall), nullptr, .9f);
        btn->setID(id);
        btn->setTag(static_cast<int>(command));
        m_editButtonBar->m_buttonArray->addObject(btn);
    }

    $override
    void toggleMode(CCObject* sender) {
        EditorUI::toggleMode(sender);
        if (auto menu = CustomEditMenu::get(this)) {
            menu->updateMenu();
        }
    }
    $override
    void resetUI() {
        EditorUI::resetUI();
        if (auto menu = CustomEditMenu::get(this)) {
            menu->updateMenu();
        }
    }
};

class $modify(EditButtonBar) {
    $override
    void loadFromItems(CCArray* items, int r, int c, bool unkBool) {
        EditButtonBar::loadFromItems(items, r, c, unkBool);
        if (this->getID() == "edit-tab-bar") {
            if (auto menu = CustomEditMenu::get(EditorUI::get())) {
                menu->updateMenu();
            }
        }
    }
};
