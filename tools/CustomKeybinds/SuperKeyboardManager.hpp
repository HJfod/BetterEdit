#pragma once

#include "../../BetterEdit.hpp"

struct SuperKeyboardDelegate {
    virtual bool keyDownSuper(enumKeyCodes);
    virtual bool keyUpSuper(enumKeyCodes);

    SuperKeyboardDelegate();
    virtual ~SuperKeyboardDelegate();

    protected:
        void superKeyPushSelf();
        void superKeyPopSelf();
};

class SuperKeyboardManager {
    protected:
        std::vector<SuperKeyboardDelegate*> m_vDelegates;

        bool init();
    
    public:
        static bool initGlobal();
        static SuperKeyboardManager* get();

        void pushDelegate(SuperKeyboardDelegate*);
        void popDelegate(SuperKeyboardDelegate*);

        bool dispatchEvent(enumKeyCodes, bool keydown);
};
