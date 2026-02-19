#include "GroupSummaryPopup.hpp"
#include <Geode/modify/EditorUI.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/binding/GJEffectManager.hpp>
#include <Geode/binding/DrawGridLayer.hpp>
#include <Geode/binding/EffectGameObject.hpp>
#include <Geode/binding/ObjectToolbox.hpp>
#include <Geode/binding/ButtonSprite.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>
#include <utils/Editor.hpp>
#include <Geode/utils/ranges.hpp>

static std::tuple<size_t, GroupSummaryFilter, std::string> LAST_PAGE = std::make_tuple(0, GroupSummaryFilter::All, "");

constexpr size_t ITEMS_PER_ROW = 5;
constexpr size_t ITEMS_PER_COLUMN = 4;
constexpr size_t ITEMS_PER_LIST = 8;

static CCSprite* createSpriteForTrigger(EffectGameObject* trigger, int group) {
    auto sprName = ObjectToolbox::sharedState()->intKeyToFrame(trigger->m_objectID);
    auto spr = CCSprite::createWithSpriteFrameName(sprName);
    spr->setScale(.65f);

    if (trigger->m_objectID == 1049) {
        auto toggleSpr = CCSprite::createWithSpriteFrameName("edit_eToggleBtn2_001.png");
        toggleSpr->setColor(trigger->m_activateGroup ? ccc3(0, 255, 127) : ccc3(255, 63, 63));
        spr->addChildAtPosition(toggleSpr, Anchor::Center);
    }

    std::string text = "";
    if (trigger->m_targetGroupID == group) {
        text = "T";
    }
    if (trigger->m_centerGroupID == group) {
        text = text.size() ? "T+C" : "C";
    }
    
    auto label = CCLabelBMFont::create(text.c_str(), "bigFont.fnt");
    label->setScale(.5f);
    spr->addChildAtPosition(label, Anchor::Center, -ccp(0, 5));

    return spr;
}

bool ObjectsListPopup::init(std::string const& title, GroupSummaryPopup* popup, CCArray* objs, bool render) {
    if (!Popup::init(280, 240))
        return false;

    this->setTitle(title);

    m_popup = popup;
    m_objs = objs;

    if (render) {
        auto bg = CCScale9Sprite::createWithSpriteFrameName("square-box.png"_spr);
        bg->setContentSize(ccp(240, 170));
        if (objs->count() > 1000) {
            auto label = CCLabelBMFont::create("Unable to Render\nObject Preview", "bigFont.fnt");
            label->setScale(.5f);
            bg->addChildAtPosition(label, Anchor::Center);
        }
        else {
            std::string saveStr;
            for (auto obj : CCArrayExt<GameObject*>(objs)) {
                saveStr += std::string(obj->getSaveString(popup->m_ui->m_editorLayer)) + ";";
            }
            auto spr = popup->m_ui->spriteFromObjectString(saveStr, false, false, 0, nullptr, nullptr, nullptr);
            limitNodeSize(spr, bg->getContentSize() - ccp(10, 10), 1.f, .1f);
            bg->addChildAtPosition(spr, Anchor::Center);
        }
        m_mainLayer->addChildAtPosition(bg, Anchor::Center);
        
        auto selectSpr = ButtonSprite::create("Select Objects", "goldFont.fnt", "GJ_button_01.png", .8f);
        selectSpr->setScale(.7f);
        auto selectBtn = CCMenuItemSpriteExtra::create(
            selectSpr, this, menu_selector(ObjectsListPopup::onSelect)
        );
        m_buttonMenu->addChildAtPosition(selectBtn, Anchor::Bottom, ccp(0, 20));
    }

    return true;
}

void ObjectsListPopup::onSelect(CCObject*) {
    // To avoid technical UB from `this->onClose(nullptr)` which frees the member
    auto popup = m_popup;

    // Select & focus the objects
    popup->m_ui->selectObjects(m_objs, true);
    be::focusEditorOnObjects(popup->m_ui, m_objs, true);
    popup->m_ui->updateButtons();

    // Close popups so they don't come in the way
    this->onClose(nullptr);
    popup->onClose(nullptr);
}

