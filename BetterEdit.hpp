#pragma once

#include <GDMake.h>
#include <GUI/CCControlExtension/CCScale9Sprite.h>
#include "tools/Templates/TemplateManager.hpp"
#include "utils.hpp"
#include "utils/InfoButton.hpp"
#include "tools/CustomUI/UIManager.hpp"
#include <set>
#include <algorithm>

using namespace gd;
using namespace gdmake;
using namespace gdmake::extra;
using namespace cocos2d;
using namespace cocos2d::extension;

bool DSdictHasKey(DS_Dictionary* dict, std::string const& key);

enum DebugLogType {
    kLogTypeNone    = 0x0,
    kLogTypeConsole = 0x1,
    kLogTypeFile    = 0x2,
};

#pragma region macros (ew)
#define BE_SETTINGS(__macro__)                                                                  \
    __macro__(ScaleSnap, float, .25f, Float, std::stof, BE_MAKE_SFUNC_RANGE, 0.01f, 1.0f)       \
    __macro__(GridSize, float, 30.0f, Float, std::stof, BE_MAKE_SFUNC_RANGE, 0.9375f, 120.0f)   \
    __macro__(ObjectsPreStartLine, int, 15, Integer, std::stoi, BE_MAKE_SFUNC_RANGE, 1, 60)     \
    __macro__(PercentageAccuracy, int, 0, Integer, std::stoi, BE_MAKE_SFUNC_RANGE, 0, 10)       \
    __macro__(AutoSaveTime, int, 1, Integer, std::stoi, BE_MAKE_SFUNC_RANGE, 1, 60)             \
    __macro__(SoftSaveFrequency, int, 1, Integer, std::stoi, BE_MAKE_SFUNC_RANGE, 1, 999)       \
    __macro__(GridSizeEnabled, bool, false, Bool, std::stoi, BE_MAKE_SFUNC, _, _)               \
    __macro__(AlwaysUseCustomGridSize, bool, false, Bool, std::stoi, BE_MAKE_SFUNC, _, _)       \
    __macro__(PasteStateEnabled, bool, true, Bool, std::stoi, BE_MAKE_SFUNC, _, _)              \
    __macro__(DisableMouseZoomMove, bool, false, Bool, std::stoi, BE_MAKE_SFUNC, _, _)          \
    __macro__(FadeOutPercentage, bool, false, Bool, std::stoi, BE_MAKE_SFUNC, _, _)             \
    __macro__(PulseObjectsInEditor, bool, false, Bool, std::stoi, BE_MAKE_SFUNC, _, _)          \
    __macro__(HasShownPulseVolumeWarning, bool, false, Bool, std::stoi, BE_MAKE_SFUNC, _, _)    \
    __macro__(RotateSawsInEditor, bool, false, Bool, std::stoi, BE_MAKE_SFUNC, _, _)            \
    __macro__(DisableEditorPos, bool, false, Bool, std::stoi, BE_MAKE_SFUNC, _, _)              \
    __macro__(DisableZoomText, bool, false, Bool, std::stoi, BE_MAKE_SFUNC, _, _)               \
    __macro__(DisablePercentage, bool, false, Bool, std::stoi, BE_MAKE_SFUNC, _, _)             \
    __macro__(DisableExtraObjectInfo, bool, false, Bool, std::stoi, BE_MAKE_SFUNC, _, _)        \
    __macro__(DisableFavoritesTab, bool, false, Bool, std::stoi, BE_MAKE_SFUNC, _, _)           \
    __macro__(DisableGlobalClipboard, bool, false, Bool, std::stoi, BE_MAKE_SFUNC, _, _)        \
    __macro__(ShowAllLayers, bool, false, Bool, std::stoi, BE_MAKE_SFUNC, _, _)                 \
    __macro__(EnableControlA, bool, false, Bool, std::stoi, BE_MAKE_SFUNC, _, _)                \
    __macro__(BypassObjectLimit, bool, false, Bool, std::stoi, BE_MAKE_SFUNC, _, _)             \
    __macro__(UseOldProgressBar, bool, false, Bool, std::stoi, BE_MAKE_SFUNC, _, _)             \
    __macro__(RepeatCopyPaste, bool, false, Bool, std::stoi, BE_MAKE_SFUNC, _, _)               \
    __macro__(EnableRelativeSwipe, bool, false, Bool, std::stoi, BE_MAKE_SFUNC, _, _)           \
    __macro__(DisableNewColorSelection, bool, false, Bool, std::stoi, BE_MAKE_SFUNC, _, _)      \
    __macro__(UseUpArrowForGameplay, bool, false, Bool, std::stoi, BE_MAKE_SFUNC, _, _)         \
    __macro__(EnableAutoSave, bool, false, Bool, std::stoi, BE_MAKE_SFUNC, _, _)                \
    __macro__(EnableSoftAutoSave, bool, false, Bool, std::stoi, BE_MAKE_SFUNC, _, _)            \
    __macro__(EnableAsyncSave, bool, false, Bool, std::stoi, BE_MAKE_SFUNC, _, _)               \
    __macro__(DisableVisibilityTab, bool, false, Bool, std::stoi, BE_MAKE_SFUNC, _, _)          \
    __macro__(AutoUpdateEnabled, bool, false, Bool, std::stoi, BE_MAKE_SFUNC, _, _)             \
    __macro__(AutoUpdateBetaTest, bool, false, Bool, std::stoi, BE_MAKE_SFUNC, _, _)            \
    __macro__(DontColorGamemodeBorders, bool, false, Bool, std::stoi, BE_MAKE_SFUNC, _, _)      \
    __macro__(HighlightTriggers, bool, false, Bool, std::stoi, BE_MAKE_SFUNC, _, _)             \
    __macro__(DisableDoubleClick, bool, false, Bool, std::stoi, BE_MAKE_SFUNC, _, _)            \
    __macro__(KeybindRepeatEnabled, bool, true, Bool, std::stoi, BE_MAKE_SFUNC, _, _)           \
    __macro__(KeybindRepeatInterval, int, 300, Integer, std::stoi, BE_MAKE_SFUNC_RANGE, 1,10000)\
    __macro__(KeybindRepeatStart, int, 700, Integer, std::stoi, BE_MAKE_SFUNC_RANGE, 1, 10000)  \
    __macro__(ShowKeybindOnHover, bool, true, Bool, std::stoi, BE_MAKE_SFUNC, _, _)             \
    __macro__(MoveCameraWhenMovingObjects, bool, true, Bool, std::stoi, BE_MAKE_SFUNC, _, _)    \
    __macro__(ShownTutorial, bool, false, Bool, std::stoi, BE_MAKE_SFUNC, _, _)                 \

