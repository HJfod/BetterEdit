#include "BetterEdit.hpp"
#include "tools/EditorLayerInput/LayerManager.hpp"
#include "tools/AutoSave/Backup/LevelBackupManager.hpp"
#include "tools/CustomKeybinds/KeybindManager.hpp"

using namespace gdmake;
using namespace gdmake::extra;
using namespace gd;
using namespace cocos2d;

log_stream g_obLogStream = log_stream();
std::vector<DebugMsg> g_internalLog;

std::string DebugTypeToStr(DebugType type) {
    switch (type) {
        case kDebugTypeGeneric:         return "General";
        case kDebugTypeHook:            return "Hook";
        case kDebugTypeInitializing:    return "Initializing";
        case kDebugTypeDeinitializing:  return "Deinitializing";
        case kDebugTypeSaving:          return "Saving";
        case kDebugTypeLoading:         return "Loading";
        case kDebugTypeError:           return "Error";
        case kDebugTypeMinorError:      return "Minor Error";
        case kDebugTypeFatalError:      return "Fatal Error";
        default:                        return "";
    }
}

bool DSdictHasKey(DS_Dictionary* dict, std::string const& key) {
    return dict->getKey(dict->getIndexOfKey(key.c_str())) == key;
}

log_stream::log_stream() {
    if (std::filesystem::exists(g_sLogfileDat)) {
        try {
            this->type = std::stoi(readFileString(g_sLogfileDat));
        } catch (...) {}
    }
    this->setType(this->type);
    this->type |= kLogTypeInternal;
    writeFileString(g_sLogfile, "");
}

void log_stream::setType(int t) {
    this->type = t;
    writeFileString(g_sLogfileDat, std::to_string(t));
}

log_stream& log_stream::operator<<(log_end end) {
    if (!this->type)
        return *this;

    auto s = "[ " + timePointAsString(std::chrono::system_clock::now()) + "] " +
        this->output.str() + "\n";

    if (this->type & kLogTypeFile) {
        std::ofstream outfile;
        outfile.open(g_sLogfile, std::ios_base::app);
        outfile << s;
        outfile.close();
    }
    if (this->type & kLogTypeConsole) {
        std::cout << s;
    }
    if (this->type & kLogTypeInternal) {
        g_internalLog.push_back({ this->dbgType, s });
        if (g_internalLog.size() > 200) {
            g_internalLog.erase(g_internalLog.begin() + 200, g_internalLog.end());
        }
    }

    this->dbgType = kDebugTypeGeneric;
    this->output.str(std::string());

    return *this;
}

log_stream& BetterEdit::log() {
    return g_obLogStream;
}

std::vector<DebugMsg>& BetterEdit::internal_log() {
    return g_internalLog;
}

#define BE_SAVE_Bool(_name_, _get_, _def_) \
    data->setBoolForKey(_name_, _get_ ^ _def_)
#define BE_SAVE_Float(_name_, _get_, _def_) \
    data->setFloatForKey(_name_, _get_)
#define BE_SAVE_Integer(_name_, _get_, _def_) \
    data->setIntegerForKey(_name_, _get_)

#define BE_LOAD_Bool(_name_, _get_, _def_) \
    set##_name_(_get_(#_name_) ^ _def_)
#define BE_LOAD_Integer(_name_, _get_, _def_) \
    set##_name_(_get_(#_name_))
#define BE_LOAD_Float(_name_, _get_, _def_) \
    set##_name_(_get_(#_name_))

#define BE_SAVE_SETTING(__name__, _, _d_, __ctype__, _0, _1, _2, _3) \
    BE_SAVE_##__ctype__##(#__name__, get##__name__(), _d_);
