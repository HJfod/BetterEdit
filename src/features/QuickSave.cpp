#include <Geode/modify/EditorPauseLayer.hpp>
#include <Geode/modify/GManager.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <cvolton.level-id-api/include/EditorIDs.hpp>
#include <hjfod.gmd-api/include/GMD.hpp>

using namespace geode::prelude;

static std::filesystem::path getQuickSaveDir() {
    return Mod::get()->getSaveDir() / "quicksave";
}
static std::filesystem::path getQuickSaveFile(GJGameLevel* level) {
    return getQuickSaveDir() / fmt::format("{}.gmd", EditorIDs::getID(level));
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
        SKIP_NEXT_LLM_SAVE = Mod::get()->template getSettingValue<bool>("quick-save");
        EditorPauseLayer::saveLevel();

        // Save to individual file regardless as a backup if saving crashes
        (void)file::createDirectoryAll(getQuickSaveDir());
        auto res = gmd::exportLevelAsGmd(m_editorLayer->m_level, getQuickSaveFile(m_editorLayer->m_level));
        if (!res) {
            log::error("Unable to quicksave level '{}': {}", m_editorLayer->m_level->m_levelName, res.unwrapErr());
        }
    }
};
class $modify(MenuLayer) {
    bool init() {
        if (!MenuLayer::init())
            return false;
        
        // Check if there's quicksaved crash data
        for (auto file : file::readDirectory(getQuickSaveDir()).unwrapOrDefault()) {
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

        return true;
    }
};
