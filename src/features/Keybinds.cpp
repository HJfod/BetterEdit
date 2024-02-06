#include <geode.custom-keybinds/include/Keybinds.hpp>
#include <Geode/modify/EditorUI.hpp>

using namespace geode::prelude;
using namespace keybinds;

struct $modify(EditorUI) {
    bool init(LevelEditorLayer* lel) {
        if (!EditorUI::init(lel))
            return false;

        this->defineKeybind("rotate-45-ccw"_spr, [=]() {
            this->transformObjectCall(EditCommand::RotateCCW45);
        });
        this->defineKeybind("rotate-45-cw"_spr, [=]() {
            this->transformObjectCall(EditCommand::RotateCW45);
        });
        this->defineKeybind("rotate-snap"_spr, [=]() {
            this->transformObjectCall(EditCommand::RotateSnap);
        });
        this->defineKeybind("show-scale"_spr, [=]() {
            this->activateScaleControl(nullptr);
        });
        this->defineKeybind("show-ui"_spr, [=]() {
            this->showUI(true);
        });
        this->defineKeybind("hide-ui"_spr, [=]() {
            this->showUI(false);
        });
        this->defineKeybind("select-all"_spr, [=]() {
            this->selectAll();
        });
        this->defineKeybind("select-all-left"_spr, [=]() {
            this->selectAllWithDirection(true);
        });
        this->defineKeybind("select-all-right"_spr, [=]() {
            this->selectAllWithDirection(false);
        });
        this->defineKeybind("move-obj-half-left"_spr, [=] {
            this->moveObjectCall(EditCommand::HalfLeft);
        });
        this->defineKeybind("move-obj-half-right"_spr, [=] {
            this->moveObjectCall(EditCommand::HalfRight);
        });
        this->defineKeybind("move-obj-half-up"_spr, [=] {
            this->moveObjectCall(EditCommand::HalfUp);
        });
        this->defineKeybind("move-obj-half-down"_spr, [=] {
            this->moveObjectCall(EditCommand::HalfDown);
        });
        this->defineKeybind("move-obj-big-left"_spr, [=] {
            this->moveObjectCall(EditCommand::BigLeft);
        });
        this->defineKeybind("move-obj-big-right"_spr, [=] {
            this->moveObjectCall(EditCommand::BigRight);
        });
        this->defineKeybind("move-obj-big-up"_spr, [=] {
            this->moveObjectCall(EditCommand::BigUp);
        });
        this->defineKeybind("move-obj-big-down"_spr, [=] {
            this->moveObjectCall(EditCommand::BigDown);
        });
        
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
        Category::EDITOR_MODIFY
    ));

    BindManager::get()->registerBindable(BindableAction(
        "select-all"_spr,
        "Select All",
        "Select All Object(s)",
        {},
        Category::EDITOR_MODIFY
    ));
    BindManager::get()->registerBindable(BindableAction(
        "select-all-left"_spr,
        "Select All Left",
        "Select All Object(s) Left of the Screen",
        {},
        Category::EDITOR_MODIFY
    ));
    BindManager::get()->registerBindable(BindableAction(
        "select-all-right"_spr,
        "Select All Right",
        "Select All Object(s) Right of the Screen",
        {},
        Category::EDITOR_MODIFY
    ));

    BindManager::get()->registerBindable(BindableAction(
        "show-scale"_spr,
        "Show Scale Control",
        "",
        {},
        Category::EDITOR_UI
    ));
    // todo: toggle UI
    BindManager::get()->registerBindable(BindableAction(
        "show-ui"_spr,
        "Show UI",
        "",
        {},
        Category::EDITOR_UI
    ));
    BindManager::get()->registerBindable(BindableAction(
        "hide-ui"_spr,
        "Hide UI",
        "",
        {},
        Category::EDITOR_UI
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
}