#define STEP_SUBDICT_NC(dict, key, ...)         \
    if (dict->stepIntoSubDictWithKey(key)) {    \
        __VA_ARGS__                             \
        dict->stepOutOfSubDict();               \
    }

#define ITERATE_STEP_SUBDICT_NC(dict, key, ...) \
    for (auto const& key :                      \
        dict->getAllKeys()) {                   \
        STEP_SUBDICT_NC(                        \
            dict,                               \
            key.c_str(),                        \
            __VA_ARGS__                         \
        );                                      \
    }                                           \

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

#define BE_MAKE_SFUNC(__name__, __type__, _, __, ___)       \
    static void set##__name__##(__type__ value) {           \
        sharedState()->m_Sett##__name__ = value;            \
    }                                                       \
    static void set##__name__##OrDefault(__type__ value) {  \
        set##__name__##(value);                             \
    }                                                       \

#define BE_MAKE_SFUNC_RANGE(__name__, __type__, __value__, __lbound__, __rbound__)      \
    static void set##__name__##(__type__ value) {                                       \
        if (value < __lbound__) sharedState()->m_Sett##__name__ = __lbound__;           \
        else if (value > __rbound__) sharedState()->m_Sett##__name__ = __rbound__;      \
        else sharedState()->m_Sett##__name__ = value;                                   \
    }                                                                                   \
    static void set##__name__##OrDefault(__type__ value) {                              \
        if (value < __lbound__) sharedState()->m_Sett##__name__ = __value__;            \
        else if (value > __rbound__) sharedState()->m_Sett##__name__ = __value__;       \
        else sharedState()->m_Sett##__name__ = value;                                   \
    }                                                                                   \

