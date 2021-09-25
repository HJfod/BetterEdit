#include "loadContextMenu.hpp"
#include "ContextMenu.hpp"

void loadContextMenu() {
    ContextMenu::load();
}

void updateContextMenu(LevelEditorLayer* lel) {
    CATCH_NULL(ContextMenu::get(lel))->updateItems();
}
