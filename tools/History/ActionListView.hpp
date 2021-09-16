#pragma once

#include "UndoHistoryManager.hpp"

static constexpr const BoomListType kBoomListType_Action
    = static_cast<BoomListType>(0x425);

class ActionCell : public TableViewCell {
    protected:
        ActionObject* m_pAction;
    
		ActionCell(const char* name, CCSize size);

        void draw() override;
	
	public:
        void loadFromAction(ActionObject* action);

		static ActionCell* create(const char* key, CCSize size);
};

class ActionListView : public CustomListView {
    protected:
        void setupList() override;
        TableViewCell* getListCell(const char* key) override;
        void loadCell(TableViewCell* cell, unsigned int index) override;
    
    public:
        static ActionListView* create(CCArray* actions, float width, float height);
};
