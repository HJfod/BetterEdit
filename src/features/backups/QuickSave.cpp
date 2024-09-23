#include "QuickSave.hpp"
#include <Geode/modify/EditorPauseLayer.hpp>
#include <Geode/modify/GManager.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <cvolton.level-id-api/include/EditorIDs.hpp>
#include <hjfod.gmd-api/include/GMD.hpp>
#include <utils/HolyUB.hpp>
#include <utils/Editor.hpp>

using namespace geode::prelude;

static std::filesystem::path getQuickSaveDir() {
    return Mod::get()->getSaveDir() / "quicksave";
}

// Autosave is stored separately, as if you press Exit without saving in the 
// editor then that should discard autosaves too, but not quicksaves
static std::filesystem::path getAutoSaveDir() {
    return Mod::get()->getSaveDir() / "autosave";
}

static bool CREATING_AUTO_SAVE = false;
void createAutoSave(LevelEditorLayer* lel) {
    CREATING_AUTO_SAVE = true;
    fakeEditorPauseLayer(lel)->saveLevel();
    CREATING_AUTO_SAVE = false;
}

static bool SKIP_NEXT_LLM_SAVE = false;
class $modify(GManager) {
	$override
	void save() {
        auto isLLM = static_cast<LocalLevelManager*>(static_cast<GManager*>(this)) == LocalLevelManager::get();
		if (isLLM && SKIP_NEXT_LLM_SAVE) {
            SKIP_NEXT_LLM_SAVE = false;
            return;
		}
        GManager::save();
        // If we have succesfully saved local levels, we can delete the temp quick save directory
        if (isLLM) {
            std::error_code ec;
            std::filesystem::remove_all(getQuickSaveDir(), ec);
            std::filesystem::remove_all(getAutoSaveDir(), ec);
        }
	}
};
class $modify(EditorPauseLayer) {
    $override
    void saveLevel() {
        // If quicksaving is disabled, save normally
        // In any case we need to still run the rest of `saveLevel` to have GD 
        // update the level state but for quicksave we just skip the function 
        // saving CCLocalLevels 
        SKIP_NEXT_LLM_SAVE = Mod::get()->template getSettingValue<bool>("quick-save") || CREATING_AUTO_SAVE;
        EditorPauseLayer::saveLevel();

        auto dir = CREATING_AUTO_SAVE ? getAutoSaveDir() : getQuickSaveDir();
        CREATING_AUTO_SAVE = false;

        // Save to individual file regardless as a backup if saving crashes
        (void)file::createDirectoryAll(dir);
        auto res = gmd::exportLevelAsGmd(
            m_editorLayer->m_level,
            dir / fmt::format("{}.gmd", EditorIDs::getID(m_editorLayer->m_level))
        );
        if (!res) {
            log::error("Unable to save level '{}': {}", m_editorLayer->m_level->m_levelName, res.unwrapErr());
        }
    }

    $override
    void FLAlert_Clicked(FLAlertLayer* fl, bool btn2) {
        if (be::isViewOnlyEditor(m_editorLayer)) {
            return EditorPauseLayer::FLAlert_Clicked(fl, btn2);
        }

        // Discard autosaves on exit without save
        if (fl->getTag() == 1 && btn2) {
            log::warn("Discarding autosaved changes");
            std::error_code ec;
            std::filesystem::remove_all(
                getAutoSaveDir() / fmt::format("{}.gmd", EditorIDs::getID(m_editorLayer->m_level)),
                ec
            );
        }
        EditorPauseLayer::FLAlert_Clicked(fl, btn2);
    }

    $override
    void onExitNoSave(CCObject* sender) {
        if (be::isViewOnlyEditor(m_editorLayer)) {
            return EditorPauseLayer::onExitNoSave(sender);
        }

        auto fl = FLAlertLayer::create(
            this,
            "Exit",
            "<cy>Exit</c> without saving? All unsaved changes will be <cr>lost</c>!\n"
            "<co>All auto-saved changes will also be discarded!</c>",
            "Cancel", "Exit",
            300
        );
        fl->setTag(1);
        fl->show();
    }
};
class $modify(MenuLayer) {
    bool init() {
        if (!MenuLayer::init())
            return false;
        
        // Only check once on startup so deleting levels doesn't become impossible
        static bool CHECKED_CRASHDATA = false;
        if (CHECKED_CRASHDATA) {
            return true;
        }
        CHECKED_CRASHDATA = true;

        restoreCrashData(getAutoSaveDir());

        // Important: Load Quick Saved crash data *after* autosaved crash data, 
        // as quick saved data is always more up-to-date
        restoreCrashData(getQuickSaveDir());
        
        return true;
    }

    static void restoreCrashData(std::filesystem::path const& path) {
        for (auto file : file::readDirectory(path).unwrapOrDefault()) {
            auto data = gmd::importGmdAsLevel(file);
            if (!data) continue;
            auto imported = *data;

            auto num = numFromString<int>(file.stem().string());
            if (!num) continue;

            // Restore existing level
            if (auto level = EditorIDs::getLevelByID(*num)) {
                level->m_levelString = imported->m_levelString;
                log::info("Restored crash data for {}", level->m_levelName);
            }
            else {
                LocalLevelManager::get()->m_localLevels->insertObject(imported, 0);
                log::info("Restored crashed level {}", imported->m_levelName);
            }
        }
    }
};
