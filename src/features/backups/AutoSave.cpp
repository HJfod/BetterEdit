#include <Geode/modify/EditorUI.hpp>
#include "Backup.hpp"

using namespace geode::prelude;

static std::chrono::seconds getAutoSaveInterval() {
    auto rate = Mod::get()->template getSettingValue<std::string>("auto-save-rate");
    switch (hash(rate.c_str())) {
        case hash("Every 10 Minutes"): return std::chrono::minutes(10);
        case hash("Every 20 Minutes"): return std::chrono::minutes(20);
        case hash("Every Hour"): return std::chrono::hours(1);
        default:
        case hash("Never"): return std::chrono::seconds(0);
    }
}

class $modify(AutoSaveUI, EditorUI) {
    struct Fields {
        std::chrono::seconds timeSinceLastAutoSave = std::chrono::seconds(0);
        Ref<Notification> autoSaveCountdownNotification;
    };

    bool init(LevelEditorLayer* editor) {
        if (!EditorUI::init(editor))
            return false;
        
        if (getAutoSaveInterval() > std::chrono::seconds(0)) {
            this->schedule(schedule_selector(AutoSaveUI::onAutoSaveTick), 1);
        }

        return true;
    }
    void onAutoSaveTick(float) {
        constexpr auto COUNTDOWN = std::chrono::seconds(5);

        auto interval = getAutoSaveInterval();

        m_fields->timeSinceLastAutoSave += std::chrono::seconds(1);

        // Never do autosaving when playtesting
        if (m_editorLayer->m_playbackMode != PlaybackMode::Not) {
            return;
        }

        if (m_fields->timeSinceLastAutoSave > interval - COUNTDOWN) {
            // Make sure the autosave notification exists
            if (!m_fields->autoSaveCountdownNotification) {
                m_fields->autoSaveCountdownNotification = Notification::create(
                    "", CCSprite::createWithSpriteFrameName("GJ_timeIcon_001.png"), 0
                );
                m_fields->autoSaveCountdownNotification->show();
            }
            
            // The actual save
            if (m_fields->timeSinceLastAutoSave > interval) {
                m_fields->autoSaveCountdownNotification->setString("Saving...");
                m_fields->autoSaveCountdownNotification->setIcon(NotificationIcon::Loading);
                m_fields->timeSinceLastAutoSave = std::chrono::seconds(0);
                
                // Run on next frame to ensure this one gets rendered first
                Loader::get()->queueInMainThread([this] {
                    // Make sure to stop playtesting!!!!!!
                    if (m_editorLayer->m_playbackMode != PlaybackMode::Not) {
                        this->onStopPlaytest(nullptr);
                    }

                    // Save level (using QuickSave if enabled)
                    auto layer = EditorPauseLayer::create(m_editorLayer);
                    layer->saveLevel();
                    layer->release();

                    // Create backup
                    auto res = Backup::create(m_editorLayer->m_level, true);
                    
                    // Cleanup
                    auto clean = Backup::cleanAutomated(m_editorLayer->m_level);
                    if (!clean) {
                        log::error("Failed to clean up automated backups: {}", clean.unwrapErr());
                    }

                    // Show error / finished
                    if (!res) {
                        log::error("Backing level up failed: {}", res.unwrapErr());
                        m_fields->autoSaveCountdownNotification->setString("Backing up failed!");
                        m_fields->autoSaveCountdownNotification->setIcon(NotificationIcon::Error);
                    }
                    else {
                        m_fields->autoSaveCountdownNotification->setString("Level saved & backed up!");
                        m_fields->autoSaveCountdownNotification->setIcon(NotificationIcon::Success);
                    }

                    // Hide the notification
                    m_fields->autoSaveCountdownNotification->hide();
                    m_fields->autoSaveCountdownNotification = nullptr;
                });
            }
            // Warning countdown
            else {
                m_fields->autoSaveCountdownNotification->setString(
                    fmt::format("Saving in {} seconds", (interval - m_fields->timeSinceLastAutoSave).count())
                );
            }
        }
    }
};
