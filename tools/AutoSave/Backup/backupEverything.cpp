#include "LevelBackupManager.hpp"
#include "../../gmd/gmd.hpp"

std::unordered_map<std::string, size_t> g_nameCounts;

struct bbbbb : public LevelBrowserLayer {
    void backupEverything(CCObject*) {
        g_nameCounts.clear();

        std::filesystem::path dir = CCFileUtils::sharedFileUtils()->getWritablePath();
        dir /= stringReplace(
            stringReplace(timePointAsString(std::chrono::system_clock::now()), ":", "."),
            " ", "_"
        );
        try { std::filesystem::create_directories(dir); } catch(...) {}

        size_t count = 0;
        std::vector<std::string> errors;
        CCARRAY_FOREACH_B_TYPE(LocalLevelManager::sharedState()->m_pLevels, lvl, GJGameLevel) {
            std::string suffix = "";
            if (g_nameCounts.count(lvl->m_sLevelName)) {
                g_nameCounts.insert({ lvl->m_sLevelName, 1 });
            } else {
                g_nameCounts[lvl->m_sLevelName]++;
                suffix += "_" + std::to_string(g_nameCounts[lvl->m_sLevelName]);
            }
            gmd::GmdFile file(lvl);
            file.setFileName(lvl->m_sLevelName + suffix);
            file.setType(gmd::kGmdTypeGmd2);
            file.setPath(dir);
            auto err = file.saveFile();
            if (!err) {
                errors.push_back(lvl->m_sLevelName + ": " + err.error);
            }
            count++;
        }
        auto text = "Saved <co>" + std::to_string(count) + "</c> levels to <cg>" + dir.string() + "</c>";
        if (errors.size()) {
            text += "\nErrors: ";
            for (auto& err : errors) {
                text += err + ", ";
            }
            text.pop_back();
            text.pop_back();
        }
        FLAlertLayer::create(nullptr, "Info", "OK", nullptr, 420.f, text)->show();
    }
};

GDMAKE_HOOK(0x15a040)
bool __fastcall LevelBrowserLayer_init(LevelBrowserLayer* self, edx_t edx, GJSearchObject* search) {
    if (!GDMAKE_ORIG(self, edx, search))
        return false;

    if (search->m_nScreenID == kGJSearchTypeMyLevels) {
        auto winSize = CCDirector::sharedDirector()->getWinSize();
        
        auto menu = CCMenu::create();
        menu->setPosition(winSize.width / 2, 30.f);

        auto btnSpr = ButtonSprite::create("Backup All", 0, 0, "bigFont.fnt", "GJ_button_01.png", 0, .8f);
        btnSpr->setScale(.6f);

        auto btn = CCMenuItemSpriteExtra::create(btnSpr, self, menu_selector(bbbbb::backupEverything));
        btn->setPosition(0, 0);
        menu->addChild(btn);

        self->addChild(menu);
    }

    return true;
}
