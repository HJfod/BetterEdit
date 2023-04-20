#include <hjfod.custom-keybinds/include/Keybinds.hpp>
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
}
