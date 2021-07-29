#include "loadEnterSearch.hpp"

// haha this is all inevitably pointless
// as the keyDown is not called when the focus
// is on the alert layer

class SetTextPopup : public FLAlertLayer, TextInputDelegate {
    public:
        void onClose(CCObject* pSender) {
            reinterpret_cast<void(__thiscall*)(SetTextPopup*, CCObject*)>(
                base + 0x145000
            )(this, pSender);
        }
};

void __fastcall SetTextPopup_keyDown(SetTextPopup* self_0xf8, edx_t edx, enumKeyCodes key) {
    auto self = as<SetTextPopup*>(as<uintptr_t>(self_0xf8) - 0xf8);

    if (key == KEY_Enter)
        return self->onClose(nullptr);

    return self->FLAlertLayer::keyDown(key);
}

void loadEnterSearch() {
    patch(0x2ab8b8, intToBytes(as<uintptr_t>(&SetTextPopup_keyDown)), true);
}

void unloadEnterSearch() {
    unpatch(0x2ab8b8, true);
}
