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
    this->m_pSettingsDict = cocos2d::CCDictionary::create();
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



int BetterEdit::getKeyInt(const char* key) {
    return this->m_pSettingsDict->valueForKey(key)->intValue();
}

BetterEdit* BetterEdit::setKeyInt(const char* key, int val) {
    this->m_pSettingsDict->setObject(
        cocos2d::CCString::create(std::to_string(val)),
        key
    );

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

