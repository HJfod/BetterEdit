#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/binding/PlayerObject.hpp>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>

using namespace geode::prelude;

struct ButtonPushed {
    float x;
    int player;
    bool dual;
    bool down;
};

enum class Replay {
    None,
    Recording,
    Playback,
};

static std::vector<ButtonPushed> RECORDED_DATA {};
static Replay RECORDING = Replay::None;
static size_t REPLAY_INDEX = 0;

struct $modify(GJBaseGameLayer) {
    void pushButton(int player, bool dual) {
        GJBaseGameLayer::pushButton(player, dual);
        if (RECORDING == Replay::Recording) {
            RECORDED_DATA.push_back(ButtonPushed {
                .x = m_player1->getPositionX(),
                .player = player,
                .dual = dual,
                .down = true,
            });
        }
    }

    void releaseButton(int player, bool dual) {
        GJBaseGameLayer::releaseButton(player, dual);
        if (RECORDING == Replay::Recording) {
            RECORDED_DATA.push_back(ButtonPushed {
                .x = m_player1->getPositionX(),
                .player = player,
                .dual = dual,
                .down = false,
            });
        }
    }
};

struct $modify(ReplayUI, EditorUI) {
    CCMenuItemSpriteExtra* replayBtn;

    bool init(LevelEditorLayer* lel) {
        if (!EditorUI::init(lel))
            return false;
        
        auto menu = this->getChildByID("playtest-menu");
        auto replayBtnSpr = CCSprite::createWithSpriteFrameName("GJ_getSongInfoBtn_001.png");
        menu->addChild(m_fields->replayBtn = CCMenuItemSpriteExtra::create(
            replayBtnSpr, this, menu_selector(ReplayUI::onReplay)
        ));
        menu->getLayout()->ignoreInvisibleChildren(true);
        this->updateReplay();

        return true;
    }

    bool ccTouchBegan(CCTouch* touch, CCEvent* event) {
        // Prevent clicks if replaying and instead allow moving camera and shit
        if (
            RECORDING == Replay::Playback &&
            m_editorLayer->m_playbackMode == PlaybackMode::Playing
        ) {
            m_editorLayer->m_playbackMode = PlaybackMode::Paused;
            auto ret = EditorUI::ccTouchBegan(touch, event);
            m_editorLayer->m_playbackMode = PlaybackMode::Playing;
            return ret;
        }
        return EditorUI::ccTouchBegan(touch, event);
    }

    void ccTouchMoved(CCTouch* touch, CCEvent* event) {
        // Prevent clicks if replaying and instead allow moving camera and shit
        if (
            RECORDING == Replay::Playback &&
            m_editorLayer->m_playbackMode == PlaybackMode::Playing
        ) {
            m_editorLayer->m_playbackMode = PlaybackMode::Paused;
            EditorUI::ccTouchMoved(touch, event);
            m_editorLayer->m_playbackMode = PlaybackMode::Playing;
            return;
        }
        EditorUI::ccTouchMoved(touch, event);
    }

    void ccTouchEnded(CCTouch* touch, CCEvent* event) {
        // Prevent clicks if replaying and instead allow moving camera and shit
        if (
            RECORDING == Replay::Playback &&
            m_editorLayer->m_playbackMode == PlaybackMode::Playing
        ) {
            m_editorLayer->m_playbackMode = PlaybackMode::Paused;
            EditorUI::ccTouchEnded(touch, event);
            m_editorLayer->m_playbackMode = PlaybackMode::Playing;
            return;
        }
        EditorUI::ccTouchEnded(touch, event);
    }

    void onPlaytest(CCObject* sender) {
        if (m_editorLayer->m_playbackMode == PlaybackMode::Not) {
            REPLAY_INDEX = 0;
            if (RECORDING == Replay::None) {
                RECORDED_DATA.clear();
                RECORDING = Replay::Recording;
            }
        }
        EditorUI::onPlaytest(sender);
        this->updateReplay();
    }

    void playtestStopped() {
        EditorUI::playtestStopped();
        RECORDING = Replay::None;
        this->updateReplay();
    }

    void onReplay(CCObject*) {
        RECORDING = Replay::Playback;
        this->onPlaytest(nullptr);
    }

    void updateReplay() {
        m_fields->replayBtn->setVisible(
            RECORDING == Replay::None &&
            RECORDED_DATA.size()
        );
        m_fields->replayBtn->getParent()->updateLayout();
    }
};

struct $modify(LevelEditorLayer) {
    void destructor() {
        LevelEditorLayer::~LevelEditorLayer();
        RECORDED_DATA.clear();
        REPLAY_INDEX = 0;
        RECORDING = Replay::None;
    }

    void update(float dt) {
        LevelEditorLayer::update(dt);
        if (RECORDING == Replay::Playback) {
            if (REPLAY_INDEX < RECORDED_DATA.size()) {
                auto push = RECORDED_DATA.at(REPLAY_INDEX);
                if (push.x < m_player1->getPositionX()) {
                    if (push.down) {
                        this->pushButton(push.player, push.dual);
                    }
                    else {
                        this->releaseButton(push.player, push.dual);
                    }
                    REPLAY_INDEX += 1;
                }
            }
        }
    }
};
