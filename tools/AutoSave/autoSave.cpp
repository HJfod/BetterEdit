#include "autoSave.hpp"

static constexpr const int AUTOSAVETIMER_TAG = 4234509;
SEL_SCHEDULE g_timerFunc;

class EditorUI_Timer : public EditorUI {
    public:
        void onSaveTimer(float dt) {
            auto timer = as<AutoSaveTimer*>(this->getChildByTag(AUTOSAVETIMER_TAG));

            if (timer) {
                if (!BetterEdit::getEnableAutoSave() || BetterEdit::isEditorViewOnlyMode()) {
                    timer->getLabel()->setOpacity(0);
                    timer->resetTimer();
                    return;
                }

                if (timer->paused())
                    return;
                
                timer->incrementTimer();

                auto time = timer->getTimerLeft();

                if (time && time <= 10)
                    timer->showStatusTime();
                
                if (time <= 0) {
                    timer->showStatus("Autosaving...");
                    timer->getLabel()->draw();

                    auto pauseLayer = EditorPauseLayer::create(this->m_pEditorLayer);
                    pauseLayer->saveLevel();
                    pauseLayer->release();

                    timer->resetTimer();
                }
            }
        }
};

CCLabelBMFont* AutoSaveTimer::getLabel() {
    return m_pLabel;
}

void AutoSaveTimer::cancel() {
    this->resetTimer();

    this->showStatus("Autosave Cancelled!");
}

bool AutoSaveTimer::cancellable() {
    return this->getTimerLeft() <= 10;
}

void AutoSaveTimer::pause() {
    this->m_bPaused = true;
}

void AutoSaveTimer::resume() {
    this->m_bPaused = false;
}

bool AutoSaveTimer::paused() {
    return this->m_bPaused;
}

bool AutoSaveTimer::init() {
    m_pLabel = CCLabelBMFont::create("", "bigFont.fnt");
    m_pLabel->setScale(.5f);
    m_pLabel->setOpacity(0);

    this->addChild(m_pLabel);

    return true;
}

AutoSaveTimer* AutoSaveTimer::create() {
    auto pRet = new AutoSaveTimer;

    if (pRet && pRet->init()) {
        pRet->autorelease();
        return pRet;
    }

    CC_SAFE_DELETE(pRet);
    return nullptr;
}

int AutoSaveTimer::getTimer() {
    return m_nTimer;
}

int AutoSaveTimer::getTimerLeft() {
    return BetterEdit::getAutoSaveTime() * 60 - m_nTimer;
}

void AutoSaveTimer::setTimer(int time) {
    m_nTimer = time;
}

void AutoSaveTimer::resetTimer() {
    m_nTimer = 0;
}

void AutoSaveTimer::incrementTimer() {
    m_nTimer++;
}

void AutoSaveTimer::showStatusTime() {
    this->showStatus(
        CCString::createWithFormat(
            "Autosaving in %i seconds", this->getTimerLeft()
        )->getCString()
    );
}

void AutoSaveTimer::showStatus(const char* text) {
    m_pLabel->setString(text);
    m_pLabel->setOpacity(255);
    m_pLabel->stopAllActions();
    m_pLabel->runAction(
        CCSequence::create(CCArray::create(
            CCDelayTime::create(1.0f),
            CCFadeOut::create(2.0f),
            nullptr
        ))
    );
}

void loadAutoSaveTimer(EditorUI* ui) {
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    auto timer = AutoSaveTimer::create();
    timer->setPosition(winSize.width / 2, winSize.height - 80.0f);
    timer->setTag(AUTOSAVETIMER_TAG);
    ui->addChild(timer);
    g_timerFunc = (SEL_SCHEDULE)&EditorUI_Timer::onSaveTimer;
    ui->schedule(g_timerFunc, 1.0f);
}

void resetAutoSaveTimer(EditorUI* ui) {
    auto timer = as<AutoSaveTimer*>(ui->getChildByTag(AUTOSAVETIMER_TAG));
    
    if (timer) {
        timer->resetTimer();
    }

    ui->unschedule(g_timerFunc);
}

AutoSaveTimer* getAutoSaveTimer(EditorUI* ui) {
    auto timer = as<AutoSaveTimer*>(ui->getChildByTag(AUTOSAVETIMER_TAG));
    
    return timer;
}
