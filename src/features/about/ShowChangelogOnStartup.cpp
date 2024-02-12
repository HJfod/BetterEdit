
#include <Geode/modify/EditorUI.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include "ChangelogPopup.hpp"

using namespace geode::prelude;

class $modify(EditorUI) {
    $override
    bool init(LevelEditorLayer* lel) {
        if (!EditorUI::init(lel))
            return false;

        if (auto available = Mod::get()->hasAvailableUpdate()) {
            auto popup = createQuickPopup(
                "Update Available!",
                fmt::format(
                    "<cy>BetterEdit</c> has a new update <ca>{}</c> available!\n\n"
                    "Visit the <cb>Geode mods list</c> to install it",
                    available.value()
                ),
                "OK", "Update",
                [](auto, bool btn2) {
                    if (btn2) {
                        openInfoPopup(Mod::get());
                    }
                },
                false
            );
            popup->m_scene = this;
            popup->show();
        }
        
        // Check if new updates have been installed
        auto lastShown = Mod::get()->template getSavedValue<VersionInfo>("last-shown-changelog");
        if (lastShown < Mod::get()->getVersion()) {
            auto popup = ChangelogPopup::create(lastShown);
            popup->m_scene = this;
            popup->show();
        }

        // Save that we have shown this version so the same changelog isn't shown multiple times
        Mod::get()->setSavedValue("last-shown-changelog", Mod::get()->getVersion());

        return true;
    }
};
