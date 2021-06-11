#pragma once

#include <GDMake.h>
#include "templates/TemplateManager.hpp"
#include "utils.hpp"

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
    protected:
        TemplateManager* m_pTemplateManager;
        std::map<std::string, BESetting> m_mSettingsDict;

        virtual bool init() override;

        virtual void encodeDataTo(DS_Dictionary* data);
        virtual void dataLoaded(DS_Dictionary* data);
        virtual void firstLoad();
    
    public:
        static BetterEdit* sharedState();
        static bool initGlobal();

        int getKeyInt(std::string const& key);
        BetterEdit* setKeyInt(std::string const& key, int val);

        inline std::map<std::string, BESetting> getSettingsDict() { return m_mSettingsDict; }
};
