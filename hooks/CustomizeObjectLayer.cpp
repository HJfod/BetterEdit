#include "../BetterEdit.hpp"
#include <InputPrompt.hpp>

std::string g_nextFreeColorInput = "";
static constexpr const int NEXTFREE_TAG = 5000;

class SetGroupIDLayer_CB : public CustomizeObjectLayer {
    public:
        void onCustomNextFree(CCObject* pSender) {
            auto p = InputPrompt::create("Next Free Offset", "ID", [this, pSender](const char* txt) -> void {
                if (txt && strlen(txt)) {
                    auto startID = 0;
                    try { startID = std::stoi(txt); } catch (...) {}

                    auto bytes = intToBytes(startID);
                    bytes.insert(bytes.begin(), 0xbf);

                    patchBytes(0x164fa5, bytes);
                    this->onNextColorChannel(pSender);

                    g_nextFreeColorInput = txt;
                } else {
                    g_nextFreeColorInput = "";
                    patchBytes(0x164fa5, { 0xbf, 0x01, 0x00, 0x00, 0x00 });
                }

                auto btn = as<CCMenuItemSpriteExtra*>(this->m_pButtonMenu->getChildByTag(NEXTFREE_TAG));

                if (btn) {
                    auto spr = as<ButtonSprite*>(btn->getNormalImage());
                    spr->setString(g_nextFreeColorInput.size() ? g_nextFreeColorInput.c_str() : "0");
                    spr->updateBGImage(
                        g_nextFreeColorInput.size() ? "GJ_button_02.png" : "GJ_button_04.png"
                    );
                }
            }, "Apply")->setApplyOnEsc(true)->setTrashButton(true);
            p->getInputNode()->getInputNode()->setAllowedChars("0123456789");
            p->getInputNode()->getInputNode()->setMaxLabelLength(6);
            p->getInputNode()->setString(g_nextFreeColorInput.c_str());
            p->show();
        }
};

GDMAKE_HOOK(0x53e00)
bool __fastcall CustomizeObjectLayer_init(CustomizeObjectLayer* self, edx_t edx, EffectGameObject* obj, CCArray* objs) {
    if (!GDMAKE_ORIG(self, edx, obj, objs))
        return false;
    
    auto nextFreeBtn = getChild<CCMenuItemSpriteExtra*>(self->m_pButtonMenu, 26);

    auto customNextFreeBtn = CCMenuItemSpriteExtra::create(
        ButtonSprite::create(
            g_nextFreeColorInput.size() ? g_nextFreeColorInput.c_str() : "0", 20, true, "goldFont.fnt", 
            g_nextFreeColorInput.size() ? "GJ_button_02.png" : "GJ_button_04.png", 25, .6f
        ),
        self,
        (SEL_MenuHandler)&SetGroupIDLayer_CB::onCustomNextFree
    );
    customNextFreeBtn->setTag(NEXTFREE_TAG);
    customNextFreeBtn->setPosition(
        nextFreeBtn->getPositionX() + 58.0f,
        nextFreeBtn->getPositionY()
    );
    self->m_pButtonMenu->addChild(customNextFreeBtn);

    return true;
}
