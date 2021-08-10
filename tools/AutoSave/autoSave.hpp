#pragma once

#include "../../BetterEdit.hpp"
#include "softSave.hpp"

class AutoSaveTimer : public CCNode {
    protected:
        int m_nTimer;
        CCLabelBMFont* m_pLabel;

        bool init();

    public:
        static AutoSaveTimer* create();

        CCLabelBMFont* getLabel();

        int getTimer();
        int getTimerLeft();
        void setTimer(int);
        void resetTimer();
        void incrementTimer();

        void showStatus(const char*);
        void showStatusTime();
};

void loadAutoSaveTimer(EditorUI*);
void resetAutoSaveTimer(EditorUI*);
