#pragma once

#include "IntegratedConsole.hpp"

static constexpr const BoomListType kBoomListType_Debug
    = static_cast<BoomListType>(0x428);

class DebugObject : public CCObject {
    public:
        std::string m_sString;
        DebugType m_eDebugType;

        inline DebugObject(DebugMsg msg) {
            this->m_sString = msg.str;
            this->m_eDebugType = msg.type;
            this->autorelease();
        }
};

class DebugCell : public TableViewCell {
    protected:
        CCLabelBMFont* m_pLabel;

		DebugCell(const char* name, CCSize size);

        void draw() override;
	
	public:
        void loadFromObject(DebugObject*);
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
