#include "UndoHistory.hpp"
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/binding/GJGameLevel.hpp>
#include <Geode/binding/GJRotationControl.hpp>
#include <Geode/binding/GJScaleControl.hpp>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/binding/ButtonSprite.hpp>
#include <Geode/loader/SettingEvent.hpp>
#include <Geode/ui/ScrollLayer.hpp>
#include <Geode/ui/General.hpp>
#include <other/AutoGrowingLayout.hpp>
#include <other/ValueTo.hpp>
#include <ui/Tag.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>

struct $modify(HistoryUI, EditorUI) {
    CCMenuItemSpriteExtra* historyBtn = nullptr;

    bool init(LevelEditorLayer* lel) {
        if (!EditorUI::init(lel))
            return false;
        
        if (auto menu = this->getChildByID("undo-menu")) {
            menu->addChild(m_fields->historyBtn = CCMenuItemSpriteExtra::create(
                CircleButtonSprite::createWithSpriteFrameName(
                    "GJ_timeIcon_001.png", 1.f, CircleBaseColor::Green,
                    CircleBaseSize::Small
                ),
                this, menu_selector(HistoryUI::onHistory)
            ));
            menu->updateLayout();
        }

        return true;
    }

    void onHistory(CCObject*) {
        if (auto history = History::get(m_editorLayer)) {
            HistoryPopup::create(history)->show();
        }
    }

    void showUI(bool show) {
        EditorUI::showUI(show);
        if (m_fields->historyBtn) {
            m_fields->historyBtn->setVisible(show);
        }
    }

    void updateUIAfterAction() {
        this->updateButtons();
        if (m_rotationControl->isVisible()) {
            this->activateRotationControl(nullptr);
        }
        this->tryUpdateTimeMarkers();
        this->updateSpecialUIElements();
    }

    void updateButtons() {
        EditorUI::updateButtons();
        if (Mod::get()->getSettingValue<bool>("enable-new-undo")) {
            if (auto history = History::get(m_editorLayer)) {
                if (history->canUndo()) {
                    m_undoBtn->setColor({ 255, 255, 255 });
                    m_undoBtn->setOpacity(255);
                    m_undoBtn->setEnabled(true);
                }
                else {
                    m_undoBtn->setColor({ 166, 166, 166 });
                    m_undoBtn->setOpacity(175);
                    m_undoBtn->setEnabled(false);
                }

                if (history->canRedo()) {
                    m_redoBtn->setColor({ 255, 255, 255 });
                    m_redoBtn->setOpacity(255);
                    m_redoBtn->setEnabled(true);
                }
                else {
                    m_redoBtn->setColor({ 166, 166, 166 });
                    m_redoBtn->setOpacity(175);
                    m_redoBtn->setEnabled(false);
                }
            }
        }
    }

    void undoLastAction(CCObject* sender) {
        if (!Mod::get()->getSettingValue<bool>("enable-new-undo")) {
            return EditorUI::undoLastAction(sender);
        }
        if (auto history = History::get(m_editorLayer)) {
            history->undo();
        }
        this->updateUIAfterAction();
    }

    void redoLastAction(CCObject* sender) {
        if (!Mod::get()->getSettingValue<bool>("enable-new-undo")) {
            return EditorUI::redoLastAction(sender);
        }
        if (auto history = History::get(m_editorLayer)) {
            history->redo();
        }
        this->updateUIAfterAction();
    }
};

