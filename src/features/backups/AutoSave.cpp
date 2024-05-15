#include <Geode/modify/EditorUI.hpp>
#include "Backup.hpp"

using namespace geode::prelude;

class $modify(AutoSaveUI, EditorUI) {
    struct Fields {
        size_t secondsSinceLastAutoSave = 0;
        Ref<Notification> autoSaveCountdownNotification;
    };

    bool init(LevelEditorLayer* editor) {
        if (!EditorUI::init(editor))
            return false;
        
        if (Mod::get()->template getSettingValue<bool>("auto-save")) {
            this->schedule(schedule_selector(AutoSaveUI::onAutoSaveTick), 1);
        }

        return true;
    }
    void onAutoSaveTick(float) {
        constexpr size_t COUNTDOWN = 5;
        constexpr size_t AUTO_SAVE_INTERVAL = 20;

        m_fields->secondsSinceLastAutoSave += 1;
        if (m_fields->secondsSinceLastAutoSave > AUTO_SAVE_INTERVAL - COUNTDOWN) {
            // Make sure the autosave notification exists
            if (!m_fields->autoSaveCountdownNotification) {
                m_fields->autoSaveCountdownNotification = Notification::create(
                    "", CCSprite::createWithSpriteFrameName("GJ_timeIcon_001.png"), 0
                );
                m_fields->autoSaveCountdownNotification->show();
            }
            
            // The actual save
            if (m_fields->secondsSinceLastAutoSave > AUTO_SAVE_INTERVAL) {
                m_fields->autoSaveCountdownNotification->setString("Saving...");
                m_fields->autoSaveCountdownNotification->setIcon(NotificationIcon::Loading);
                m_fields->secondsSinceLastAutoSave = 0;
                
                // Run on next frame to ensure this one gets rendered first
                Loader::get()->queueInMainThread([this] {
                    // Save level
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
                    fmt::format("Saving in {} seconds", AUTO_SAVE_INTERVAL - m_fields->secondsSinceLastAutoSave)
                );
            }
        }
    }
};
