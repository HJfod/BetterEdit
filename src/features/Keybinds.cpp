#include <Geode/DefaultInclude.hpp>

#ifdef GEODE_IS_DESKTOP

#include <Geode/modify/EditorUI.hpp>
#include <Geode/binding/EditButtonBar.hpp>
#include <Geode/binding/GameManager.hpp>
#include <Geode/ui/Notification.hpp>
#include <utils/Editor.hpp>
#include <utils/HolyUB.hpp>
#include "GridScaling.hpp"
#include <features/GroupSummaryPopup.hpp>

using namespace geode::prelude;

struct $modify(EditorUI) {
    $override
    bool init(LevelEditorLayer* lel) {
        if (!EditorUI::init(lel))
            return false;

        this->defineKeybind("keybind-rotate-45-ccw", [this]() {
            this->transformObjectCall(EditCommand::RotateCCW45);
        });
        this->defineKeybind("keybind-rotate-45-cw", [this]() {
            this->transformObjectCall(EditCommand::RotateCW45);
        });
        this->defineKeybind("keybind-rotate-snap", [this]() {
            this->transformObjectCall(EditCommand::RotateSnap);
        });
        this->defineKeybind("keybind-show-scale", [this]() {
            if (auto scaleBtn = this->querySelector("scale-button")) {
                this->activateScaleControl(scaleBtn);
            }
        });
        this->defineKeybind("keybind-show-scale-xy", [this]() {
            if (auto scaleBtn = this->querySelector("scale-xy-button")) {
                this->activateScaleControl(scaleBtn);
            }
        });
        this->defineKeybind("keybind-show-warp", [this]() {
            if (auto btn = this->querySelector("warp-button")) {
                this->activateTransformControl(btn);
            }
        });
        this->defineKeybind("keybind-toggle-link-controls", [this]() {
            GameManager::get()->toggleGameVariable("0097");
            m_editorLayer->updateOptions();
        });
        this->defineKeybind("keybind-show-ui", [this]() {
            this->showUI(true);
        });
        this->defineKeybind("keybind-hide-ui", [this]() {
            this->showUI(false);
        });

        this->defineKeybind("keybind-open-edit-object", [this]() {
            this->editObject(nullptr);
        });
        this->defineKeybind("keybind-open-edit-group", [this]() {
            this->editGroup(nullptr);
        });
        this->defineKeybind("keybind-open-edit-special", [this]() {
            this->editObjectSpecial(0);
        });
        this->defineKeybind("keybind-copy-values", [this]() {
            this->onCopyState(nullptr);
        });
        this->defineKeybind("keybind-paste-state", [this]() {
            this->onPasteState(nullptr);
        });
        this->defineKeybind("keybind-paste-color", [this]() {
            this->onPasteColor(nullptr);
        });

        this->defineKeybind("keybind-enlarge-grid-size", [this]() {
            incrementGridSize(this);
        });
        this->defineKeybind("keybind-ensmallen-grid-size", [this]() {
            decrementGridSize(this);
        });

        this->defineKeybind("keybind-save-level", [this]() {
            // Prevent spamming Ctrl + S
            static std::chrono::time_point<std::chrono::system_clock> LAST_USED = std::chrono::system_clock::now();
            auto lastUsed = LAST_USED;
            auto now = LAST_USED = std::chrono::system_clock::now();
            if (now - lastUsed < std::chrono::seconds(2)) {
                return;
            }
            
            if (m_editorLayer->m_playbackMode != PlaybackMode::Not) {
                this->onStopPlaytest(nullptr);
            }
            fakeEditorPauseLayer(m_editorLayer)->saveLevel();
            Notification::create("Level saved", NotificationIcon::Success)->show();
        });
        this->defineKeybind("keybind-pause-resume-playtest", [this]() {
            // LevelEditorLayer::onPausePlaytest is inlined at least on Windows 
            // but this does the job better probably anyway sooo
            if (m_editorLayer->m_playbackMode != PlaybackMode::Not) {
                this->onPlaytest(nullptr);
            }
        });

        this->defineKeybind("keybind-build-helper", [this]() {
            fakeEditorPauseLayer(m_editorLayer)->onBuildHelper(nullptr);
        });
        this->defineKeybind("keybind-create-loop", [this]() {
            fakeEditorPauseLayer(m_editorLayer)->onCreateLoop(nullptr);
        });
        this->defineKeybind("keybind-align-x", [this]() {
            fakeEditorPauseLayer(m_editorLayer)->onAlignX(nullptr);
        });
        this->defineKeybind("keybind-align-y", [this]() {
            fakeEditorPauseLayer(m_editorLayer)->onAlignY(nullptr);
        });
        this->defineKeybind("keybind-select-all", [this]() {
            fakeEditorPauseLayer(m_editorLayer)->onSelectAll(nullptr);
        });
        this->defineKeybind("keybind-select-all-left", [this]() {
            fakeEditorPauseLayer(m_editorLayer)->onSelectAllLeft(nullptr);
        });
        this->defineKeybind("keybind-select-all-right", [this]() {
            fakeEditorPauseLayer(m_editorLayer)->onSelectAllRight(nullptr);
        });

        this->defineKeybind("keybind-move-obj-half-left", [this] {
            this->moveObjectCall(EditCommand::HalfLeft);
        });
        this->defineKeybind("keybind-move-obj-half-right", [this] {
            this->moveObjectCall(EditCommand::HalfRight);
        });
        this->defineKeybind("keybind-move-obj-half-up", [this] {
            this->moveObjectCall(EditCommand::HalfUp);
        });
        this->defineKeybind("keybind-move-obj-half-down", [this] {
            this->moveObjectCall(EditCommand::HalfDown);
        });
        this->defineKeybind("keybind-move-obj-quarter-left", [this] {
            this->moveObjectCall(EditCommandExt::QuarterLeft);
        });
        this->defineKeybind("keybind-move-obj-quarter-right", [this] {
            this->moveObjectCall(EditCommandExt::QuarterRight);
        });
        this->defineKeybind("keybind-move-obj-quarter-up", [this] {
            this->moveObjectCall(EditCommandExt::QuarterUp);
        });
        this->defineKeybind("keybind-move-obj-quarter-down", [this] {
            this->moveObjectCall(EditCommandExt::QuarterDown);
        });
        this->defineKeybind("keybind-move-obj-eighth-left", [this] {
            this->moveObjectCall(EditCommandExt::EighthLeft);
        });
        this->defineKeybind("keybind-move-obj-eighth-right", [this] {
            this->moveObjectCall(EditCommandExt::EighthRight);
        });
        this->defineKeybind("keybind-move-obj-eighth-up", [this] {
            this->moveObjectCall(EditCommandExt::EighthUp);
        });
        this->defineKeybind("keybind-move-obj-eighth-down", [this] {
            this->moveObjectCall(EditCommandExt::EighthDown);
        });
        this->defineKeybind("keybind-move-obj-big-left", [this] {
            this->moveObjectCall(EditCommand::BigLeft);
        });
        this->defineKeybind("keybind-move-obj-big-right", [this] {
            this->moveObjectCall(EditCommand::BigRight);
        });
        this->defineKeybind("keybind-move-obj-big-up", [this] {
            this->moveObjectCall(EditCommand::BigUp);
        });
        this->defineKeybind("keybind-move-obj-big-down", [this] {
            this->moveObjectCall(EditCommand::BigDown);
        });

        this->defineKeybind("keybind-group-summary", [this] {
            GroupSummaryPopup::create(this)->show();
        });

        return true;
    }

    void defineKeybind(const char* key, std::function<void()> callback) {
        this->addEventListener(
            KeybindSettingPressedEventV3(Mod::get(), key),
            [=](Keybind const&, bool down, bool, double) {
                if (down) {
                    callback();
                }
            }
        );
    }
};

#endif
