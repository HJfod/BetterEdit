#include <GDMake.h>

using addEditorTabFunc = std::function<gd::EditButtonBar*(gd::EditorUI*)>;
void addEditorTab(const char* spr, addEditorTabFunc bbar);
