#include "TemplateManager.hpp"

using namespace gdmake;

TemplateManager* g_templateManager;

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

        if (g_templateManager && g_templateManager->init())
            g_templateManager->autorelease();
        else {
            CC_SAFE_DELETE(g_templateManager);
            return nullptr;
        }
    }

    return g_templateManager;
}

GDMAKE_HOOK(0x3D5E0)
void __fastcall AppDelegate_trySaveGame(cocos2d::CCObject* self) {
    TemplateManager::sharedState()->save();

    return GDMAKE_ORIG_V(self);
}

GDMAKE_HOOK(0xCC500)
void __fastcall GameManager_dataLoadedHook(gd::GameManager* self, edx_t edx, DS_Dictionary* dict) {
    TemplateManager::sharedState();

    return GDMAKE_ORIG_V(self, edx, dict);
}

