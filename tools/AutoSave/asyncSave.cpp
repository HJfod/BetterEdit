#include "asyncSave.hpp"
#include "../RotateSaws/rotateSaws.hpp"
#include <thread>
#include <mutex>

std::mutex g_stringMutex;
static std::string g_sharedString;

GDMAKE_HOOK(0x162480)
std::string* __fastcall LevelEditorLayer_getLevelString(
    LevelEditorLayer* self, edx_t edx, std::string* resStr
) {
    if (!BetterEdit::getEnableAsyncSave())
        return GDMAKE_ORIG_P(self, edx, resStr);

    self->m_bHighDetail = false;

    g_sharedString = self->m_pLevelSettings->getSaveString() + ";";

    std::vector<std::thread> threads;

    CCARRAY_FOREACH_B_TYPE(self->m_pObjectContainerArrays, array, CCArray) {
        CCARRAY_FOREACH_B_TYPE(array, obj, GameObject) {
            if (!self->m_bHighDetail)
                self->m_bHighDetail = obj->m_bHighDetail;
            
            // structured bindings cant be captured sooo
            auto obj_ = obj;
            
            threads.push_back(std::thread ([obj_]() -> void {
                auto res = obj_->getSaveString();

                g_stringMutex.lock();

                // g_sharedString += res + ";";

                g_stringMutex.unlock();
            }));
        }
    }

    for (auto & t : threads)
        t.join();
    
    *resStr = g_sharedString;

    return resStr;
}

GDMAKE_HOOK(0x75010)
void __fastcall EditorPauseLayer_saveLevel(EditorPauseLayer* self) {
    stopRotations(self->m_pEditorLayer);

    GDMAKE_ORIG_V(self);

    SoftSaveManager::clear();
}

