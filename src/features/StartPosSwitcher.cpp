#include <Geode/Geode.hpp>
#include <BetterEdit.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
#include <Geode/modify/GJGameLevel.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/GameObject.hpp>
#undef min
#undef max

USE_GEODE_NAMESPACE();

struct FromLevelStart {};
struct DefaultBehaviour {};
using FromPoint = CCPoint;
using FromObj = StartPosObject*;
using StartPosKind = std::variant<
    FromLevelStart,
    DefaultBehaviour,
    FromPoint,
    FromObj
>;

class SelectStartPosPopup : public Popup<LevelEditorLayer*> {
protected:
    LevelEditorLayer* m_editor;

    bool setup(LevelEditorLayer* editor) override {
        m_editor = editor;
        m_noElasticity = true;

        this->setTitle("Select Start Pos");

        auto goToStartSpr = ButtonSprite::create(
            "From Level Start", "goldFont.fnt", "GJ_button_01.png"
        );
        goToStartSpr->setScale(.7f);
        auto goToStartBtn = CCMenuItemSpriteExtra::create(
            goToStartSpr, this, menu_selector(SelectStartPosPopup::onSelectFromStart)
        );
        goToStartBtn->setPosition(0.f, 20.f);
        m_buttonMenu->addChild(goToStartBtn);

        auto goToEndSpr = ButtonSprite::create(
            "From Last Start Pos", "goldFont.fnt", "GJ_button_01.png"
        );
        goToEndSpr->setScale(.7f);
        auto goToEndBtn = CCMenuItemSpriteExtra::create(
            goToEndSpr, this, menu_selector(SelectStartPosPopup::onSelectFromLast)
        );
        goToEndBtn->setPosition(0.f, -20.f);
        m_buttonMenu->addChild(goToEndBtn);

        return true;
    }

