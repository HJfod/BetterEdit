#pragma once

#include <vector>
#include <functional>

class BESharedFuncs {
public:
    static std::vector<std::function<void()>> editorExitEvents;

    static void addEditorExitFunc(std::function<void()>);
};

// funny macro
#define BE_EDITOREXIT() \
    void GEODE_CONCAT(editorExitEvent, __LINE__)(); \
    $execute { BESharedFuncs::addEditorExitFunc([] { GEODE_CONCAT(editorExitEvent, __LINE__)(); }); } \
    void GEODE_CONCAT(editorExitEvent, __LINE__)()