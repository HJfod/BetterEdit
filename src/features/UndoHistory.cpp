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
    m_history = history;

    this->setTitle("History");

    auto winSize = CCDirector::get()->getWinSize();

    auto scrollPos = CCPoint(winSize / 2) - CCPoint { 0.f, 10.f };
    auto scrollSize = CCSize { 346.f, 220.f };
    auto scroll = ScrollLayer::create(scrollSize);
    scroll->setPosition(scrollPos - scrollSize / 2);

    float height = 0;
    for (auto& ev : history->getEvents()) {
        auto menu = CCMenu::create();
        menu->setContentSize({ scrollSize.width, 30.f });
        menu->ignoreAnchorPointForPosition(false);

        auto icon = ev->icon();
        icon->setPosition({ 20.f, 15.f });
        menu->addChild(icon);

        auto desc = CCLabelBMFont::create(ev->desc().c_str(), "bigFont.fnt");
        desc->limitLabelWidth(scrollSize.width - 80.f, .5f, .1f);
        desc->setAnchorPoint({ .0f, .5f });
        desc->setPosition(40.f, 15.f);
        desc->setID("name");
        menu->addChild(desc);

        auto undoBtnSpr = CCSprite::createWithSpriteFrameName("GJ_undoBtn_001.png");
        undoBtnSpr->setScale(.4f);
        auto undoBtn = CCMenuItemSpriteExtra::create(
            undoBtnSpr, this, menu_selector(HistoryPopup::onUndoTill)
        );
        undoBtn->setPosition(scrollSize.width - 20.f, 15.f);
        undoBtn->setUserData(ev);
        undoBtn->setID("undo-button");
        menu->addChild(undoBtn);

        auto border = CCLayerColor::create({ 0, 0, 0, 100 }, scrollSize.width, 1);
        menu->addChild(border);

        scroll->m_contentLayer->addChild(menu);
        height += 30.f;

        m_items.push_back(menu);
    }

    this->updateState();

    if (height < scrollSize.height) {
        height = scrollSize.height;
    }

    scroll->m_contentLayer->setContentSize({
        scroll->m_contentLayer->getContentSize().width,
        height
    });
    scroll->m_contentLayer->setLayout(
        ColumnLayout::create()
            ->setAxisAlignment(AxisAlignment::End)
            ->setAxisReverse(true)
            ->setGap(0)
    );
    m_mainLayer->addChild(scroll);

    addListBorders(m_mainLayer, scrollPos, scrollSize);

    return true;
}

void HistoryPopup::onUndoTill(CCObject* sender) {
    m_history->undoTo(static_cast<EditorEvent*>(
        static_cast<CCNode*>(sender)->getUserData()
    ));
    this->updateState();
}

void HistoryPopup::onRedoTill(CCObject* sender) {
    m_history->redoTo(static_cast<EditorEvent*>(
        static_cast<CCNode*>(sender)->getUserData()
    ));
    this->updateState();
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
    return m_events.at(m_events.size() - m_undone - 1).get();
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
    while (m_undone < m_events.size() && this->current() != event) {
        this->current()->undo();
        m_undone += 1;
    }
}

void History::redo(size_t count) {
    while (count--) {
        if (m_undone > 0) {
            m_undone -= 1;
            this->current()->redo();
        } else break;
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

void HistoryPopup::updateState() {
    size_t i = 0;
    for (auto& item : m_items) {
        auto redoable = (i > m_history->getRedoneCount() - 1);
        static_cast<CCLabelBMFont*>(item->getChildByID("name"))->setColor(
            redoable ? ccColor3B { 155, 155, 155 } : ccColor3B { 255, 255, 255 }
        );
        auto undoBtn = static_cast<CCMenuItemSpriteExtra*>(item->getChildByID("undo-button"));
        static_cast<CCSprite*>(undoBtn->getNormalImage())
            ->setDisplayFrame(CCSpriteFrameCache::get()->spriteFrameByName(
                redoable ? "GJ_redoBtn_001.png" : "GJ_undoBtn_001.png"
            ));
        undoBtn->setTarget(this, redoable ?
            menu_selector(HistoryPopup::onRedoTill) :
            menu_selector(HistoryPopup::onUndoTill)
        );
        i += 1;
    }
    static_cast<HistoryUI*>(EditorUI::get())->updateUIAfterAction();
}