    void onSelectFromStart(CCObject*);
    void onSelectFromLast(CCObject*);

public:
    static SelectStartPosPopup* create(LevelEditorLayer* editor) {
        auto ret = new SelectStartPosPopup();
        if (ret && ret->init(220.f, 150.f, editor)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};

class $modify(StartPosLevel, GJGameLevel) {
    StartPosKind m_startPos;

    StartPosLevel() : m_startPos(DefaultBehaviour()) {}

    bool match(CCPoint const& pos) {
        if (std::holds_alternative<FromPoint>(m_fields->m_startPos)) {
            return std::get<FromPoint>(m_fields->m_startPos) == pos;
        }
        return false;
    }
};

class PlaytestHerePopup : public CCNode {
protected:
    Ref<StartPosObject> m_startPos;
    LevelEditorLayer* m_editor;
    static Ref<PlaytestHerePopup> s_popup;

    bool init(LevelEditorLayer* lel, StartPosObject* startPos) {
        if (!CCNode::init())
            return false;
        
        m_editor = lel;
        m_startPos = startPos;

        this->setZOrder(0xB00B5);

        auto bg = CCScale9Sprite::create("square02_001.png", { 0, 0, 80, 80 });
        bg->setScale(.5f);
        bg->setOpacity(80);
        bg->setContentSize({ 120, 60 });
        this->addChild(bg);

        auto menu = CCMenu::create();
        menu->setPosition(0, 0);

        auto playTestBtnSpr = CCSprite::createWithSpriteFrameName("GJ_playEditorBtn_001.png");
        playTestBtnSpr->setScale(.615f);

        auto playTestBtn = CCMenuItemSpriteExtra::create(
            playTestBtnSpr, this, menu_selector(PlaytestHerePopup::onPlaytest)
        );
        playTestBtn->setPosition(-15.f, .0f);
        menu->addChild(playTestBtn);

        auto playInGameBtnSpr = CCSprite::createWithSpriteFrameName("GJ_playBtn2_001.png");
        playInGameBtnSpr->setScale(.3f);

        auto playInGameBtn = CCMenuItemSpriteExtra::create(
            playInGameBtnSpr, this, menu_selector(PlaytestHerePopup::onPlayInGame)
        );
        playInGameBtn->setPosition(15.f, .0f);
        menu->addChild(playInGameBtn);

        this->addChild(menu);

        CCDirector::sharedDirector()->getTouchDispatcher()->incrementForcePrio(2);
        menu->registerWithTouchDispatcher();

        s_popup = this;

        return true;
    }

    static PlaytestHerePopup* create(
        LevelEditorLayer* lel, StartPosObject* startPos
    ) {
        auto ret = new PlaytestHerePopup;
        if (ret && ret->init(lel, startPos)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }

    // has to be defined outside because it used StartPosSwitchLayer
    void onPlaytest(CCObject*);
    void onPlayInGame(CCObject*) {
        this->select();
        m_editor->m_editorUI->onPause(nullptr);
    }

public:
    void select() {
        static_cast<StartPosLevel*>(
            m_editor->m_level
        )->m_fields->m_startPos = m_startPos->getPosition();
    }

    static void move() {
        if (s_popup) {
            s_popup->setPosition(
                s_popup->m_startPos->getPositionX(),
                s_popup->m_startPos->getPositionY() + 35.f
            );
        }
    }

    static void show(LevelEditorLayer* lel, StartPosObject* startPos) {
        PlaytestHerePopup::hide();
        PlaytestHerePopup::create(lel, startPos);
        PlaytestHerePopup::move();
        lel->m_objectLayer->addChild(s_popup);
    }

    static void hide() {
        if (s_popup) {
            s_popup->removeFromParent();
            s_popup = nullptr;
        }
    }
};
Ref<PlaytestHerePopup> PlaytestHerePopup::s_popup = nullptr;

class $modify(PlayLayer) {
	void addObject(GameObject* obj) {
        auto oldStartPos = m_startPos;
		PlayLayer::addObject(obj);
        if (oldStartPos) {
            m_startPos = oldStartPos;
            m_playerStartPosition = oldStartPos->getPosition();
        }
        if (obj->m_objectID == 31) {
            if (static_cast<StartPosLevel*>(m_level)->match(obj->getPosition())) {
                m_startPos = static_cast<StartPosObject*>(obj);
                m_playerStartPosition = obj->getPosition();
            }
            if (std::holds_alternative<FromLevelStart>(static_cast<StartPosLevel*>(
                m_level
            )->m_fields->m_startPos)) {
                m_startPos = nullptr;
                m_playerStartPosition = CCPoint { 0, 105.f };
            }
        }
    }
};

class $modify(StartPosSwitchLayer, LevelEditorLayer) {
    StartPosKind m_selectedStartPos;

    StartPosSwitchLayer() : m_selectedStartPos(DefaultBehaviour()) {
        PlaytestHerePopup::hide();
    }

    void addSpecial(GameObject* obj) {
        LevelEditorLayer::addSpecial(obj);
        if (obj->m_objectID == 31) {
            if (static_cast<StartPosLevel*>(m_level)->match(obj->getPosition())) {
                m_fields->m_selectedStartPos = static_cast<StartPosObject*>(obj);
            }
        }
    }

    void destructor() {
        PlaytestHerePopup::hide();
    }

    void handleAction(bool idk, CCArray* idk2) {
        LevelEditorLayer::handleAction(idk, idk2);
        PlaytestHerePopup::move();
    }

    void setupLevelStart(LevelSettingsObject* obj) {
        // selected start position
        if (std::holds_alternative<FromObj>(m_fields->m_selectedStartPos)) {
            auto obj = std::get<FromObj>(m_fields->m_selectedStartPos);
            this->setStartPosObject(obj);
            auto startPos = obj->getPosition();
            m_player1->setStartPos(startPos);
            m_player1->resetObject();
            m_player2->setStartPos(startPos);
            m_player2->resetObject();
            LevelEditorLayer::setupLevelStart(obj->m_levelSettings);
        }
        // from level start
        else if (std::holds_alternative<FromLevelStart>(m_fields->m_selectedStartPos)) {
            this->setStartPosObject(nullptr);
            m_player1->setStartPos({ 0.f, 105.f });
            m_player1->resetObject();
            m_player2->setStartPos({ 0.f, 105.f });
            m_player2->resetObject();
            LevelEditorLayer::setupLevelStart(m_levelSettings);
        }
        // default behaviour
        else {
            LevelEditorLayer::setupLevelStart(obj);
        }
    }
};

void PlaytestHerePopup::onPlaytest(CCObject*) {
    this->select();
    static_cast<StartPosSwitchLayer*>(
        m_editor
    )->m_fields->m_selectedStartPos = m_startPos;
    m_editor->setStartPosObject(m_startPos);
    EditorUI::get()->onPlaytest(EditorUI::get()->m_playtestBtn);
}

void SelectStartPosPopup::onSelectFromStart(CCObject*) {
    static_cast<StartPosSwitchLayer*>(
        m_editor
    )->m_fields->m_selectedStartPos = FromLevelStart();
    static_cast<StartPosLevel*>(
        m_editor->m_level
    )->m_fields->m_startPos = FromLevelStart();
    this->onClose(nullptr);
}

void SelectStartPosPopup::onSelectFromLast(CCObject*) {
    StartPosObject* last = nullptr;
	for(auto* obj : cocos::CCArrayExt<StartPosObject>(m_editor->m_objects)) {
        if (obj->m_objectID == 31) {
            if (!last || last->getPositionX() < obj->getPositionX()) {
                last = obj;
            }
        }
    }
    if (last) {
        static_cast<StartPosSwitchLayer*>(
            m_editor
        )->m_fields->m_selectedStartPos = last;
        static_cast<StartPosLevel*>(
            m_editor->m_level
        )->m_fields->m_startPos = last->getPosition();
    } else {
        static_cast<StartPosSwitchLayer*>(
            m_editor
        )->m_fields->m_selectedStartPos = DefaultBehaviour();
        static_cast<StartPosLevel*>(
            m_editor->m_level
        )->m_fields->m_startPos = DefaultBehaviour();
    }
    this->onClose(nullptr);
}

class $modify(EditorPauseLayer) {
    bool init(LevelEditorLayer* lel) {
        if (!EditorPauseLayer::init(lel))
            return false;
        
        PlaytestHerePopup::hide();

        return true;
    }
};

class $modify(MyEditorUI, EditorUI) {
    CCMenuItemSpriteExtra* m_startPosBtn;

    void onSelectStartPos(CCObject*) {
        SelectStartPosPopup::create(m_editorLayer)->show();
    }

    bool init(LevelEditorLayer* lel) {
        if (!EditorUI::init(lel))
            return false;
        
        auto startPosInnerSpr = CCSprite::createWithSpriteFrameName(
            "start-pos-border.png"_spr
        );
        startPosInnerSpr->setScale(.75f);

        auto startPosSpr = CircleButtonSprite::create(startPosInnerSpr);
        startPosSpr->setScale(.8f);

        m_fields->m_startPosBtn = CCMenuItemSpriteExtra::create(
            startPosSpr, this, menu_selector(MyEditorUI::onSelectStartPos)
        );
        m_fields->m_startPosBtn->setPosition(
            m_playtestBtn->getPositionX() + 50.f,
            m_playtestBtn->getPositionY()
        );
        m_playtestBtn->getParent()->addChild(m_fields->m_startPosBtn);

        return true;
    }

    void showUI(bool show) {
        EditorUI::showUI(show);
        m_fields->m_startPosBtn->setVisible(show);
    }

    void moveObject(GameObject* obj, CCPoint pos) {
        EditorUI::moveObject(obj, pos);
        PlaytestHerePopup::move();
    }

    void moveObjectCall(EditCommand cmd) {
        EditorUI::moveObjectCall(cmd);
        PlaytestHerePopup::move();
    }

    void deselectAll() {
        EditorUI::deselectAll();
        PlaytestHerePopup::hide();
    }
};

class $modify(GameObject) {
    void selectObject(ccColor3B const& color) {
        GameObject::selectObject(color);
        PlaytestHerePopup::hide();
        if (m_objectID == 31) {
            PlaytestHerePopup::show(
                LevelEditorLayer::get(),
                static_cast<StartPosObject*>(as<GameObject*>(this))
            );
        }
    }
};
