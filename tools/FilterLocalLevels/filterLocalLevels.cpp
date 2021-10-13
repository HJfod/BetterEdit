#include "filterLocalLevels.hpp"
#include "levelBrowserHook.hpp"

std::unordered_map<LocalLevelFilter, ThreeWayBool> g_mStates;

void LevelBrowserLayer_CB::onFilterSearch(CCObject*) {
    LocalLevelFilterLayer::create(this)->show();
}

void LocalLevelFilterLayer::setup() {
    this->m_bNoElasticity = true;

    this->m_mStates = g_mStates;

    this->addToggle("Verified", kLocalLevelFilterVerified);
    this->addToggle("Uploaded", kLocalLevelFilterUploaded);
    this->addToggle("Copied",   kLocalLevelFilterCopied);
    this->addToggle("Song",     kLocalLevelFilterSong);
    this->addToggle("2-Player", kLocalLevelFilter2Player);
    this->addToggle("Coins",    kLocalLevelFilterCoins);

    this->registerWithTouchDispatcher();
    CCDirector::sharedDirector()->getTouchDispatcher()->incrementForcePrio(2);
}

void LocalLevelFilterLayer::onToggle(ThreeWayBoolSelect* sel, ThreeWayBool val) {
    this->m_mStates[static_cast<LocalLevelFilter>(sel->getTag())] = val;
}

void LocalLevelFilterLayer::addToggle(const char* text, LocalLevelFilter filter) {
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    auto sel = ThreeWayBoolSelect::create(
        text, this, threeway_selector(LocalLevelFilterLayer::onToggle)
    );
    sel->setPosition(
        winSize.width / 2 - 105.f + (m_nFilterCount % 4) * 70.f,
        winSize.height / 2 + 70.f - (m_nFilterCount / 4) * 50.f
    );
    sel->setTag(filter);
    this->m_pLayer->addChild(sel);

    this->m_nFilterCount++;
}

LocalLevelFilterLayer::~LocalLevelFilterLayer() {
    g_mStates = this->m_mStates;
}

LocalLevelFilterLayer* LocalLevelFilterLayer::create(LevelBrowserLayer* browser) {
    auto ret = new LocalLevelFilterLayer;

    if (ret && (ret->m_pBrowserLayer = browser) && ret->init(
        400.f, 260.f, "GJ_square01.png", "Filtered Search"
    )) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}

