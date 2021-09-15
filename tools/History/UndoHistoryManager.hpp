#pragma once

#include "Action.hpp"

class UndoHistoryManager {
    protected:
        CCArray* m_pActions;

        bool init();

        ~UndoHistoryManager();

    public:
        void addAction(ActionObject* action);
        void removeAction(ActionObject* id);

        CCArray* getActionHistory();

        static bool initGlobal();
        static UndoHistoryManager* get();
};
