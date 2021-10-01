#pragma once

#include "IntegratedConsole.hpp"

static constexpr const BoomListType kBoomListType_Debug
    = static_cast<BoomListType>(0x428);

class DebugCell : public TableViewCell {
    protected:
        CCLabelBMFont* m_pLabel;

		DebugCell(const char* name, CCSize size);

        void draw() override;
	
	public:
        void loadFromString(CCString*);
        void updateBGColor(int);
        void updateLabelColor();

		static DebugCell* create(const char* key, CCSize size);
};

class DebugListView : public CustomListView {
    protected:
        void setupList() override;
        TableViewCell* getListCell(const char* key) override;
        void loadCell(TableViewCell* cell, unsigned int index) override;
    
    public:
        static DebugListView* create(CCArray* actions, float width, float height);
};