bool HistoryNode::init(HistoryPopup* popup, EditorEvent* event, float width) {
    if (!CCNode::init())
        return false;
    
    m_popup = popup;
    m_event = event;
    this->ignoreAnchorPointForPosition(false);
    this->setContentSize({ width, 30.f });

    m_bottomClip = Clip::create();
    m_bottomClip->setContentSize({ width, 300.f });
    m_bottomClip->setInverted(true);

    m_bottomMenu = CCMenu::create();
    m_bottomMenu->setContentSize({ width - 16.f, 30.f });
    m_bottomMenu->ignoreAnchorPointForPosition(false);
    m_bottomMenu->setAnchorPoint({ .0f, .0f });
    m_bottomMenu->setPosition(8.f, 5.f);

    for (auto& detail : event->details()) {
        auto tag = Tag::create(detail.info, detail.icon);
        m_bottomMenu->addChild(tag);
    }

    m_bottomMenu->setLayout(
        RowLayout::create()
            ->setCrossAxisOverflow(true)
            ->setGrowCrossAxis(true)
            ->setAxisAlignment(AxisAlignment::Start)
    );
    m_bottomClip->addChild(m_bottomMenu);

    this->addChild(m_bottomClip);

    m_topMenu = CCMenu::create();
    m_topMenu->setPosition(0.f, 0.f);
    m_topMenu->setContentSize({ width, 30.f });

    auto icon = event->icon();
    icon->setPosition({ 20.f, 15.f });
    m_topMenu->addChild(icon);

    m_name = CCLabelBMFont::create(event->desc().c_str(), "bigFont.fnt");
    m_name->limitLabelWidth(width - 120.f, .5f, .1f);
    m_name->setAnchorPoint({ .0f, .5f });
    m_name->setPosition(40.f, 15.f);
    m_name->setID("name");
    m_topMenu->addChild(m_name);

    auto undoBtnSpr = ButtonSprite::create(
        CCSprite::createWithSpriteFrameName("undo.png"_spr),
        0, false, 32.f, "GJ_button_01.png", .9f
    );
    undoBtnSpr->setScale(.525f);
    m_undoBtn = CCMenuItemSpriteExtra::create(
        undoBtnSpr, this, menu_selector(HistoryNode::onUndoTill)
    );
    m_undoBtn->setPosition(width - 20.f, 15.f);
    m_undoBtn->setID("undo-button");
    m_topMenu->addChild(m_undoBtn);

    if (event->details().size()) {
        auto detailsBtnSpr = CCSprite::createWithSpriteFrameName("edit_downBtn_001.png");
        detailsBtnSpr->setScale(.725f);
        m_detailsBtn = CCMenuItemSpriteExtra::create(
            detailsBtnSpr, this, menu_selector(HistoryNode::onDetails)
        );
        m_detailsBtn->setPosition(width - 45.f, 15.f);
        m_detailsBtn->setID("details-button");
        m_topMenu->addChild(m_detailsBtn);
    }

    this->addChild(m_topMenu);
    
    auto border = CCLayerColor::create({ 0, 0, 0, 100 }, width, 1);
    border->setZOrder(11);
    this->addChild(border);

    return true;
}

void HistoryNode::onRedoTill(CCObject*) {
    m_popup->getHistory()->redoTo(m_event);
    m_popup->updateState();
}

void HistoryNode::onUndoTill(CCObject*) {
    m_popup->getHistory()->undoTo(m_event);
    m_popup->updateState();
}

void HistoryNode::onDetails(CCObject*) {
    this->stopAllActions();
    this->runAction(CCEaseInOut::create(
        ValueTo<float>::create(
            .15f,
            m_detailsOpen, (m_detailsOpen > 0.f ? 0.f : 1.f), 
            [this](auto&) {
                m_popup->updateState();
            }
        ), 2.f
    ));
    m_popup->updateState();
}

void HistoryNode::updateState(bool redoable) {
    m_name->setColor(
        redoable ? ccColor3B { 155, 155, 155 } : ccColor3B { 255, 255, 255 }
    );
    auto bspr = static_cast<ButtonSprite*>(m_undoBtn->getNormalImage());
    bspr->m_subSprite->setFlipX(redoable);
    bspr->updateBGImage(redoable ? "GJ_button_02.png" : "GJ_button_01.png");
    m_undoBtn->setTarget(this, redoable ?
        menu_selector(HistoryNode::onRedoTill) :
        menu_selector(HistoryNode::onUndoTill)
    );

    auto extension = m_detailsOpen * (m_bottomMenu->getScaledContentSize().height + 5.f);
    this->setContentSize({ m_obContentSize.width, extension + 30.f });

    if (m_detailsBtn) {
        static_cast<CCSprite*>(m_detailsBtn->getNormalImage())->setFlipY(m_detailsOpen > 0.f);
    }
    m_topMenu->setPosition(0.f, extension);
    m_bottomClip->setPositionY(extension);
    m_bottomMenu->setPositionY(-extension + 5.f);
    m_bottomMenu->setEnabled(m_detailsOpen >= 1.f);
}

