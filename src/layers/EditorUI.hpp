#pragma once

#include "../offsets.hpp"

class EditorUI : public cocos2d::CCLayer {
    protected:
        PAD(0xD0)
        cocos2d::CCArray* selectedObjs;

        static inline bool (__thiscall* init)(EditorUI*, uintptr_t);
        static bool __fastcall initHook(EditorUI*, uintptr_t, uintptr_t);

        void onShowTools(cocos2d::CCObject*);

    public:
        inline static cocos2d::CCArray* (__thiscall* pasteObjectsFromString)(EditorUI*, std::string);

        void deselect(gd::GameObject* _obj) {
            // _obj->select(false);

            std::cout << "ye0\n";

            reinterpret_cast<void(__fastcall*)(
                gd::GameObject*
            )>(
                gd::base + 0xeee50
            )(
                _obj
            );

            std::cout << "ye1\n";

            cocos2d::CCArray* objs = as<cocos2d::CCArray*>(
                as<uintptr_t>(this) + 0xD0
            );

            std::cout << "ye2\n";

            objs->removeLastObject(true);

            std::cout << "ye3\n";
        };

        static EditorUI* sharedUI() {
            return as<EditorUI*>(
                gd::GameManager::sharedState()->getEditorLayer()->getEditorUI()
            );
        }

        static MH_STATUS load() {
            EditorUI::pasteObjectsFromString =
                as<decltype(pasteObjectsFromString)>(gd::base + 0x88240);

            return MH_CreateHook(
                (PVOID)(gd::base + 0x76310),
                as<LPVOID>(EditorUI::initHook),
                as<LPVOID*>(&EditorUI::init)
            );
        }
};
