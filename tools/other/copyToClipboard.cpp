#include "../../BetterEdit.hpp"

bool verifyObjectString(std::string const& str) {
    std::vector<std::string> objs;
    if (str.find(";") != std::string::npos) {
        objs = stringSplit(str, ";");
    } else {
        objs = { str };
    }

    if (!objs.size())
        return false;

    for (auto const& obj : objs) {
        if (
            obj.size() > 2 &&
            obj.find_first_not_of(' ') != std::string::npos &&
            obj.find(",") == std::string::npos
        )
            return false;

        auto kv = stringSplit(obj, ",");
        for (auto ix = 0u; ix < kv.size(); ix += 2u) {
            auto key = kv[ix];
            if (
                !key.size() ||
                key.find_first_not_of(' ') != std::string::npos
            )
                return false;

            try {
                std::stoi(key);
            } catch (...) {
                return false;
            }
        }
    }

    return true;
}

GDMAKE_HOOK(0x87fb0)
void __fastcall EditorUI_onCopy(EditorUI* self, edx_t edx, CCObject* pSender) {
    GDMAKE_ORIG_V(self, edx, pSender);

    if (BetterEdit::getCopyObjectsToClipboard()) {
        copyToWin32Clipboard(self->m_sClipboard);
    }
}

GDMAKE_HOOK(0x880c0)
void __fastcall EditorUI_onPaste(EditorUI* self, edx_t edx, CCObject* pSender) {
    if (BetterEdit::getCopyObjectsToClipboard()) {
        auto str = readWin32Clipboard();

        if (str.size()) {
            auto orig = self->m_sClipboard;
            self->m_sClipboard = str;
            GDMAKE_ORIG_V(self, edx, pSender);
            self->m_sClipboard = orig;
            return;
        }
    }
    GDMAKE_ORIG_V(self, edx, pSender);
}