HistoryNode* HistoryNode::create(HistoryPopup* popup, EditorEvent* event, float width) {
    auto ret = new HistoryNode;
    if (ret && ret->init(popup, event, width)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool HistoryPopup::setup(History* history) {
    m_noElasticity = true;
    m_history = history;

    this->setTitle("History");

    auto winSize = CCDirector::get()->getWinSize();

    auto scrollPos = CCPoint(winSize / 2) - CCPoint { 0.f, 10.f };
    auto scrollSize = CCSize { 346.f, 220.f };
    m_scroll = ScrollLayer::create(scrollSize);
    m_scroll->setPosition(scrollPos - scrollSize / 2);

    for (auto& ev : history->getEvents()) {
        auto menu = HistoryNode::create(this, ev, scrollSize.width);
        m_scroll->m_contentLayer->addChild(menu);
        m_items.push_back(menu);
    }

    this->updateState();

    m_scroll->m_contentLayer->setLayout(
        AutoGrowingLayout::create(scrollSize.height)
            ->setAxisAlignment(AxisAlignment::End)
            ->setAxisReverse(true)
            ->setGap(0)
    );
    m_mainLayer->addChild(m_scroll);

    addListBorders(m_mainLayer, scrollPos, scrollSize);

    auto spr = CCSprite::createWithSpriteFrameName("GJ_trashBtn_001.png");
    spr->setScale(.5f);
    auto btn = CCMenuItemSpriteExtra::create(
        spr, this, menu_selector(HistoryPopup::onClear)
    );
    btn->setPosition(m_size.width / 2 - 20.f, m_size.height / 2 - 20.f);
    m_buttonMenu->addChild(btn);

    return true;
}

void HistoryPopup::onClear(CCObject*) {
    createQuickPopup(
        "Clear History",
        "Are you sure you want to <cr>clear history</c>? This will make it "
        "impossible to undo current changes!",
        "Cancel", "Clear",
        [this](auto, bool btn2) {
            if (btn2) {
                m_history->clear();
                static_cast<HistoryUI*>(EditorUI::get())->updateUIAfterAction();
                this->onClose(nullptr);
            }
        }
    );
}

void HistoryPopup::updateState() {
    size_t i = 0;
    for (auto& item : m_items) {
        item->updateState(i++ >= m_history->getRedoneCount());
    }
    m_scroll->m_contentLayer->updateLayout();
    static_cast<HistoryUI*>(EditorUI::get())->updateUIAfterAction();
}

History* HistoryPopup::getHistory() const {
    return m_history;
}

HistoryPopup* HistoryPopup::create(History* history) {
    auto ret = new HistoryPopup;
    if (ret && ret->init(360.f, 280.f, history)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

void History::onEvent(EditorEvent* ev) {
    log::info("{}", ev->toDiffString());
    // pop undone history
    if (m_undone) {
        m_events.erase(m_events.end() - m_undone, m_events.end());
        m_undone = 0;
    }
    m_events.emplace_back(ev->unique());
    EditorUI::get()->updateButtons();
}

std::vector<EditorEvent*> History::getEvents() const {
    std::vector<EditorEvent*> res;
    res.reserve(m_events.size());
    for (auto& ev : m_events) {
        res.push_back(ev.get());
    }
    return res;
}

size_t History::getRedoneCount() const {
    return m_events.size() - m_undone;
}

size_t History::getUndoneCount() const {
    return m_undone;
}

EditorEvent* History::current() {
    if (m_undone < m_events.size()) {
        return m_events.at(m_events.size() - m_undone - 1).get();
    }
    return nullptr;
}

void History::undo(size_t count) {
    while (count--) {
        if (m_undone < m_events.size()) {
            this->current()->undo();
            m_undone += 1;
        } else break;
    }
}

void History::undoTo(EditorEvent* event) {
    while (m_undone < m_events.size()) {
        auto c = this->current();
        c->undo();
        m_undone += 1;
        if (c == event) {
            break;
        }
    }
}

void History::redo(size_t count) {
    while (count-- && m_undone > 0) {
        m_undone -= 1;
        this->current()->redo();
    }
}

void History::redoTo(EditorEvent* event) {
    while (m_undone > 0 && this->current() != event) {
        m_undone -= 1;
        this->current()->redo();
    }
}

bool History::canUndo() const {
    return m_undone < m_events.size();
}

bool History::canRedo() const {
    return m_undone > 0;
}

void History::clear() {
    m_events.clear();
    m_undone = 0;
}

History* History::create(GJGameLevel* level) {
    auto ret = new History();
    ret->m_initialString = level->m_levelString;
    ret->autorelease();
    return ret;
}

struct $modify(HistoryLayer, LevelEditorLayer) {
    Ref<History> history;

    bool init(GJGameLevel* level) {
        if (!LevelEditorLayer::init(level))
            return false;
        
        m_fields->history = History::create(level);

        return true;
    }
};

History* History::get(LevelEditorLayer* lel) {
    if (!lel) return nullptr;
    return static_cast<HistoryLayer*>(lel)->m_fields->history;
}
