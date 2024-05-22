#include <Geode/modify/EditButtonBar.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <utils/EditCommandExt.hpp>
#include <utils/HandleUIHide.hpp>

using namespace geode::prelude;

static constexpr size_t MOVE_MENU_GROUPS_PER_PAGE = 4;

class MoveGroup : public CCMenu {
protected:
    bool init(
        CCMenuItemSpriteExtra* up,
        CCMenuItemSpriteExtra* down,
        CCMenuItemSpriteExtra* left,
        CCMenuItemSpriteExtra* right
    ) {
        if (!CCMenu::init())
            return false;
        
        this->ignoreAnchorPointForPosition(false);
        this->setAnchorPoint({ .5f, .5f });
        this->setContentSize({ 56, 28 });

        up->getNormalImage()->setScale(.75f);
        down->getNormalImage()->setScale(.7f);
        left->getNormalImage()->setScale(.75f);
        right->getNormalImage()->setScale(.75f);

        this->addChildAtPosition(up,    Anchor::Top,    ccp( 0, -5));
        this->addChildAtPosition(down,  Anchor::Bottom, ccp( 0,  5));
        this->addChildAtPosition(left,  Anchor::Left,   ccp( 10, 0));
        this->addChildAtPosition(right, Anchor::Right,  ccp(-10, 0));

        return true;
    }

public:
    static MoveGroup* create(
        CCMenuItemSpriteExtra* up,
        CCMenuItemSpriteExtra* down,
        CCMenuItemSpriteExtra* left,
        CCMenuItemSpriteExtra* right
    ) {
        auto ret = new MoveGroup();
        if (ret && ret->init(up, down, left, right)) {
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

        m_groupRow = CCNode::create();
        m_groupRow->setLayout(RowLayout::create()->setGap(3));
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

        this->updateButtonLayout();
        this->scheduleUpdate();
        
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

        // Add the buttons from the page to the group row
        m_groupRow->removeAllChildren();
        for (size_t i = 0; i < MOVE_MENU_GROUPS_PER_PAGE; i += 1) {
            auto pagedIndex = m_movePage * MOVE_MENU_GROUPS_PER_PAGE + i;
            if (pagedIndex >= m_groups.size()) {
                break;
            }
            m_groupRow->addChild(m_groups.at(pagedIndex));
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
    static void createOrUpdate(EditorUI* ui) {
        CustomEditMenu* result;

        if (auto existing = static_cast<CustomEditMenu*>(ui->getChildByID("custom-move-menu"_spr))) {
            existing->updateButtonLayout();
            result = existing;
        }
        else {
            auto winSize = CCDirector::get()->getWinSize();
            result = CustomEditMenu::create(ui);
            result->setID("custom-move-menu"_spr);
            result->setPosition(winSize.width / 2, 5);
            ui->addChild(result, 10);
            handleUIHideOnPlaytest(ui, result);
        }

        // Update visibility & scale
        result->setVisible(ui->m_editButtonBar->isVisible());
        result->setScale(ui->m_editButtonBar->getScale());
        ui->m_editButtonBar->setVisible(false);
    }

    void updateButtonLayout() {
        // Clear existing groups
        for (auto group : m_groups) {
            group->removeFromParent();
        }
        m_groups.clear();

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

            auto id = btn->getID();
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
                // Add this group to the list
                groups.push_back(std::make_pair(modID, group));
            }

            // We want to preserve the order of non-move buttons, so put 
            // everything in the list
            btnsWithIDs.push_back(std::make_pair(btn->getID(), btn));
        }

        // Group up move buttons
        for (auto [modID, group] : std::move(groups)) {
            // Try to find all the directions of the buttons; if any are missing, 
            // the move buttons will go into the bottom row
            auto up    = findBtnByID(modID + "move-up-" + group);
            auto down  = findBtnByID(modID + "move-down-" + group);
            auto left  = findBtnByID(modID + "move-left-" + group);
            auto right = findBtnByID(modID + "move-right-" + group);
            if (
                up && down && left && right &&
                // `groups` has duplicates
                !up->getParent() && !down->getParent() && !left->getParent() && !right->getParent()
            ) {
                m_groups.push_back(MoveGroup::create(up, down, left, right));
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
    }
};

class $modify(EditorUI) {
    $override
    void createMoveMenu() {
        EditorUI::createMoveMenu();

        this->addMoveButton("move-up-quarter-button"_spr,    "edit_upBtn5_001.png",    "1/4", EditCommandExt::QuarterUp);
        this->addMoveButton("move-down-quarter-button"_spr,  "edit_downBtn5_001.png",  "1/4", EditCommandExt::QuarterDown);
        this->addMoveButton("move-left-quarter-button"_spr,  "edit_leftBtn5_001.png",  "1/4", EditCommandExt::QuarterLeft);
        this->addMoveButton("move-right-quarter-button"_spr, "edit_rightBtn5_001.png", "1/4", EditCommandExt::QuarterRight);

        this->addMoveButton("move-up-eighth-button"_spr,    "edit_upBtn5_001.png",    "1/8", EditCommandExt::EighthUp);
        this->addMoveButton("move-down-eighth-button"_spr,  "edit_downBtn5_001.png",  "1/8", EditCommandExt::EighthDown);
        this->addMoveButton("move-left-eighth-button"_spr,  "edit_leftBtn5_001.png",  "1/8", EditCommandExt::EighthLeft);
        this->addMoveButton("move-right-eighth-button"_spr, "edit_rightBtn5_001.png", "1/8", EditCommandExt::EighthRight);

        this->addMoveButton("move-up-unit-button"_spr,    "edit_upBtn_001.png",    "Unit", EditCommandExt::UnitUp);
        this->addMoveButton("move-down-unit-button"_spr,  "edit_downBtn_001.png",  "Unit", EditCommandExt::UnitDown);
        this->addMoveButton("move-left-unit-button"_spr,  "edit_leftBtn_001.png",  "Unit", EditCommandExt::UnitLeft);
        this->addMoveButton("move-right-unit-button"_spr, "edit_rightBtn_001.png", "Unit", EditCommandExt::UnitRight);

        m_editButtonBar->reloadItems(
            GameManager::get()->getIntGameVariable("0049"),
            GameManager::get()->getIntGameVariable("0050")
        );

        if (Mod::get()->template getSettingValue<bool>("new-edit-menu")) {
            CustomEditMenu::createOrUpdate(this);
        }
    }

    void addMoveButton(const char* id, const char* spr, const char* text, EditCommand command) {
        auto btn = this->getSpriteButton(spr, menu_selector(EditorUI::moveObjectCall), nullptr, .9f);
        btn->setID(id);
        btn->setTag(static_cast<int>(command));

        if (text) {
            auto label = CCLabelBMFont::create(text, "bigFont.fnt");
            label->setScale(.35f);
            label->setZOrder(10);
            btn->addChildAtPosition(label, Anchor::Bottom, ccp(0, 11));
        }

        m_editButtonBar->m_buttonArray->addObject(btn);
    }

    $override
    void resetUI() {
        EditorUI::resetUI();
        if (Mod::get()->template getSettingValue<bool>("new-edit-menu")) {
            CustomEditMenu::createOrUpdate(this);
        }
    }
};

class $modify(EditButtonBar) {
    $override
    void loadFromItems(CCArray* items, int r, int c, bool unkBool) {
        EditButtonBar::loadFromItems(items, r, c, unkBool);
        if (Mod::get()->template getSettingValue<bool>("new-edit-menu") && this->getID() == "edit-button-bar") {
            CustomEditMenu::createOrUpdate(EditorUI::get());
        }
    }
};
