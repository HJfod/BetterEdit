#pragma once

#include <GDMake.h>
#include "templates/TemplateManager.hpp"

class BetterEdit : public gd::GManager {
    protected:
        TemplateManager* m_pTemplateManager;
        cocos2d::CCDictionary* m_pSettingsDict;

        virtual bool init() override;

        virtual void encodeDataTo(DS_Dictionary* data);
        virtual void dataLoaded(DS_Dictionary* data);
        virtual void firstLoad();
    
    public:
        static BetterEdit* sharedState();
        static bool initGlobal();

        int getKeyInt(const char* key);
        BetterEdit* setKeyInt(const char* key, int val);
};