#define BE_LOAD_SETTING(__name__, _, _d_, __ctype__, _0, _1, _2, _3) \
    if (DSdictHasKey(data, #__name__)) \
        BE_LOAD_##__ctype__##(__name__, data->get##__ctype__##ForKey, _d_);
#define BE_DEFAULT_SETTING(__name__, _, __value__, __, ___, _0, _1, _2) \
    set##__name__(__value__);

BetterEdit* g_betterEdit;



bool BetterEdit::init() {
    this->m_sFileName = "BetterEdit.dat";

    if (!LayerManager::initGlobal())
        return false;

    // BE_SETTINGS(BE_DEFAULT_SETTING)

    this->setup();

    return true;
}

void BetterEdit::encodeDataTo(DS_Dictionary* data) {
    BetterEdit::log() << kDebugTypeSaving << "Saving Settings" << log_end();
    STEP_SUBDICT(data, "settings",
        BE_SETTINGS(BE_SAVE_SETTING)
    );

    BetterEdit::log() << kDebugTypeSaving << "Saving Bools" << log_end();
    STEP_SUBDICT(data, "bools",
        for (auto & [key, val] : m_mSaveBools)
            if (val.global)
                data->setBoolForKey(key.c_str(), *val.global);
    );

    // BetterEdit::log() << kDebugTypeSaving << "Saving Editor Layers" << log_end();
    // STEP_SUBDICT(data, "editor-layers",
    //     LayerManager::get()->encodeDataTo(data);
    // );

    BetterEdit::log() << kDebugTypeSaving << "Saving Presets" << log_end();
    STEP_SUBDICT(data, "presets",
        auto ix = 0u;
        for (auto preset : m_vPresets)
            STEP_SUBDICT(data, ("k" + std::to_string(ix)).c_str(),
                data->setStringForKey("name", preset.name);
                data->setStringForKey("data", preset.data);
                ix++;
            );
    );

    BetterEdit::log() << kDebugTypeSaving << "Saving Backups" << log_end();
    LevelBackupManager::get()->save();

    BetterEdit::log() << kDebugTypeSaving << "Saving Keybinds" << log_end();
    KeybindManager::get()->save();
}

void BetterEdit::dataLoaded(DS_Dictionary* data) {
    BetterEdit::log() << kDebugTypeLoading << "Loading Settings" << log_end();
    STEP_SUBDICT_NC(data, "settings",
        BE_SETTINGS(BE_LOAD_SETTING)
    );

    BetterEdit::log() << kDebugTypeLoading << "Loading Bools" << log_end();
    STEP_SUBDICT_NC(data, "bools",
        for (auto key : data->getAllKeys())
            m_mSaveBools[key] = { nullptr, data->getBoolForKey(key.c_str()) };
    );

    // BetterEdit::log() << kDebugTypeLoading << "Loading Editor Layers" << log_end();
    // STEP_SUBDICT_NC(data, "editor-layers",
    //     LayerManager::get()->dataLoaded(data);
    // );

    BetterEdit::log() << kDebugTypeLoading << "Loading Presets" << log_end();
    STEP_SUBDICT_NC(data, "presets",
        for (auto key : data->getAllKeys())
            STEP_SUBDICT_NC(data, key.c_str(),
                m_vPresets.push_back({
                    data->getStringForKey("name"),
                    data->getStringForKey("data")
                });
            );
    );
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

void BetterEdit::loadTextures() {
    this->log() << kDebugTypeInitializing << "Loading Textures" << log_end();

    for (auto const& tex : this->m_vTextureSheets) {
        CCTextureCache::sharedTextureCache()
            ->addImage((tex + ".png").c_str(), false);
        CCSpriteFrameCache::sharedSpriteFrameCache()
            ->addSpriteFramesWithFile((tex + ".plist").c_str());
    }
}

void BetterEdit::FLAlert_Clicked(FLAlertLayer* l, bool btn2) {
    if (l->getTag() == 2) {
        if (btn2) {
            BetterEdit::setEnableExperimentalFeatures(true);
            as<CCFuncPointer*>(l->getUserObject())->invoke();
        }
    }
}

bool BetterEdit::useExperimentalFeatures(std::function<void()> msg_cb) {
    if (BetterEdit::getEnableExperimentalFeatures())
        return true;

    if (msg_cb) {
        auto fl = FLAlertLayer::create(
            BetterEdit::sharedState(),
            "Experimental Features",
            "Cancel", "Enable",
            380.0f,
            "You're trying to use an <cb>Experimental Feature.</c>\n "
            "These features are likely <cy>unfinished</c> and may "
            "<cr>crash</c>, or just not work at all.\n "
            "Would you like to <cg>enable Experimental Features</c>?"
        );
        fl->setTag(2);
        fl->setUserObject(CCFuncPointer::create(msg_cb));
        fl->show();
    }

    return false;
}

void BetterEdit::showHookConflictMessage() {
    FLAlertLayer::create(
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

