#include <MoreTabs.hpp>
#include <Geode/binding/CCMenuItemToggler.hpp>
#include <Geode/binding/EditButtonBar.hpp>
#include <Geode/binding/GameManager.hpp>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <hjfod.custom-keybinds/include/Keybinds.hpp>

using namespace geode::prelude;
using namespace keybinds;
using namespace better_edit;

bool MoreTabs::init(EditorUI* ui) {
    if (!CCNode::init())
        return false;
    
    m_ui = ui;

    m_editTabMenu = CCMenu::create();
    m_editTabMenu->setLayout(ui->m_tabsMenu->getLayout());
    m_editTabMenu->setContentSize(ui->m_tabsMenu->getContentSize());
    m_editTabMenu->setPosition(ui->m_tabsMenu->getPosition());
    this->addChild(m_editTabMenu);

    this->addEditTab("edit_rightBtn2_001.png", ui->m_editButtonBar);

    this->updateMode();

    return true;
}

void MoreTabs::updateMode(bool show) {
    m_editTabMenu->setVisible(show && m_ui->m_selectedMode == 3);
    int i = 0;
    for (auto& tab : m_editTabs) {
        tab->setVisible(show && m_ui->m_selectedMode == 3 && m_selectedEditTab == i++);
    }
    for (auto& child : CCArrayExt<CCMenuItemToggler>(m_editTabMenu->getChildren())) {
        child->toggle(m_selectedEditTab == child->getTag());
    }
}

void MoreTabs::onEditTab(CCObject* sender) {
    m_selectedEditTab = sender->getTag();
    int i = 0;
    for (auto& tab : m_editTabs) {
        tab->setVisible(m_selectedEditTab == i++);
    }
    for (auto& child : CCArrayExt<CCMenuItemToggler>(m_editTabMenu->getChildren())) {
        child->toggle(false);
    }
}

CCMenuItemToggler* MoreTabs::createTab(const char* icon, CCObject* target, SEL_MenuHandler selector) {
    auto tabOff = CCSprite::createWithSpriteFrameName("GJ_tabOff_001.png");
    tabOff->setOpacity(150);
    
    auto tabOffTop = CCSprite::createWithSpriteFrameName(icon);
    tabOffTop->setPosition(tabOff->getContentSize() / 2 - ccp(0.f, 1.f));
    tabOff->addChild(tabOffTop);
    limitNodeSize(tabOffTop, tabOff->getContentSize() / 1.5f, 1.f, .1f);
    
    auto tabOn = CCSprite::createWithSpriteFrameName("GJ_tabOn_001.png");

    auto tabOnTop = CCSprite::createWithSpriteFrameName(icon);
    tabOnTop->setPosition(tabOn->getContentSize() / 2 - ccp(0.f, 1.f));
    tabOn->addChild(tabOnTop);
    limitNodeSize(tabOnTop, tabOff->getContentSize() / 1.5f, 1.f, .1f);

    auto tab = CCMenuItemToggler::create(tabOff, tabOn, target, selector);
    tab->setSizeMult(1.2f);
    return tab;
}

int MoreTabs::addCreateTab(const char* icon, EditButtonBar* content) {
    auto tab = this->createTab(icon, m_ui, menu_selector(EditorUI::onSelectBuildTab));
    tab->setTag(m_ui->m_tabsArray->count());
    tab->setClickable(false);

    m_ui->m_tabsArray->addObject(tab);
    m_ui->m_tabsMenu->addChild(tab);
    m_ui->m_tabsMenu->updateLayout();

    content->setVisible(false);
    m_ui->m_createButtonBars->addObject(content);
    if (!content->getParent()) {
        m_ui->addChild(content, 10);
    }

    return tab->getTag();
}

int MoreTabs::addCreateTab(const char* icon, CCArray* buttons) {
    auto winSize = CCDirector::get()->getWinSize();
    return this->addCreateTab(icon, EditButtonBar::create(
        buttons, { winSize.width / 2, 86.f },
        m_ui->m_tabsArray->count(), true,
        GameManager::get()->getIntGameVariable("0049"),
        GameManager::get()->getIntGameVariable("0050")
    ));
}

int MoreTabs::addCreateTab(const char* icon, std::vector<int> const& objIDs) {
    auto buttons = CCArray::create();
    for (auto& id : objIDs) {
        buttons->addObject(m_ui->getCreateBtn(id, 4));
    }
    return this->addCreateTab(icon, buttons);
}

int MoreTabs::addEditTab(const char* icon, CCNode* content) {
    auto tab = this->createTab(icon, this, menu_selector(MoreTabs::onEditTab));
    tab->setTag(m_editTabs.size());
    m_editTabMenu->addChild(tab);
    m_editTabMenu->updateLayout();
    
    m_editTabs.push_back(content);
    if (!content->getParent()) {
        content->setZOrder(10);
        m_ui->addChild(content);
    }
    content->setVisible(false);

    if (m_editTabs.size() == 1) {
        this->onEditTab(tab);
        tab->toggle(true);
    }

    return tab->getTag();
}

EditButtonBar* MoreTabs::getCreateTab(int tag) const {
    return static_cast<EditButtonBar*>(m_ui->m_createButtonBars->objectAtIndex(tag));
}

void MoreTabs::nextTab() {
    if (m_ui->m_selectedMode == 3) {
        m_selectedEditTab += 1;
        if (m_selectedEditTab >= static_cast<int>(m_editTabs.size())) {
            m_selectedEditTab = 0;
        }
        this->updateMode();
    }
}

void MoreTabs::prevTab() {
    if (m_ui->m_selectedMode == 3) {
        m_selectedEditTab -= 1;
        if (m_selectedEditTab < 0) {
            m_selectedEditTab = m_editTabs.size() - 1;
        }
        this->updateMode();
    }
}

MoreTabs* MoreTabs::create(EditorUI* ui) {
    auto ret = new MoreTabs;
    if (ret && ret->init(ui)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

MoreTabs* MoreTabs::get(EditorUI* ui, bool create) {
    if (!ui) return nullptr;
    if (auto tabs = ui->getChildByID("more-tabs"_spr)) {
        return static_cast<MoreTabs*>(tabs);
    }
    if (!create) return nullptr;
    auto tabs = MoreTabs::create(ui);
    tabs->setID("more-tabs"_spr);
    ui->addChild(tabs);
    return tabs;
}

struct $modify(MoreTabsUI, EditorUI) {
    bool init(LevelEditorLayer* lel) {
        if (!EditorUI::init(lel))
            return false;
        
        // hook into F1 and F2 keybinds to have them work for additional tabs aswell

        this->defineKeybind("robtop.geometry-dash/prev-build-tab", [=] {
            if (auto tabs = MoreTabs::get(this, false)) {
                tabs->prevTab();
            }
        });
        this->defineKeybind("robtop.geometry-dash/next-build-tab", [=] {
            if (auto tabs = MoreTabs::get(this, false)) {
                tabs->nextTab();
            }
        });
        
        return true;
    }

    void defineKeybind(const char* id, std::function<void()> callback) {
        this->template addEventListener<InvokeBindFilter>([=](InvokeBindEvent* event) {
            if (event->isDown()) {
                callback();
            }
            return ListenerResult::Propagate;
        }, id);
    }

    void toggleMode(CCObject* sender) {
        EditorUI::toggleMode(sender);
        if (auto tabs = MoreTabs::get(this, false)) {
            tabs->updateMode();
        }
    }

    void showUI(bool show) {
        EditorUI::showUI(show);
        if (auto tabs = MoreTabs::get(this, false)) {
            tabs->updateMode(show);
        }
    }
};
