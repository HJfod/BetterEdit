#pragma once

#include <GDMake.h>
#include "tools/Templates/TemplateManager.hpp"
#include "utils.hpp"
#include <set>
#include <algorithm>

#pragma region macros (ew)
#define BE_SETTINGS(__macro__)                                                                  \
    __macro__(ScaleSnap, int, 4, Integer, std::stoi, BE_MAKE_SFUNC_RANGE, 0, 10)                \
    __macro__(GridSize, float, 30.0f, Float, std::stof, BE_MAKE_SFUNC_RANGE, 7.5f, 120.0f)      \
    __macro__(GridSizeEnabled, bool, false, Bool, std::stoi, BE_MAKE_SFUNC, _, _)               \
    __macro__(PasteStateEnabled, bool, true, Bool, std::stoi, BE_MAKE_SFUNC, _, _)              \

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
    set##__name__(static_cast<__type__>(__conv__(value))); }                                \
    static std::string get##__name__##AsString() { return formatToString(get##__name__()); }
#pragma endregion macros (ew)

class BetterEdit : public gd::GManager {
    public:
        struct Preset {
            std::string name;
            std::string data;
        };
    
        using FavoritesList = std::vector<int>;
    
    protected:
        TemplateManager* m_pTemplateManager;
        std::vector<Preset> m_vPresets;
        std::vector<std::string> m_vScheduledErrors;
        FavoritesList m_vFavorites;

        BE_SETTINGS(BE_MAKE_SETTING)

    protected:
        virtual bool init() override;

        virtual void encodeDataTo(DS_Dictionary* data);
        virtual void dataLoaded(DS_Dictionary* data);
        virtual void firstLoad();
    
    public:
        template<typename T>
        inline static std::string formatToString(T num) {
            std::string res = std::to_string(num);

            if (std::is_same<T, float>::value && res.find('.') != std::string::npos) {
                while (res.at(res.length() - 1) == '0')
                    res = res.substr(0, res.length() - 1);
                
                if (res.at(res.length() - 1) == '.')
                    res = res.substr(0, res.length() - 1);
            }

            return res;
        }

        bool m_bHookConflictFound = true;
        
        static BetterEdit* sharedState();
        static bool initGlobal();

        static void showHookConflictMessage();

        inline void scheduleError(std::string const& err) { this->m_vScheduledErrors.push_back(err); }
        inline std::vector<std::string> & getErrors() { return this->m_vScheduledErrors; }

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