// jesus fucking christ
#define BE_MAKE_SETTING(__name__, __type__, __value__, _, __conv__, __sfunc__, __lb__, __rb__)  \
    protected: __type__ m_Sett##__name__ = __value__;                                       \
    public: __sfunc__(__name__, __type__, __value__, __lb__, __rb__)                        \
    static __type__ get##__name__() { return sharedState()->m_Sett##__name__; }             \
    static void set##__name__##FromString(std::string const& value) {                       \
    try { set##__name__(static_cast<__type__>(__conv__(value))); } catch (...) {} }         \
    static std::string get##__name__##AsString() { return formatToString(get##__name__()); }
#pragma endregion macros (ew)

static constexpr const char* g_sLogfileDat = "BE_log_t.dat";
static constexpr const char* g_sLogfile = "BE_log.log";

struct log_end {};

class log_stream {
    protected:
        std::stringstream output;
        int type = 0;

    public:
        log_stream();

        void setType(int t);
        inline int getType() { return type; }

        inline log_stream& operator<<(std::string const& s) {
            output << s; return *this;
        }
        inline log_stream& operator<<(const char* s) {
            output << s; return *this;
        }
        inline log_stream& operator<<(int n) {
            output << n; return *this;
        }
        inline log_stream& operator<<(long n) {
            output << n; return *this;
        }
        inline log_stream& operator<<(CCPoint p) {
            output << p.x << ", " << p.y; return *this;
        }
        log_stream& operator<<(log_end n);
};

class BetterEdit : public gd::GManager {
    public:
        struct Preset {
            std::string name;
            std::string data;
        };
    
        using FavoritesList = std::vector<int>;

        enum ScheduleTime {
            kScheduleTimeMenuLayer,
        };
    
    protected:
        struct save_bool {
            bool* global;
            bool loaded;
            bool skip_loaded = false;
        };

        TemplateManager* m_pTemplateManager;
        std::vector<Preset> m_vPresets;
        std::map<ScheduleTime, std::vector<std::string>> m_mScheduledErrors;
        std::map<std::string, save_bool> m_mSaveBools;
        FavoritesList m_vFavorites;
        bool m_bEditorInitialized;
        bool m_bDisableEditorEditing;

        BE_SETTINGS(BE_MAKE_SETTING)

    protected:
        virtual bool init() override;

        virtual void encodeDataTo(DS_Dictionary* data);
        virtual void dataLoaded(DS_Dictionary* data);
        virtual void firstLoad();
    
    public:
        template<typename T>
        inline static std::string formatToString(T num, unsigned int precision = 60u) {
            std::string res = std::to_string(num);

            if (std::is_same<T, float>::value && res.find('.') != std::string::npos) {
                while (res.at(res.length() - 1) == '0')
                    res = res.substr(0, res.length() - 1);
                
                if (res.at(res.length() - 1) == '.')
                    res = res.substr(0, res.length() - 1);

                if (res.find('.') != std::string::npos) {
                    auto pos = res.find('.');

                    if (precision)
                        res = res.substr(0, pos + 1 + precision);
                    else
                        res = res.substr(0, pos);
                }
            }

            return res;
        }

        static inline bool isEditorInitialized() {
            return BetterEdit::sharedState()->m_bEditorInitialized;
        }
        static void setEditorInitialized(bool b) {
            BetterEdit::sharedState()->m_bEditorInitialized = b;
        }

        static inline bool isEditorViewOnlyMode() {
            return BetterEdit::sharedState()->m_bDisableEditorEditing;
        }
        static void setEditorViewOnlyMode(bool b) {
            BetterEdit::sharedState()->m_bDisableEditorEditing = b;
        }

        static inline void saveGlobalBool(std::string name, bool* b) {
            auto be = BetterEdit::sharedState();

            if (be->m_mSaveBools.count(name)) {
                if (be->m_mSaveBools[name].skip_loaded)
                    return;

                *b = be->m_mSaveBools[name].loaded;
            }

            be->m_mSaveBools[name] = { b, *b, true };
        }

        bool m_bHookConflictFound = true;
        
        static BetterEdit* sharedState();
        static bool initGlobal();

        static void showHookConflictMessage();

        static log_stream& log();

        inline void scheduleError(ScheduleTime time, std::string const& err) {
            if (!this->m_mScheduledErrors.count(time))
                this->m_mScheduledErrors[time] = std::vector<std::string>();

            this->m_mScheduledErrors[time].push_back(err);
        }
        inline std::vector<std::string> & getErrors(ScheduleTime time) {
            return this->m_mScheduledErrors[time]; 
        }

        inline std::vector<Preset> & getPresets() { return m_vPresets; }
        inline BetterEdit* addPreset(Preset const& preset) { m_vPresets.push_back(preset); return this; }
        inline BetterEdit* removePreset(unsigned int index) { m_vPresets.erase(m_vPresets.begin() + index); return this; }
        inline BetterEdit* removePreset(std::string const& name) {
            auto ix = 0u;
            for (auto preset : m_vPresets)
                if (preset.name == name)
                    removePreset(ix);
                else ix++;
            return this;
        }

        inline FavoritesList & getFavorites() { return m_vFavorites; }
        inline bool addFavorite(int id) {
            if (std::find(m_vFavorites.begin(), m_vFavorites.end(), id) != m_vFavorites.end())
                return false;
            m_vFavorites.push_back(id);
            return true;
        }
        inline bool removeFavorite(int id) {
            for (size_t ix = 0; ix < m_vFavorites.size(); ix++)
                if (m_vFavorites[ix] == id) {
                    m_vFavorites.erase(m_vFavorites.begin() + ix);
                    return true;
                }
            
            return false;
        }
        inline void moveFavorite(int id, int pos) {
            auto idPosIt = std::find(m_vFavorites.begin(), m_vFavorites.end(), id);

            if (idPosIt == m_vFavorites.end()) return;

            size_t idPos = std::distance(m_vFavorites.begin(), idPosIt);
            
            if (idPos + pos < 0) return;
            if (idPos + pos > m_vFavorites.size() - 1) return;

            vectorMove(m_vFavorites, idPos, idPos + pos);
        } 
};
