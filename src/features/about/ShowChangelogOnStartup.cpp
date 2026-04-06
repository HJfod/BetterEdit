
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include "ChangelogPopup.hpp"

using namespace geode::prelude;

class $modify(LevelEditorLayer) {
    $override
    bool init(GJGameLevel* level, bool idk) {
        if (!LevelEditorLayer::init(level, idk))
            return false;

        // We don't really need to check for updates on startup anymore, since 
        // Geode is now really good at it by itself

        if (Mod::get()->getSettingValue<bool>("enable-changelog-popup")) {
            // Check if new updates have been installed
            auto lastShown = Mod::get()->getSavedValue<VersionInfo>("last-shown-changelog");
            if (lastShown < Mod::get()->getVersion()) {
                // Create the popup (two) frame(s) later to hopefully avoid touch issues
                Loader::get()->queueInMainThread([this, lastShown] {
                    Loader::get()->queueInMainThread([this, lastShown] {
                        auto popup = ChangelogPopup::create(lastShown);
                        popup->m_scene = this;
                        popup->show();
                    });
                });
            }
        }

        // Save that we have shown this version so the same changelog isn't shown multiple times
        Mod::get()->setSavedValue("last-shown-changelog", Mod::get()->getVersion());

        return true;
    }
};
