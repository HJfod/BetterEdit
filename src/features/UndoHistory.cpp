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

bool HistoryPopup::setup(History* history) {
    m_noElasticity = true;

    this->setTitle("History");

    auto winSize = CCDirector::get()->getWinSize();
    auto scrollSize = CCSize { 320.f, 220.f };
    auto scroll = ScrollLayer::create(scrollSize);
    scroll->setPosition(winSize / 2 - scrollSize / 2);

    size_t i = 0;
    for (auto& ev : history->getEvents()) {
        auto menu = CCMenu::create();
        menu->setContentSize({ scrollSize.width, 30.f });
        menu->ignoreAnchorPointForPosition(false);

        auto icon = CCSprite::createWithSpriteFrameName(ev->icon());
        icon->setPosition({ 15.f, 15.f });
        menu->addChild(icon);

        auto desc = CCLabelBMFont::create(ev->desc().c_str(), "bigFont.fnt");
        desc->limitLabelWidth(scrollSize.width - 80.f, .5f, .1f);
        desc->setAnchorPoint({ .0f, .5f });
        desc->setPosition(40.f, 15.f);
        menu->addChild(desc);

        if (i > history->getRedoneCount() - 1) {
            desc->setColor({ 155, 155, 155 });
        }

        auto border = CCLayerColor::create({ 0, 0, 0, 100 }, scrollSize.width, 1);
        menu->addChild(border);

        scroll->m_contentLayer->addChild(menu);

        i++;
    }

    scroll->m_contentLayer->setLayout(
        ColumnLayout::create()
            ->setAxisAlignment(AxisAlignment::End)
            ->setAxisReverse(true)
            ->setGap(0)
    );
    m_mainLayer->addChild(scroll);

    addListBorders(m_mainLayer, winSize / 2, scrollSize);

    return true;
}

HistoryPopup* HistoryPopup::create(History* history) {
    auto ret = new HistoryPopup;
    if (ret && ret->init(348.f, 280.f, history)) {
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

void History::undo() {
    if (m_undone < m_events.size()) {
        m_events.at(m_events.size() - m_undone - 1)->undo();
        m_undone += 1;
    }
}

void History::redo() {
    if (m_undone > 0) {
        m_undone -= 1;
        m_events.at(m_events.size() - m_undone - 1)->redo();
    }
}

bool History::canUndo() const {
    return m_undone < m_events.size();
}

bool History::canRedo() const {
    return m_undone > 0;
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
    return static_cast<HistoryLayer*>(lel)->m_fields->history;
}

struct $modify(HistoryUI, EditorUI) {
    bool init(LevelEditorLayer* lel) {
        if (!EditorUI::init(lel))
            return false;
        
        if (auto menu = this->getChildByID("undo-menu")) {
            menu->addChild(CCMenuItemSpriteExtra::create(
                CCSprite::createWithSpriteFrameName("btn_chatHistory_001.png"),
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
