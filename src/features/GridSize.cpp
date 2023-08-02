#include <Geode/modify/EditorUI.hpp>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/binding/CCMenuItemToggler.hpp>
#include <Geode/ui/InputNode.hpp>
#include <hjfod.custom-keybinds/include/Keybinds.hpp>
#include <MoreTabs.hpp>
#include <ModifyEditor.hpp>

using namespace geode::prelude;
using namespace better_edit;
using namespace keybinds;

// todo: make the input work and post an event when the grid size is changed

class GridSizeButtonBar : public CCMenu {
protected:
    InputNode* m_input;
    CCMenuItemToggler* m_lockBtn;
    EventListener<EventFilter<GridChangeEvent>> m_listener = { this, &GridSizeButtonBar::onEvent };

    bool init() {
        if (!CCMenu::init())
            return false;
        
        auto winSize = CCDirector::get()->getWinSize();

        this->ignoreAnchorPointForPosition(false);
        this->setAnchorPoint({ .5f, .5f });
        this->setContentSize({ 200.f, 60.f });
        this->setID("grid-size-tab-menu"_spr);

        auto zoomOutSpr = CCSprite::createWithSpriteFrameName("GJ_zoomOutBtn_001.png");
        zoomOutSpr->setScale(.5f);
        auto zoomOutBtn = CCMenuItemSpriteExtra::create(
            zoomOutSpr, this, menu_selector(GridSizeButtonBar::onZoom)
        );
        zoomOutBtn->setTag(false);
        zoomOutBtn->setPosition(m_obContentSize / 2 - ccp(45.f, 0.f));
        this->addChild(zoomOutBtn);

        auto zoomInSpr = CCSprite::createWithSpriteFrameName("GJ_zoomInBtn_001.png");
        zoomInSpr->setScale(.5f);
        auto zoomInBtn = CCMenuItemSpriteExtra::create(
            zoomInSpr, this, menu_selector(GridSizeButtonBar::onZoom)
        );
        zoomInBtn->setTag(true);
        zoomInBtn->setPosition(m_obContentSize / 2 + ccp(45.f, 0.f));
        this->addChild(zoomInBtn);

        auto lockOffSpr = CCSprite::createWithSpriteFrameName("GJ_lock_open_001.png");
        lockOffSpr->setScale(.65f);
        auto lockOnSpr = CCSprite::createWithSpriteFrameName("GJ_lock_001.png");
        lockOnSpr->setScale(.65f);
        m_lockBtn = CCMenuItemToggler::create(
            lockOffSpr, lockOnSpr,
            this, menu_selector(GridSizeButtonBar::onLock)
        );
        m_lockBtn->setPosition(m_obContentSize / 2 + ccp(65.f, 0.f));
        m_lockBtn->setClickable(false);
        this->addChild(m_lockBtn);

        m_input = InputNode::create(60.f, "Size");
        m_input->setPosition(m_obContentSize / 2);
        m_input->setScale(.85f);
        this->addChild(m_input);
        
        return true;
    }

    void onZoom(CCObject* sender) {
        EditorGrid::get()->zoom(static_cast<CCMenuItemSpriteExtra*>(sender)->getTag());
    }

    void onLock(CCObject* sender) {
        EditorGrid::get()->setLocked(!EditorGrid::get()->isLocked());
        static_cast<CCMenuItemToggler*>(sender)->toggle(EditorGrid::get()->isLocked());
    }

    ListenerResult onEvent(GridChangeEvent* ev) {
        m_input->setString(numToString(ev->newSize));
        m_lockBtn->toggle(ev->locked);
        return ListenerResult::Propagate;
    }

public:
    static GridSizeButtonBar* create() {
        auto ret = new GridSizeButtonBar();
        if (ret && ret->init()) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};

struct $modify(EditorUI) {
    bool init(LevelEditorLayer* lel) {
        if (!EditorUI::init(lel))
            return false;
        
        MoreTabs::get(this)->addEditTab(
            "grid.png"_spr,
            GridSizeButtonBar::create()
        );

        this->template addEventListener<InvokeBindFilter>([=](InvokeBindEvent* event) {
            if (event->isDown()) {
                EditorGrid::get()->zoom(true);
            }
            return ListenerResult::Propagate;
        }, "zoom-grid-in"_spr);

        this->template addEventListener<InvokeBindFilter>([=](InvokeBindEvent* event) {
            if (event->isDown()) {
                EditorGrid::get()->zoom(false);
            }
            return ListenerResult::Propagate;
        }, "zoom-grid-out"_spr);

        return true;
    }
};

$execute {
    BindManager::get()->registerBindable(BindableAction(
        "zoom-grid-in"_spr,
        "Decrease Grid Size",
        "Decreases the size of the editor grid and keeps it at that size. "
        "If the size hits 30 and the grid size isn't locked, it will return to "
        "the ordinary dynamic behaviour",
        {},
        Category::EDITOR_UI
    ));

    BindManager::get()->registerBindable(BindableAction(
        "zoom-grid-out"_spr,
        "Increase Grid Size",
        "Increases the size of the editor grid and keeps it at that size. "
        "If the size hits 30 and the grid size isn't locked, it will return to "
        "the ordinary dynamic behaviour",
        {},
        Category::EDITOR_UI
    ), "zoom-grid-in"_spr);
}
