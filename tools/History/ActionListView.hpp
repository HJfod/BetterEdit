#pragma once

#include "UndoHistoryManager.hpp"

static constexpr const BoomListType kBoomListType_Action
    = static_cast<BoomListType>(0x425);
static constexpr const BoomListType kBoomListType_UndoObject
    = static_cast<BoomListType>(0x435);

class ActionCell : public TableViewCell {
    protected:
        ActionObject* m_pAction;
        UndoObject* m_pUndoObject;
    
		ActionCell(const char* name, CCSize size);

        void draw() override;
	
	public:
        void loadFromAction(ActionObject* action);
        void loadFromUndoObject(UndoObject* obj);

		static ActionCell* create(const char* key, CCSize size);
};

class ActionListView : public CustomListView {
    protected:
        void setupList() override;
        TableViewCell* getListCell(const char* key) override;
        void loadCell(TableViewCell* cell, unsigned int index) override;
    
    public:
        static ActionListView* create(
            CCArray* actions,
            BoomListType type,
            float width, float height
        );
};
