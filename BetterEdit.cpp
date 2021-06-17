#include "BetterEdit.hpp"

using namespace gdmake;
using namespace gd;
using namespace cocos2d;

#define STEP_SUBDICT(dict, key, ...)            \
    if (!dict->stepIntoSubDictWithKey(key)) {   \
        dict->setSubDictForKey(key);            \
        if (!dict->stepIntoSubDictWithKey(key)) \
            return;                             \
    }                                           \
    __VA_ARGS__                                 \
    dict->stepOutOfSubDict();

BetterEdit* g_betterEdit;



bool BetterEdit::init() {
    this->m_pTemplateManager = TemplateManager::sharedState();
    this->m_sFileName = "BetterEdit.dat";

    this->setKeyInt("scale-snap", 4);

    this->setup();

    return true;
}

void BetterEdit::encodeDataTo(DS_Dictionary* data) {
    STEP_SUBDICT(data, "templates",
        m_pTemplateManager->encodeDataTo(data);
    );
}

void BetterEdit::dataLoaded(DS_Dictionary* data) {
    STEP_SUBDICT(data, "templates",
        m_pTemplateManager->dataLoaded(data);
    );
}

void BetterEdit::firstLoad() {}



int BetterEdit::getKeyInt(std::string const& key) {
    auto si = this->m_mSettingsDict.find(key);
    if (si != this->m_mSettingsDict.end()) {
        auto val = this->m_mSettingsDict[key];

        if (val.type == BESetting::Int)
            return val.data_n;
    }
    
    return 0;
}

BetterEdit* BetterEdit::setKeyInt(std::string const& key, int val) {
    if (key.size())
        this->m_mSettingsDict[key] = BESetting(val);

    return this;
}



BetterEdit* BetterEdit::sharedState() {
    return g_betterEdit;
}

bool BetterEdit::initGlobal() {
    g_betterEdit = new BetterEdit();

    if (g_betterEdit && g_betterEdit->init())
        return true;

    CC_SAFE_DELETE(g_betterEdit);
    return false;
}


void BetterEdit::showHookConflictMessage() {
    gd::FLAlertLayer::create(
        nullptr,
        "Hook Conflict Detected",
        "OK", nullptr,
        380.0f,
        "It appears that you have other <cp>mods</c> installed which "
        "are <cy>conflicting</c> with <cl>BetterEdit</c>.\n\n"

        "Mods that may have caused this include <co>GroupIDFilter</c>, "
        "<co>Global Clipboard</c>, and other editor mods.\n\n"

        "Please <cr>remove</c> or <cg>load</c> the mods at a different "
        "loading phase. Contact <cy>HJfod#1795</c> for help."
    )->show();
}


GDMAKE_HOOK(0x3D5E0)
void __fastcall AppDelegate_trySaveGame(cocos2d::CCObject* self) {
    BetterEdit::sharedState()->save();

    return GDMAKE_ORIG_V(self);
}

GDMAKE_HOOK(0xCC500)
void __fastcall GameManager_dataLoadedHook(gd::GameManager* self, edx_t edx, DS_Dictionary* dict) {
    BetterEdit::sharedState();

    return GDMAKE_ORIG_V(self, edx, dict);
}

