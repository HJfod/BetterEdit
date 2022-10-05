#include <Geode/Bindings.hpp>
#include <Geode/Modify.hpp>
#include <Geode/Loader.hpp>
#include <BetterEdit.hpp>
#include <Geode/utils/WackyGeodeMacros.hpp>

#undef min
#undef max

USE_GEODE_NAMESPACE();

class $modify(StartPosAwareLevel, GJGameLevel) {
    std::optional<CCPoint> m_startPos;
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
        static_cast<StartPosAwareLevel*>(
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
            if (auto pos = static_cast<StartPosAwareLevel*>(
                m_level
            )->m_fields->m_startPos) {
                if (obj->getPosition() == pos.value()) {
                    m_startPos = static_cast<StartPosObject*>(obj);
                    m_playerStartPosition = obj->getPosition();
                }
            }
        }
    }
};

class $modify(StartPosSwitchLayer, LevelEditorLayer) {
    StartPosObject* m_selectedStartPos;

    StartPosSwitchLayer() : m_selectedStartPos(nullptr) {
        PlaytestHerePopup::hide();
    }

    void addSpecial(GameObject* obj) {
        LevelEditorLayer::addSpecial(obj);
        if (obj->m_objectID == 31) {
            if (auto pos = static_cast<StartPosAwareLevel*>(m_level)->m_fields->m_startPos) {
                if (pos.value() == obj->getPosition()) {
                    m_fields->m_selectedStartPos = static_cast<StartPosObject*>(obj);
                }
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
        if (!m_fields->m_selectedStartPos) {
            return LevelEditorLayer::setupLevelStart(obj);
        }
        this->setStartPosObject(m_fields->m_selectedStartPos);
        auto startPos = m_fields->m_selectedStartPos->getPosition();
        m_player1->setStartPos(startPos);
        m_player1->resetObject();
        m_player2->setStartPos(startPos);
        m_player2->resetObject();
        LevelEditorLayer::setupLevelStart(
            m_fields->m_selectedStartPos->m_levelSettings
        );
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

class $modify(EditorPauseLayer) {
    bool init(LevelEditorLayer* lel) {
        if (!EditorPauseLayer::init(lel))
            return false;
        
        PlaytestHerePopup::hide();

        return true;
    }
};

class $modify(EditorUI) {
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
