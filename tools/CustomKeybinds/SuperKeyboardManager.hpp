#pragma once

#include "../../BetterEdit.hpp"

struct SuperKeyboardDelegate {
    virtual void keyDownSuper(enumKeyCodes);
    virtual void keyUpSuper(enumKeyCodes);

    SuperKeyboardDelegate();
    virtual ~SuperKeyboardDelegate();
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

        void dispatchEvent(enumKeyCodes, bool keydown);
};
