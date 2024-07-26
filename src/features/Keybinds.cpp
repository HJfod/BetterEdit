#include <Geode/DefaultInclude.hpp>

// todo: CustomKeybinds on Mac
#ifdef GEODE_IS_WINDOWS

#include <geode.custom-keybinds/include/Keybinds.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <utils/EditCommandExt.hpp>
#include <Geode/binding/EditButtonBar.hpp>
#include <utils/HolyUB.hpp>
#include "GridScaling.hpp"

#include <utils/Pro.hpp>
#ifdef BETTEREDIT_PRO
#include <pro/features/GroupSummaryPopup.hpp>
#endif

using namespace geode::prelude;
using namespace keybinds;

struct $modify(EditorUI) {
    $override
    bool init(LevelEditorLayer* lel) {
        if (!EditorUI::init(lel))
            return false;

        this->defineKeybind("rotate-45-ccw"_spr, [this]() {
            this->transformObjectCall(EditCommand::RotateCCW45);
        });
        this->defineKeybind("rotate-45-cw"_spr, [this]() {
            this->transformObjectCall(EditCommand::RotateCW45);
        });
        this->defineKeybind("rotate-snap"_spr, [this]() {
            this->transformObjectCall(EditCommand::RotateSnap);
        });
        this->defineKeybind("show-scale"_spr, [this]() {
            if(auto scaleBtn = m_editButtonBar->m_buttonArray->objectAtIndex(m_editButtonBar->m_buttonArray->count() - 3)) {
                this->activateScaleControl(scaleBtn);
            }
        });
        this->defineKeybind("toggle-link-controls"_spr, [this]() {
            GameManager::get()->toggleGameVariable("0097");
            m_editorLayer->updateOptions();
        });
        this->defineKeybind("show-ui"_spr, [this]() {
            this->showUI(true);
        });
        this->defineKeybind("hide-ui"_spr, [this]() {
            this->showUI(false);
        });

        this->defineKeybind("open-edit-object"_spr, [this]() {
            this->editObject(nullptr);
        });
        this->defineKeybind("open-edit-group"_spr, [this]() {
            this->editGroup(nullptr);
        });
        this->defineKeybind("open-edit-special"_spr, [this]() {
            this->editObject2(nullptr);
        });
        this->defineKeybind("copy-values"_spr, [this]() {
            this->onCopyState(nullptr);
        });
        this->defineKeybind("paste-state"_spr, [this]() {
            this->onPasteState(nullptr);
        });
        this->defineKeybind("paste-color"_spr, [this]() {
            this->onPasteColor(nullptr);
        });

        this->defineKeybind("enlarge-grid-size"_spr, [this]() {
            incrementGridSize(this);
        });
        this->defineKeybind("ensmallen-grid-size"_spr, [this]() {
            decrementGridSize(this);
        });

        this->defineKeybind("save-level"_spr, [this]() {
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

        this->defineKeybind("build-helper"_spr, [this]() {
            fakeEditorPauseLayer(m_editorLayer)->onBuildHelper(nullptr);
        });
        this->defineKeybind("align-x"_spr, [this]() {
            fakeEditorPauseLayer(m_editorLayer)->onAlignX(nullptr);
        });
        this->defineKeybind("align-y"_spr, [this]() {
            fakeEditorPauseLayer(m_editorLayer)->onAlignY(nullptr);
        });
        this->defineKeybind("select-all"_spr, [this]() {
            fakeEditorPauseLayer(m_editorLayer)->onSelectAll(nullptr);
        });
        this->defineKeybind("select-all-left"_spr, [this]() {
            fakeEditorPauseLayer(m_editorLayer)->onSelectAllLeft(nullptr);
        });
        this->defineKeybind("select-all-right"_spr, [this]() {
            fakeEditorPauseLayer(m_editorLayer)->onSelectAllRight(nullptr);
        });

        this->defineKeybind("move-obj-half-left"_spr, [this] {
            this->moveObjectCall(EditCommand::HalfLeft);
        });
        this->defineKeybind("move-obj-half-right"_spr, [this] {
            this->moveObjectCall(EditCommand::HalfRight);
        });
        this->defineKeybind("move-obj-half-up"_spr, [this] {
            this->moveObjectCall(EditCommand::HalfUp);
        });
        this->defineKeybind("move-obj-half-down"_spr, [this] {
            this->moveObjectCall(EditCommand::HalfDown);
        });
        this->defineKeybind("move-obj-quarter-left"_spr, [this] {
            this->moveObjectCall(EditCommandExt::QuarterLeft);
        });
        this->defineKeybind("move-obj-quarter-right"_spr, [this] {
            this->moveObjectCall(EditCommandExt::QuarterRight);
        });
        this->defineKeybind("move-obj-quarter-up"_spr, [this] {
            this->moveObjectCall(EditCommandExt::QuarterUp);
        });
        this->defineKeybind("move-obj-quarter-down"_spr, [this] {
            this->moveObjectCall(EditCommandExt::QuarterDown);
        });
        this->defineKeybind("move-obj-eighth-left"_spr, [this] {
            this->moveObjectCall(EditCommandExt::EighthLeft);
        });
        this->defineKeybind("move-obj-eighth-right"_spr, [this] {
            this->moveObjectCall(EditCommandExt::EighthRight);
        });
        this->defineKeybind("move-obj-eighth-up"_spr, [this] {
            this->moveObjectCall(EditCommandExt::EighthUp);
        });
        this->defineKeybind("move-obj-eighth-down"_spr, [this] {
            this->moveObjectCall(EditCommandExt::EighthDown);
        });
        this->defineKeybind("move-obj-big-left"_spr, [this] {
            this->moveObjectCall(EditCommand::BigLeft);
        });
        this->defineKeybind("move-obj-big-right"_spr, [this] {
            this->moveObjectCall(EditCommand::BigRight);
        });
        this->defineKeybind("move-obj-big-up"_spr, [this] {
            this->moveObjectCall(EditCommand::BigUp);
        });
        this->defineKeybind("move-obj-big-down"_spr, [this] {
            this->moveObjectCall(EditCommand::BigDown);
        });

        BE_PRO_FEATURE(
            this->defineKeybind("group-summary"_spr, [this] {
                GroupSummaryPopup::create()->show();
            });
        );
        
        return true;
    }

    void defineKeybind(const char* id, std::function<void()> callback) {
        this->template addEventListener<InvokeBindFilter>([=](InvokeBindEvent* event) {
            if (event->isDown()) {
                callback();
            }
            return ListenerResult::Propagate;
        }, id);
    }
};

$execute {
    BindManager::get()->registerBindable(BindableAction(
        "rotate-45-ccw"_spr,
        "Rotate 45 CCW",
        "Rotate the Selected Object(s) 45 Degrees Counter-Clockwise",
        { Keybind::create(KEY_Q, Modifier::Shift) },
        Category::EDITOR_MODIFY
    ));
    BindManager::get()->registerBindable(BindableAction(
        "rotate-45-cw"_spr,
        "Rotate 45 CW",
        "Rotate the Selected Object(s) 45 Degrees Clockwise",
        { Keybind::create(KEY_E, Modifier::Shift) },
        Category::EDITOR_MODIFY
    ));
    BindManager::get()->registerBindable(BindableAction(
        "rotate-snap"_spr,
        "Rotate Snap",
        "Rotate the Selected Object(s) to Match Adjacent Slopes",
        {},
        Category::EDITOR_MODIFY,
        false
    ));
    BindManager::get()->registerBindable(BindableAction(
        "show-scale"_spr,
        "Show Scale Control",
        "Show the object scaling controls",
        {},
        Category::EDITOR_UI,
        false
    ));

    BindManager::get()->registerBindable(BindableAction(
        "save-level"_spr,
        "Save Level",
        "",
        {},
        Category::EDITOR_MODIFY,
        false
    ));
    BindManager::get()->registerBindable(BindableAction(
        "build-helper"_spr,
        "Build Helper",
        "Executes the <cy>Build Helper</c> feature, aka remaps Groud and Color "
        "IDs of the selected objects to unused ones",
        {},
        Category::EDITOR_MODIFY,
        false
    ));
    BindManager::get()->registerBindable(BindableAction(
        "align-x"_spr,
        "Align X",
        "Executes the <cy>Align X</c> feature, aka aligns all of the selected "
        "objects along the X axis",
        {},
        Category::EDITOR_MODIFY,
        false
    ));
    BindManager::get()->registerBindable(BindableAction(
        "align-y"_spr,
        "Align Y",
        "Executes the <cy>Align Y</c> feature, aka aligns all of the selected "
        "objects along the Y axis",
        {},
        Category::EDITOR_MODIFY,
        false
    ));
    BindManager::get()->registerBindable(BindableAction(
        "toggle-link-controls"_spr,
        "Toggle Link Controls",
        "",
        {},
        Category::EDITOR_UI,
        false
    ));

    BindManager::get()->registerBindable(BindableAction(
        "open-edit-object"_spr,
        "Edit Object",
        "Open the <cb>Edit Object</c> popup for the selected objects",
        {},
        Category::EDITOR_MODIFY,
        false
    ));
    BindManager::get()->registerBindable(BindableAction(
        "open-edit-group"_spr,
        "Edit Group",
        "Open the <co>Edit Group</c> popup for the selected objects",
        {},
        Category::EDITOR_MODIFY,
        false
    ));
    BindManager::get()->registerBindable(BindableAction(
        "open-edit-special"_spr,
        "Edit Special",
        "Open the <cj>Edit Special</c> popup for the selected objects",
        {},
        Category::EDITOR_MODIFY,
        false
    ));
    BindManager::get()->registerBindable(BindableAction(
        "copy-values"_spr,
        "Copy Values",
        "",
        {},
        Category::EDITOR_MODIFY,
        false
    ));
    BindManager::get()->registerBindable(BindableAction(
        "paste-state"_spr,
        "Paste State",
        "",
        {},
        Category::EDITOR_MODIFY,
        false
    ));
    BindManager::get()->registerBindable(BindableAction(
        "paste-color"_spr,
        "Paste Color",
        "",
        {},
        Category::EDITOR_MODIFY,
        false
    ));

    BindManager::get()->registerBindable(BindableAction(
        "enlarge-grid-size"_spr,
        "Increase Grid Size",
        "",
        {},
        Category::EDITOR_UI,
        false
    ));
    BindManager::get()->registerBindable(BindableAction(
        "ensmallen-grid-size"_spr,
        "Decrease Grid Size",
        "",
        {},
        Category::EDITOR_UI,
        false
    ));

    BindManager::get()->registerBindable(BindableAction(
        "select-all"_spr,
        "Select All",
        "Select All Object(s)",
        {},
        Category::EDITOR_MODIFY,
        false
    ));
    BindManager::get()->registerBindable(BindableAction(
        "select-all-left"_spr,
        "Select All Left",
        "Select All Object(s) Left of the Screen",
        {},
        Category::EDITOR_MODIFY,
        false
    ));
    BindManager::get()->registerBindable(BindableAction(
        "select-all-right"_spr,
        "Select All Right",
        "Select All Object(s) Right of the Screen",
        {},
        Category::EDITOR_MODIFY,
        false
    ));

    // todo: toggle UI
    BindManager::get()->registerBindable(BindableAction(
        "show-ui"_spr,
        "Show UI",
        "",
        {},
        Category::EDITOR_UI,
        false
    ));
    BindManager::get()->registerBindable(BindableAction(
        "hide-ui"_spr,
        "Hide UI",
        "",
        {},
        Category::EDITOR_UI,
        false
    ));
    BindManager::get()->registerBindable({
        "move-obj-half-left"_spr,
        "Move Object Half Left",
        "Move Selected Object(s) half of a Block left (15 Units)",
        { Keybind::create(KEY_A, Modifier::Control | Modifier::Alt) },
        Category::EDITOR_MOVE, true
    });
    BindManager::get()->registerBindable({
        "move-obj-half-right"_spr,
        "Move Object Half Right",
        "Move Selected Object(s) half of a Block right (15 Units)",
        { Keybind::create(KEY_D, Modifier::Control | Modifier::Alt) },
        Category::EDITOR_MOVE, true
    });
    BindManager::get()->registerBindable({
        "move-obj-half-up"_spr,
        "Move Object Half Up",
        "Move Selected Object(s) half of a Block up (15 Units)",
        { Keybind::create(KEY_W, Modifier::Control | Modifier::Alt) },
        Category::EDITOR_MOVE, true
    });
    BindManager::get()->registerBindable({
        "move-obj-half-down"_spr,
        "Move Object Half Down",
        "Move Selected Object(s) half of a Block down (15 Units)",
        { Keybind::create(KEY_S, Modifier::Control | Modifier::Alt) },
        Category::EDITOR_MOVE, true
    });
    BindManager::get()->registerBindable({
        "move-obj-quarter-left"_spr,
        "Move Object Quarter Left",
        "Move Selected Object(s) quarter of a Block left (7.5 Units)",
        {},
        Category::EDITOR_MOVE, true
    });
    BindManager::get()->registerBindable({
        "move-obj-quarter-right"_spr,
        "Move Object Quarter Right",
        "Move Selected Object(s) quarter of a Block right (7.5 Units)",
        {},
        Category::EDITOR_MOVE, true
    });
    BindManager::get()->registerBindable({
        "move-obj-quarter-up"_spr,
        "Move Object Quarter Up",
        "Move Selected Object(s) quarter of a Block up (7.5 Units)",
        {},
        Category::EDITOR_MOVE, true
    });
    BindManager::get()->registerBindable({
        "move-obj-quarter-down"_spr,
        "Move Object Quarter Down",
        "Move Selected Object(s) quarter of a Block down (7.5 Units)",
        {},
        Category::EDITOR_MOVE, true
    });
    BindManager::get()->registerBindable({
        "move-obj-eighth-left"_spr,
        "Move Object Eighth Left",
        "Move Selected Object(s) eighth of a Block left (3.75 Units)",
        {},
        Category::EDITOR_MOVE, true
    });
    BindManager::get()->registerBindable({
        "move-obj-eighth-right"_spr,
        "Move Object Eighth Right",
        "Move Selected Object(s) eighth of a Block right (3.75 Units)",
        {},
        Category::EDITOR_MOVE, true
    });
    BindManager::get()->registerBindable({
        "move-obj-eighth-up"_spr,
        "Move Object Eighth Up",
        "Move Selected Object(s) eighth of a Block up (3.75 Units)",
        {},
        Category::EDITOR_MOVE, true
    });
    BindManager::get()->registerBindable({
        "move-obj-eighth-down"_spr,
        "Move Object Eighth Down",
        "Move Selected Object(s) eighth of a Block down (3.75 Units)",
        {},
        Category::EDITOR_MOVE, true
    });
    BindManager::get()->registerBindable({
        "move-obj-big-left"_spr,
        "Move Object Big Left",
        "Move Selected Object(s) 5 Blocks left (150 Units)",
        {},
        Category::EDITOR_MOVE, true
    });
    BindManager::get()->registerBindable({
        "move-obj-big-right"_spr,
        "Move Object Big Right",
        "Move Selected Object(s) 5 Blocks right (150 Units)",
        {},
        Category::EDITOR_MOVE, true
    });
    BindManager::get()->registerBindable({
        "move-obj-big-up"_spr,
        "Move Object Big Up",
        "Move Selected Object(s) 5 Blocks up (150 Units)",
        {},
        Category::EDITOR_MOVE, true
    });
    BindManager::get()->registerBindable({
        "move-obj-big-down"_spr,
        "Move Object Big Down",
        "Move Selected Object(s) 5 Blocks down (150 Units)",
        {},
        Category::EDITOR_MOVE, true
    });

    BE_PRO_FEATURE_GLOBAL(
        BindManager::get()->registerBindable({
            "group-summary"_spr,
            "Open Group Summary",
            "",
            {},
            Category::EDITOR, false
        });
    );
}

#endif
