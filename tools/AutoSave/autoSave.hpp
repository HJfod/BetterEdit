#pragma once

#include "../../BetterEdit.hpp"
#include "softSave.hpp"

class AutoSaveTimer : public CCNode {
    protected:
        int m_nTimer;
        bool m_bPaused = false;
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
        void cancel();
        void pause();
        void resume();
        bool paused();
        bool cancellable();

        void showStatus(const char*);
        void showStatusTime();
};

void loadAutoSaveTimer(EditorUI*);
void resetAutoSaveTimer(EditorUI*);
AutoSaveTimer* getAutoSaveTimer(EditorUI*);
