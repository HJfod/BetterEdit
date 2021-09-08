#pragma once

#include "KeybindManager.hpp"
#include "KeybindingsLayer.hpp"
#include <BrownAlertDelegate.hpp>
#include "../../utils/BGLabel.hpp"

class KeymapLayer : public BrownAlertDelegate, public FLAlertLayerProtocol {
    public:
        enum Layout {
            kLayoutQWERTY,
            kLayoutQWERTZ,
        };

    protected:
        struct keybtn {
            enumKeyCodes key;
            std::string text = "";
            bool sprite = false;
            CCMenuItemToggler** target = nullptr;
            int width;
            std::vector<keybtn> sub;

            inline keybtn(float w) {
                key = KEY_None;
                width = static_cast<int>(s_fItemWidth * w);
                text = "";
            }

            inline keybtn(enumKeyCodes k) {
                key = k;
                width = s_fItemWidth;
                text = getShortenedKeyName(k);
            }

            inline keybtn(enumKeyCodes k, CCMenuItemToggler** b) {
                key = k;
                width = s_fItemWidth;
                text = getShortenedKeyName(k);
                target = b;
            }

            inline keybtn(enumKeyCodes k, float w, CCMenuItemToggler** b) {
                key = k;
                width = static_cast<int>(s_fItemWidth * w);
                text = getShortenedKeyName(k);
                target = b;
            }

            inline keybtn(std::string const& t, enumKeyCodes k) {
                key = k;
                sprite = true;
                text = t;
                width = s_fItemWidth;
            }

            inline keybtn(enumKeyCodes k, std::string const& t) {
                key = k;
                text = t;
                width = s_fItemWidth;
            }

            inline keybtn(enumKeyCodes k, float w) {
                key = k;
                width = static_cast<int>(s_fItemWidth * w);
                text = getShortenedKeyName(k);
            }

            inline keybtn(std::string const& keys) {
                key = KEY_None;

                for (auto const& key : keys)
                    sub.push_back({ static_cast<enumKeyCodes>(key) });
            }

            inline keybtn(int keys, std::string const& prefix, enumKeyCodes start) {
                key = KEY_None;

                for (auto ix = 0; ix < keys; ix++)
                    sub.push_back({
                        static_cast<enumKeyCodes>(start + ix),
                        prefix + std::to_string(ix + 1)
                    });
            }
        };

        static constexpr int s_fItemWidth = 12;
        static constexpr int s_fItemSeparation = 10;
        static constexpr ccColor3B m_colNormal = { 255, 255, 255 };
        static constexpr ccColor3B m_colToggleable = m_colNormal;
        static constexpr ccColor3B m_colToggled = m_colNormal;
        static constexpr ccColor3B m_colInUseEditor = { 0, 255, 50 };
        static constexpr ccColor3B m_colInUsePlay = { 255, 255, 80  };
        static constexpr ccColor3B m_colInUseGlobal = { 205, 50, 255 };
        static constexpr ccColor3B m_colInUse = { 50, 90, 255 };
        static constexpr ccColor3B m_colConflict = { 255, 80, 20 };
        static constexpr ccColor3B m_colText = { 255, 255, 255 };

        Layout m_eLayout;
        std::vector<std::vector<keybtn>> m_mKeymap;
        std::vector<ButtonSprite*> m_vKeyBtns;
        bool m_bControlDown = false;
        bool m_bShiftDown = false;
        bool m_bAltDown = false;
        bool m_bUpdateOnNextVisit = false;
        CCMenuItemToggler* m_pControlToggle = nullptr;
        CCMenuItemToggler* m_pShiftToggle = nullptr;
        CCMenuItemToggler* m_pAltToggle = nullptr;
        TextArea* m_pCallbackLabel;
        Keybind m_obHovered;
        std::set<enumKeyCodes> m_ePressed;
        std::string m_sLabelString = "";
        ButtonSprite* m_pSelected;
        std::vector<CCMenuItemSpriteExtra*> m_pBindBtns;
        KeybindingsLayer_CB* m_pKeybindingsLayer;
        BGLabel* m_pSelectedKeybindLabel;
        CCLabelBMFont* m_pSelectedLayoutLabel;
        int m_nSelectedLayout = 0;
        std::vector<Layout> m_vLayouts;

        void FLAlert_Clicked(FLAlertLayer*, bool);

        void setup() override;
        CCMenuItemSpriteExtra* getKeyButton(const char*, int, int, CCPoint const&, bool = false);
        CCMenuItemToggler* getKeyToggle(const char*, int, CCMenuItemToggler**, SEL_MenuHandler, CCPoint const&);
        void updateKey(ButtonSprite*);

        void keyDown(enumKeyCodes) override;
        void keyUp(enumKeyCodes) override;
        void visit() override;

        Keybind getKeybind(enumKeyCodes = KEY_None);

        void onToggle(CCObject*);
        void onSelect(CCObject*);
        void onSetBind(CCObject*);
        void onUnbind(CCObject*);
        void onSwitchLayout(CCObject*);

    public:
        void updateKeys();
        void showHoveredKey();
        void loadKeybind(Keybind const&);
        void loadLayout(Layout);

        static std::string layoutToString(Layout);
        static std::string getShortenedKeyName(enumKeyCodes);
    
        static KeymapLayer* create(KeybindingsLayer_CB*);
};
