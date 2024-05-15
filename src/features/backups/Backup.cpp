#include "Backup.hpp"
#include <Geode/loader/Dirs.hpp>
#include <hjfod.gmd-api/include/GMD.hpp>
#include <hjfod.bettersave/include/BetterSave.hpp>
#include <matjson/stl_serialize.hpp>
#include <fmt/chrono.h>

struct BackupMetadata final {
	typename Backup::TimePoint createTime = Backup::Clock::now();
};

template <>
struct matjson::Serialize<BackupMetadata> {
    static matjson::Value to_json(BackupMetadata const& meta) {
		return matjson::Object({
			{ "create-time", std::chrono::duration_cast<Backup::TimeUnit>(meta.createTime.time_since_epoch()).count() }
		});
	}
    static BackupMetadata from_json(matjson::Value const& value) {
		auto meta = BackupMetadata();
		auto obj = value.as_object();
        meta.createTime = Backup::TimePoint(Backup::TimeUnit(obj["create-time"].as_int()));
		return meta;
	}
	static bool is_json(matjson::Value const& value) {
		return value.is_object();
	}
};

GJGameLevel* Backup::getLevel() const {
    return m_level;
}
typename Backup::TimePoint Backup::getCreateTime() const {
    return m_createTime;
}

Result<> Backup::restoreThis() {
    // Add changes to memory
    m_forLevel->m_levelString = m_level->m_levelString;
    m_forLevel->m_levelName = m_level->m_levelName;
    m_forLevel->m_levelDesc = m_level->m_levelDesc;
    m_forLevel->m_levelLength = m_level->m_levelLength;
    m_forLevel->m_objectCount = m_level->m_objectCount;
    m_forLevel->m_audioTrack = m_level->m_audioTrack;
    m_forLevel->m_songID = m_level->m_songID;
    m_forLevel->m_songIDs = m_level->m_songIDs;
    m_forLevel->m_sfxIDs = m_level->m_sfxIDs;
    m_forLevel->m_twoPlayerMode = m_level->m_twoPlayerMode;

    // Save the level file to commit these changes
    GEODE_UNWRAP(save::created::saveLevel(m_forLevel));

    return Ok();
}
Result<> Backup::deleteThis() {
    std::error_code ec;
    ghc::filesystem::remove_all(m_directory, ec);
    if (ec) {
        return Err("Unable to delete backup directory: {} (code {})", ec.message(), ec.value());
    }
    return Ok();
}

Result<std::shared_ptr<Backup>> Backup::load(ghc::filesystem::path const& dir, GJGameLevel* forLevel) {
    GEODE_UNWRAP_INTO(auto level, gmd::importGmdAsLevel(dir / "level.gmd").expect("Unable to read level file: {error}"));
    auto meta = file::readFromJson<BackupMetadata>(dir / "meta.json").unwrapOrDefault();

    auto backup = std::make_shared<Backup>();
    backup->m_level = level;
    backup->m_forLevel = forLevel;
    backup->m_createTime = meta.createTime;
    backup->m_directory = dir;
    return Ok(backup);
}
std::vector<std::shared_ptr<Backup>> Backup::load(GJGameLevel* level) {
    std::vector<std::shared_ptr<Backup>> res;
    for (auto folder : file::readDirectory(save::getCurrentLevelSaveDir(level) / "backups").unwrapOrDefault()) {
        if (!ghc::filesystem::is_directory(folder)) {
            continue;
        }
        auto b = Backup::load(folder, level);
        if (!b) {
            log::error("Unable to load backup at {}: {}", folder, b.unwrapErr());
            continue;
        }
        res.push_back(*b);
    }
    std::reverse(res.begin(), res.end());
    return res;
}
Result<> Backup::create(GJGameLevel* level) {
    if (level->m_levelType != GJLevelType::Editor) {
        return Err("Can not backup a non-editor level");
    }
    auto time = Clock::now();

    auto dir = save::getCurrentLevelSaveDir(level) / "backups" / fmt::format("{:%Y-%m-%d_%H-%M}", time);

    if (ghc::filesystem::exists(dir)) {
        return Err("Level was already backed up less than a minute ago");
    }

    GEODE_UNWRAP(file::createDirectoryAll(dir).expect("Failed to create directory: {error}"));

    GEODE_UNWRAP(gmd::exportLevelAsGmd(level, dir / "level.gmd").expect("Unable to save level: {error}"));

    auto metadata = BackupMetadata {
        .createTime = time
    };
    GEODE_UNWRAP(file::writeToJson(dir / "meta.json", metadata).expect("Unable to save metadata: {error}"));

    return Ok();
}
