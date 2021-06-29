#include "BetterEdit.hpp"

using namespace gdmake;
using namespace gd;
using namespace cocos2d;

#define STEP_SUBDICT_NC(dict, key, ...)         \
    if (dict->stepIntoSubDictWithKey(key)) {    \
        __VA_ARGS__                             \
        dict->stepOutOfSubDict();               \
    }

#define STEP_SUBDICT(dict, key, ...)            \
    {                                           \
    if (!dict->stepIntoSubDictWithKey(key)) {   \
        dict->setSubDictForKey(key);            \
        if (!dict->stepIntoSubDictWithKey(key)) \
            return;                             \
    }                                           \
    __VA_ARGS__                                 \
    dict->stepOutOfSubDict();                   \
    }

BetterEdit* g_betterEdit;



bool BetterEdit::init() {
    this->m_pTemplateManager = TemplateManager::sharedState();
    this->m_sFileName = "BetterEdit.dat";

    this->setKeyInt("scale-snap", 4);

    this->setup();

    return true;
}

void BetterEdit::encodeDataTo(DS_Dictionary* data) {
    STEP_SUBDICT(data, "settings",
        for (auto [key, value] : m_mSettingsDict)
            switch (value.type) {
                case BESetting::Int:
                    STEP_SUBDICT(data, "int",
                        data->setIntegerForKey(key.c_str(), value.data_n);
                    ) break;
                case BESetting::String:
                    STEP_SUBDICT(data, "string",
                        data->setStringForKey(key.c_str(), value.data_s);
                    ) break;
            }
    );

    STEP_SUBDICT(data, "templates",
        m_pTemplateManager->encodeDataTo(data);
    );

    STEP_SUBDICT(data, "presets",
        auto ix = 0u;
        for (auto preset : m_vPresets)
            STEP_SUBDICT(data, ("k" + std::to_string(ix)).c_str(),
                data->setStringForKey("name", preset.name);
                data->setStringForKey("data", preset.data);
                ix++;
            );
    );

    std::string favStr = "";
    for (auto fav : m_vFavorites)
        favStr += std::to_string(fav) + ",";
    data->setStringForKey("favorites", favStr);
}

void BetterEdit::dataLoaded(DS_Dictionary* data) {
    STEP_SUBDICT_NC(data, "settings",
        STEP_SUBDICT_NC(data, "int",
            for (auto key : data->getAllKeys())
                m_mSettingsDict.insert({ key, BESetting(data->getIntegerForKey(key.c_str())) });
        )
        STEP_SUBDICT_NC(data, "string",
            for (auto key : data->getAllKeys())
                m_mSettingsDict.insert({ key, BESetting(data->getStringForKey(key.c_str())) });
        )
    );

    STEP_SUBDICT_NC(data, "templates",
        m_pTemplateManager->dataLoaded(data);
    );

    STEP_SUBDICT_NC(data, "presets",
        for (auto key : data->getAllKeys())
            STEP_SUBDICT_NC(data, key.c_str(),
                m_vPresets.push_back({
                    data->getStringForKey("name"),
                    data->getStringForKey("data")
                });
            );
    );

    for (auto fav : stringSplit(data->getStringForKey("favorites"), ","))
        try { this->addFavorite(std::stoi(fav)); } catch (...) {}
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