ObjectsListPopup* ObjectsListPopup::create(
    std::string const& title, GroupSummaryPopup* popup, CCArray* objs, bool render
) {
    auto ret = new ObjectsListPopup();
    if (ret && ret->init(title, popup, objs, render)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool GroupSummaryPopup::init(EditorUI* ui) {
    if (!Popup::init(380, 290))
        return false;

    m_noElasticity = true;
    m_ui = ui;

    // This should be remembered across launches because this is a preference
    m_view = static_cast<GroupSummaryView>(Mod::get()->template getSavedValue<int>(
        "group-summary-view-mode",
        static_cast<int>(GroupSummaryView::Grid)
    ));

    // These however would be silly to store between sessions or between editors
    std::string lastSearchQuery;
    std::tie(m_page, m_filter, lastSearchQuery) = LAST_PAGE;

    this->setTitle("View Groups");

    // Load up groups to memory (holy shit an use case for multimap?????)
    std::unordered_multimap<int, EffectGameObject*> triggersTargetingGroups {};
    for (auto trigger : CCArrayExt<EffectGameObject*>(m_ui->m_editorLayer->m_drawGridLayer->m_effectGameObjects)) {
        for (auto group : be::getTriggerTargetedGroups(trigger)) {
            triggersTargetingGroups.emplace(group, trigger);
        }
    }
    for (int group = 1; group < 10000; group += 1) {
        auto objs = ObjectsInGroupID {
            .groupID = group,
            .targets = be::getObjectsFromGroupDict(m_ui->m_editorLayer->m_groupDict, group),
            .triggers = CCArray::create(),
        };
        auto [first, last] = triggersTargetingGroups.equal_range(group);
        for (auto it = first; it != last; it++) {
            objs.triggers->addObject(it->second);
        }
        m_objectsInGroupsCache.emplace(group, objs);
    }

    m_groupsListContainer = CCNode::create();
    m_groupsListContainer->setContentSize(ccp(280, 220));
    m_groupsListContainer->setAnchorPoint(ccp(.5f, .5f));
    m_mainLayer->addChildAtPosition(m_groupsListContainer, Anchor::Center, ccp(0, 0));

    auto prevPageSpr = CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
    prevPageSpr->setScale(.7f);
    m_prevPageBtn = CCMenuItemSpriteExtra::create(
        prevPageSpr, this, menu_selector(GroupSummaryPopup::onPage)
    );
    m_prevPageBtn->setTag(-1);
    m_buttonMenu->addChildAtPosition(
        m_prevPageBtn, Anchor::Center,
        ccp(-m_groupsListContainer->getContentWidth() / 2 - 20, -5)
    );

    auto nextPageSpr = CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
    nextPageSpr->setFlipX(true);
    nextPageSpr->setScale(.7f);
    m_nextPageBtn = CCMenuItemSpriteExtra::create(
        nextPageSpr, this, menu_selector(GroupSummaryPopup::onPage)
    );
    m_nextPageBtn->setTag(1);
    m_buttonMenu->addChildAtPosition(
        m_nextPageBtn, Anchor::Center,
        ccp(m_groupsListContainer->getContentWidth() / 2 + 20, -5)
    );

    m_filtersMenu = CCMenu::create();
    m_filtersMenu->setContentHeight(180);
    m_filtersMenu->setScale(.65f);
    for (auto sprName : std::initializer_list<std::pair<GroupSummaryFilter, const char*>> {
        std::make_pair(GroupSummaryFilter::All, "group-summary-all.png"_spr),
        std::make_pair(GroupSummaryFilter::Unused, "group-summary-unused.png"_spr),
        std::make_pair(GroupSummaryFilter::Used, "group-summary-used.png"_spr),
        std::make_pair(GroupSummaryFilter::NoTriggers, "group-summary-no-triggers.png"_spr),
        std::make_pair(GroupSummaryFilter::NoTargets, "group-summary-no-targets.png"_spr),
    }) {
        auto sprOff = ButtonSprite::create(
            CCSprite::createWithSpriteFrameName(sprName.second),
            50, true, 50, "GJ_button_01.png", .9f
        );
        sprOff->setScale(.8f);
        auto sprOn = ButtonSprite::create(
            CCSprite::createWithSpriteFrameName(sprName.second),
            50, true, 50, "GJ_button_02.png", .9f
        );
        sprOn->setScale(.8f);
        auto toggle = CCMenuItemToggler::create(
            sprOff, sprOn, this, menu_selector(GroupSummaryPopup::onFilter)
        );
        toggle->setTag(static_cast<int>(sprName.first));
        toggle->m_notClickable = true;
        m_filtersMenu->addChild(toggle);
    }
    m_filtersMenu->setLayout(
        ColumnLayout::create()
            ->setAxisReverse(true)
            ->setAxisAlignment(AxisAlignment::End)
    );
    m_buttonMenu->addChildAtPosition(m_filtersMenu, Anchor::TopLeft, ccp(20, -15), ccp(0, 1));

    m_viewMenu = CCMenu::create();
    m_viewMenu->setContentWidth(70);
    m_viewMenu->setScale(.65f);
    for (auto sprName : std::initializer_list<std::pair<GroupSummaryView, const char*>> {
        std::make_pair(GroupSummaryView::List, "GJ_extendedIcon_001.png"),
        std::make_pair(GroupSummaryView::Grid, "geode.loader/grid-view.png"),
    }) {
        auto sprOff = ButtonSprite::create(
            CCSprite::createWithSpriteFrameName(sprName.second),
            50, true, 50, "GJ_button_01.png", .9f
        );
        sprOff->setScale(.8f);
        auto sprOn = ButtonSprite::create(
            CCSprite::createWithSpriteFrameName(sprName.second),
            50, true, 50, "GJ_button_02.png", .9f
        );
        sprOn->setScale(.8f);
        auto toggle = CCMenuItemToggler::create(
            sprOff, sprOn, this, menu_selector(GroupSummaryPopup::onView)
        );
        toggle->setTag(static_cast<int>(sprName.first));
        toggle->m_notClickable = true;
        m_viewMenu->addChild(toggle);
    }
    m_viewMenu->setLayout(RowLayout::create());
    m_buttonMenu->addChildAtPosition(m_viewMenu, Anchor::TopLeft, ccp(50, -10), ccp(0, 1));

    m_search = TextInput::create(m_groupsListContainer->getContentWidth() / .75f, "Search Groups");
    m_search->setCommonFilter(CommonFilter::Uint);
    m_search->setTextAlign(TextInputAlign::Left);
    m_search->setScale(.75f);
    m_search->setString(lastSearchQuery);
    m_search->setCallback([this](std::string const&) {
        this->updatePages();
    });
    m_mainLayer->addChildAtPosition(m_search, Anchor::Bottom, ccp(0, 20));

    m_pageLabel = CCLabelBMFont::create("", "goldFont.fnt");
    m_pageLabel->setScale(.5f);
    m_mainLayer->addChildAtPosition(m_pageLabel, Anchor::TopRight, ccp(-15, -7), ccp(1, 1));

    m_matchesLabel = CCLabelBMFont::create("", "bigFont.fnt");
    m_matchesLabel->setScale(.25f);
    m_mainLayer->addChildAtPosition(m_matchesLabel, Anchor::TopRight, ccp(-15, -22), ccp(1, 1));

    this->updatePages(m_page);

    return true;
}

void GroupSummaryPopup::loadPage(size_t page) {
    const size_t lastPossiblePage = m_pages.pages.empty() ? 0 : m_pages.pages.size() - 1;

    // Update page & clear existing page
    // The possible page number is clamped to number of pages
    m_page = std::min(page, lastPossiblePage);
    m_groupsListContainer->removeAllChildren();

    // Store this so if the popup is opened again we get to the same pagez
    LAST_PAGE = std::make_tuple(m_page, m_filter, m_search->getString());

    // Update toggles
    for (auto toggle : CCArrayExt<CCMenuItemToggler*>(m_filtersMenu->getChildren())) {
        toggle->toggle(m_filter == static_cast<GroupSummaryFilter>(toggle->getTag()));
    }
    for (auto toggle : CCArrayExt<CCMenuItemToggler*>(m_viewMenu->getChildren())) {
        toggle->toggle(m_view == static_cast<GroupSummaryView>(toggle->getTag()));
    }

    CCSize itemSize;
    switch (m_view) {
        case GroupSummaryView::Grid: {
            m_groupsListContainer->setLayout(
                RowLayout::create()
                    ->setCrossAxisAlignment(AxisAlignment::End)
                    ->setCrossAxisLineAlignment(AxisAlignment::Start)
                    ->setGrowCrossAxis(true)
                    ->setCrossAxisOverflow(false)
                    ->setGap(2.5f)
            );
            itemSize = CCSize(
                (m_groupsListContainer->getContentWidth() - 2.5f * (ITEMS_PER_ROW - 1)) / ITEMS_PER_ROW,
                (m_groupsListContainer->getContentHeight() - 2.5f * (ITEMS_PER_COLUMN - 1)) / ITEMS_PER_COLUMN
            );
        } break;

        case GroupSummaryView::List: {
            m_groupsListContainer->setLayout(
                ColumnLayout::create()
                    ->setCrossAxisOverflow(false)
                    ->setAxisReverse(true)
                    ->setAxisAlignment(AxisAlignment::End)
                    ->setGap(2.5f)
            );
            itemSize = CCSize(
                m_groupsListContainer->getContentWidth(),
                (m_groupsListContainer->getContentHeight() - 2.5f * (ITEMS_PER_LIST - 1)) / ITEMS_PER_LIST
            );
        } break;
    }

    if (m_pages.pages.empty()) {
        auto emptyLabel = CCLabelBMFont::create("No Groups Found :(", "bigFont.fnt");
        emptyLabel->setOpacity(155);
        emptyLabel->setColor(ccc3(155, 155, 155));
        emptyLabel->setLayoutOptions(AxisLayoutOptions::create()->setScaleLimits(.1f, .5f));
        m_groupsListContainer->addChild(emptyLabel);
    }
    else for (auto group : m_pages.pages.at(m_page)) {
        constexpr float LIST_VIEW_NUM_SPACE  = 25;
        constexpr float LIST_VIEW_OBJS_SPACE = 55;

        auto itemContainer = CCMenu::create();
        itemContainer->ignoreAnchorPointForPosition(false);
        itemContainer->setContentSize(itemSize);

        auto itemBG = NineSlice::create("square02b_001.png");
        itemBG->setScale(.5f);
        itemBG->setContentSize(itemSize / itemBG->getScale());
        itemBG->setColor(ccc3(0, 0, 0));
        itemBG->setOpacity(90);
        itemContainer->addChildAtPosition(itemBG, Anchor::Center);

        auto triggers = CCNode::create();
        triggers->setScale(.75f);
        triggers->setAnchorPoint(ccp(.5f, .5f));

        const auto addTriggerSprite = [this, triggers](CCSprite* spr) {
            auto container = CCNode::create();
            spr->setScale(.65f);
            container->setContentSize(
                m_view == GroupSummaryView::Grid ? 
                    CCSize(5, spr->getScaledContentHeight()) : 
                    spr->getScaledContentSize()
            );
            container->addChildAtPosition(spr, Anchor::Center);
            triggers->addChild(container);
        };

        if (group.triggers->count() == 0) {
            addTriggerSprite(CCSprite::createWithSpriteFrameName("no-trigger.png"_spr));
        }
        for (auto trigger : CCArrayExt<EffectGameObject*>(group.triggers)) {
            addTriggerSprite(createSpriteForTrigger(trigger, group.groupID));
        }
        if (m_view == GroupSummaryView::Grid) {
            triggers->setContentWidth((itemSize.width - 5) / triggers->getScale());
            triggers->setLayout(
                RowLayout::create()
                    ->setGap(0)
            );
        }
        else {
            triggers->setContentWidth(
                (itemSize.width - LIST_VIEW_OBJS_SPACE - LIST_VIEW_NUM_SPACE) /
                    triggers->getScale()
            );
            triggers->setLayout(
                SimpleRowLayout::create()
                    ->setGap(2.5f)
                    ->setMainAxisAlignment(MainAxisAlignment::Start)
                    ->setMainAxisScaling(AxisScaling::Fit)
                    ->setCrossAxisScaling(AxisScaling::Grow)
            );
        }

        auto triggersBtn = CCMenuItemSpriteExtra::create(
            triggers, this, menu_selector(GroupSummaryPopup::onGroupTriggers)
        );
        triggersBtn->setEnabled(group.triggers->count() > 0);
        triggersBtn->setUserObject(group.triggers);
        triggersBtn->setTag(group.groupID);
        if (m_view == GroupSummaryView::Grid) {
            itemContainer->addChildAtPosition(triggersBtn, Anchor::Center, ccp(0, 0));
        }
        else {
            itemContainer->addChildAtPosition(
                triggersBtn, Anchor::Left,
                ccp(LIST_VIEW_NUM_SPACE + triggersBtn->getScaledContentWidth() / 2, 0)
            );
        }

        auto groupNum = CCLabelBMFont::create(fmt::format("{}", group.groupID).c_str(), "goldFont.fnt");
        if (m_view == GroupSummaryView::Grid) {
            groupNum->limitLabelWidth(itemSize.width - 5, .5f, .1f);
            itemContainer->addChildAtPosition(groupNum, Anchor::Top, ccp(0, -10));
        }
        else {
            groupNum->limitLabelWidth(LIST_VIEW_NUM_SPACE - 5, .5f, .1f);
            itemContainer->addChildAtPosition(groupNum, Anchor::Left, ccp(LIST_VIEW_NUM_SPACE / 2, 0));
        }

        auto objCount = CCLabelBMFont::create(
            fmt::format("{} objs", group.targets ? group.targets->count() : 0).c_str(),
            "bigFont.fnt"
        );
        if (m_view == GroupSummaryView::Grid) {
            objCount->limitLabelWidth(itemSize.width - 5, .35f, .1f);
        }
        else {
            objCount->limitLabelWidth(LIST_VIEW_OBJS_SPACE, .35f, .1f);
        }

        auto objCountBtn = CCMenuItemSpriteExtra::create(
            objCount, this, menu_selector(GroupSummaryPopup::onGroupTargets)
        );
        objCountBtn->setEnabled(group.targets && group.targets->count() > 0);
        objCountBtn->setUserObject(group.targets);
        objCountBtn->setTag(group.groupID);
        if (m_view == GroupSummaryView::Grid) {
            itemContainer->addChildAtPosition(objCountBtn, Anchor::Bottom, ccp(0, 10));
        }
        else {
            itemContainer->addChildAtPosition(
                objCountBtn, Anchor::Right,
                ccp(-objCountBtn->getContentWidth() / 2 - 5, 0)
            );
        }

        m_groupsListContainer->addChild(itemContainer);
    }
    m_groupsListContainer->updateLayout();

    m_pageLabel->setString(fmt::format("Page {}/{}", m_page + 1, m_pages.pages.size() + 1).c_str());
    m_matchesLabel->setString(fmt::format("(showing {} groups)", m_pages.totalMatches).c_str());

    be::enableButton(m_prevPageBtn, m_page > 0);
    be::enableButton(m_nextPageBtn, m_page < lastPossiblePage);
}
void GroupSummaryPopup::onPage(CCObject* sender) {
    // x + y >= 0 <=> x >= -y
    if (static_cast<int>(m_page) >= -sender->getTag()) {
        this->loadPage(m_page + sender->getTag());
    }
}
void GroupSummaryPopup::updatePages(size_t page) {
    Mod::get()->setSavedValue("group-summary-view-mode", static_cast<int>(m_view));

    // Clear existing pages
    m_pages.reset();

    // Load up pages
    const size_t groupsPerPage = m_view == GroupSummaryView::Grid ? 
        ITEMS_PER_ROW * ITEMS_PER_COLUMN : 
        ITEMS_PER_LIST;
    for (auto& [group, objs] : m_objectsInGroupsCache) {
        bool matches = false;
        bool hasTargets = objs.targets && objs.targets->count() > 0;
        bool hasTriggers = objs.triggers->count() > 0;
        switch (m_filter) {
            case GroupSummaryFilter::All:        matches = true; break;
            case GroupSummaryFilter::Used:       matches = hasTargets && hasTriggers; break;
            case GroupSummaryFilter::Unused:     matches = !hasTargets && !hasTriggers; break;
            case GroupSummaryFilter::NoTargets:  matches = !hasTargets && hasTriggers; break;
            case GroupSummaryFilter::NoTriggers: matches = hasTargets && !hasTriggers; break;
        }
        if (m_search->getString().size() && !numToString(group).starts_with(m_search->getString())) {
            matches = false;
        }
        if (matches) {
            if (m_pages.pages.empty() || m_pages.pages.back().size() >= groupsPerPage) {
                m_pages.pages.emplace_back();
            }
            m_pages.pages.back().push_back(objs);
            m_pages.totalMatches += 1;
        }
    }

    this->loadPage(page);
}
void GroupSummaryPopup::onFilter(CCObject* sender) {
    m_filter = static_cast<GroupSummaryFilter>(sender->getTag());
    this->updatePages();
}
void GroupSummaryPopup::onView(CCObject* sender) {
    m_view = static_cast<GroupSummaryView>(sender->getTag());
    this->updatePages();
}

void GroupSummaryPopup::onGroupTriggers(CCObject* sender) {
    auto triggers = static_cast<CCArray*>(static_cast<CCNode*>(sender)->getUserObject());
    ObjectsListPopup::create(fmt::format("Triggers for Group {}", sender->getTag()), this, triggers, false)->show();
}
void GroupSummaryPopup::onGroupTargets(CCObject* sender) {
    auto targets = static_cast<CCArray*>(static_cast<CCNode*>(sender)->getUserObject());
    ObjectsListPopup::create(fmt::format("Targets for Group {}", sender->getTag()), this, targets, true)->show();
}

GroupSummaryPopup* GroupSummaryPopup::create(EditorUI* ui) {
    auto ret = new GroupSummaryPopup();
    if (ret && ret->init(ui)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

class $modify(GroupSummaryEditorUI, EditorUI) {
    struct Fields final {
        ListenerHandle onUIHide;
    };

    $override
    bool init(LevelEditorLayer* lel) {
        if (!EditorUI::init(lel))
            return false;

        if (!Mod::get()->getSettingValue<bool>("group-summary")) {
            return true;
        }

        // Reset as it would be silly to remember this between levels
        LAST_PAGE = std::make_tuple(0, GroupSummaryFilter::All, "");

        if (auto menu = this->querySelector("editor-buttons-menu")) {
            auto spr = EditorButtonSprite::createWithSpriteFrameName("group-summary.png"_spr, 1.f, EditorBaseColor::DimGreen);
            auto btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(GroupSummaryEditorUI::onSummary));
            btn->setContentSize(ccp(40, 40));
            btn->setID("group-summary"_spr);
            menu->addChild(btn);
            menu->updateLayout();

            m_fields->onUIHide = UIShowEvent(this).listen([btn](bool show) {
                btn->setVisible(show);
            });
        }
        
        return true;
    }

    void onSummary(CCObject*) {
        GroupSummaryPopup::create(this)->show();
    }
};
