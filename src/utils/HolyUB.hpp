#pragma once

#include <Geode/binding/EditorPauseLayer.hpp>

using namespace geode::prelude;

struct FakeEditorPauseLayer final {
    char m_alloc[sizeof(EditorPauseLayer)];
    EditorPauseLayer* operator->() {
        return reinterpret_cast<EditorPauseLayer*>(&m_alloc);
    }
};

// Windows 2.206 inlines a lot of editor update funcs to EditorPauseLayer callbacks
// This type puns a LevelEditorLayer to an "EditorPauseLayer" that can be used to call 
// such a callback (since all those callbacks do is just deref the editorLayer member 
// and call an inlined function on it)
static FakeEditorPauseLayer fakeEditorPauseLayer(LevelEditorLayer* lel) {
    auto epl = FakeEditorPauseLayer();
    epl->m_editorLayer = lel;
    return epl;
}
