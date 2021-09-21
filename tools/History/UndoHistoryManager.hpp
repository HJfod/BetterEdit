#pragma once

#include "Action.hpp"

class UndoHistoryManager {
    protected:
        CCArray* m_pActions;

        bool init();

        virtual ~UndoHistoryManager();

    public:
        void addAction(ActionObject* action);
        void removeAction(ActionObject* id);

        CCArray* getActionHistory();
        void clearHistory();

        static bool initGlobal();
        static UndoHistoryManager* get();
};
