#pragma once

#include <GDMake.h>

class Template;

class TemplateManager : public gd::GManager {
    std::vector<Template*> m_vTemplates;

    virtual bool init() override;

    virtual void encodeDataTo(DS_Dictionary* data);
    virtual void dataLoaded(DS_Dictionary* data);
    virtual void firstLoad();

    public:
        static TemplateManager* sharedState();
};
