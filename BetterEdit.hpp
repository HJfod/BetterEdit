#pragma once

#include <GDMake.h>
#include "tools/Templates/TemplateManager.hpp"
#include "utils.hpp"
#include <set>

struct BESetting {
    enum SType { Int, String };
    SType type;
    union {
        int data_n;
        std::string data_s;
    };

    BESetting() {}
    BESetting(BESetting const& sett) {
        this->type = sett.type;
        switch (sett.type) {
            case SType::Int: this->data_n = sett.data_n; break;
            case SType::String: this->data_s = sett.data_s; break;
        }
    }
    BESetting& operator=(BESetting const& sett) {
        this->type = sett.type;
        switch (sett.type) {
            case SType::Int: this->data_n = sett.data_n; break;
            case SType::String: this->data_s = sett.data_s; break;
        }

        return *this;
    }
    ~BESetting() {}

    BESetting(int x) {
        this->data_n = x;
        this->type = SType::Int;
    }

    BESetting(std::string x) {
        this->data_s = x;
        this->type = SType::String;
    }
};

class BetterEdit : public gd::GManager {
    public:
        struct Preset {
            std::string name;
            std::string data;
        };
    
        using FavoritesList = std::set<int>;
    
    protected:
        TemplateManager* m_pTemplateManager;
        std::map<std::string, BESetting> m_mSettingsDict;
        std::vector<Preset> m_vPresets;
        FavoritesList m_vFavorites;

        virtual bool init() override;

        virtual void encodeDataTo(DS_Dictionary* data);
        virtual void dataLoaded(DS_Dictionary* data);
        virtual void firstLoad();
    
    public:
        bool m_bHookConflictFound = true;
        
        static BetterEdit* sharedState();
        static bool initGlobal();

        int getKeyInt(std::string const& key);
        BetterEdit* setKeyInt(std::string const& key, int val);

        inline std::map<std::string, BESetting> getSettingsDict() { return m_mSettingsDict; }
        static void showHookConflictMessage();

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
            if (m_vFavorites.count(id))
                return false;
            m_vFavorites.insert(id);
            return true;
        }
        inline bool removeFavorite(int id) {
            auto res = m_vFavorites.count(id);
            m_vFavorites.erase(id);
            return res;
        }
};
