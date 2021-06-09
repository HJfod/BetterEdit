#include "TemplateManager.hpp"

using namespace gdmake;

TemplateManager* g_templateManager;

bool TemplateManager::init() {
    return true;
}

void TemplateManager::dataLoaded(DS_Dictionary* data) {

}

void TemplateManager::firstLoad() {

}

void TemplateManager::encodeDataTo(DS_Dictionary* data) {
    data->setIntegerForKey("version", 1);
    data->setIntegerForKey("count", this->m_vTemplates.size());

    if (!data->stepIntoSubDictWithKey("templates")) {
        data->setSubDictForKey("templates");

        if (!data->stepIntoSubDictWithKey("templates"))
            return;
    }

    for (auto ix = 0u; ix < this->m_vTemplates.size(); ix++) {
        auto key = std::string("k") + std::to_string(ix);

        data->setSubDictForKey(key.c_str());
        if (!data->stepIntoSubDictWithKey(key.c_str()))
            return;

        // auto kit = dynamic_cast<IconKitObject*>(this->m_vKits->objectAtIndex(ix));
        // kit->encodeWithCoder(data);

        data->stepOutOfSubDict();
    }

    data->stepOutOfSubDict();
}

TemplateManager* TemplateManager::sharedState() {
    if (!g_templateManager) {
        g_templateManager = new TemplateManager();

        if (!(g_templateManager && g_templateManager->init())) {
            CC_SAFE_DELETE(g_templateManager);
            return nullptr;
        }
    }

    return g_templateManager;
}
