#include "BetterEdit.hpp"
#include "tools/EditorLayerInput/LayerManager.hpp"
#include "tools/AutoSave/Backup/LevelBackupManager.hpp"

using namespace gdmake;
using namespace gdmake::extra;
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

#define BE_SAVE_SETTING(__name__, _, __, __ctype__, _0, _1, _2, _3) data->set##__ctype__##ForKey(#__name__, get##__name__());
#define BE_LOAD_SETTING(__name__, _, __, __ctype__, _0, _1, _2, _3) set##__name__##OrDefault(data->get##__ctype__##ForKey(#__name__));
#define BE_DEFAULT_SETTING(__name__, _, __value__, __, ___, _0, _1, _2) set##__name__(__value__);

BetterEdit* g_betterEdit;



bool BetterEdit::init() {
    this->m_pTemplateManager = TemplateManager::sharedState();
    this->m_sFileName = "BetterEdit.dat";

    // BE_SETTINGS(BE_DEFAULT_SETTING)

    this->setup();

    return true;
}

void BetterEdit::encodeDataTo(DS_Dictionary* data) {
    STEP_SUBDICT(data, "settings",
        BE_SETTINGS(BE_SAVE_SETTING)
    );

    STEP_SUBDICT(data, "templates",
        m_pTemplateManager->encodeDataTo(data);
    );

    STEP_SUBDICT(data, "editor-layers",
        LayerManager::get()->encodeDataTo(data);
    );

    STEP_SUBDICT(data, "level-backups",
        LevelBackupManager::get()->encodeDataTo(data);
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
        BE_SETTINGS(BE_LOAD_SETTING)
    );

    STEP_SUBDICT_NC(data, "templates",
        m_pTemplateManager->dataLoaded(data);
    );

    STEP_SUBDICT_NC(data, "editor-layers",
        LayerManager::get()->dataLoaded(data);
    );

    STEP_SUBDICT_NC(data, "level-backups",
        LevelBackupManager::get()->dataLoaded(data);
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

