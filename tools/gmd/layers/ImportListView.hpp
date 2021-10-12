#pragma once

#include "../../../BetterEdit.hpp"
#include "ImportObject.hpp"

static constexpr const BoomListType kBoomListType_Import
    = static_cast<BoomListType>(0x42c);

class ImportListView;
class ImportLayer;

class ImportLevelCell : public TableViewCell {
    protected:
        ImportListView* m_pList;
        ImportObject* m_pObject;
        bool m_bSelected = false;

		ImportLevelCell(const char* name, CCSize size);
        ~ImportLevelCell();

        void draw() override;
        void onView(CCObject*);
        void onDelete(CCObject*);
        void onSelect(CCObject*);
	
	public:
        void loadFromObject(ImportObject*);
        bool isSelected() const;
        ImportObject* getObject() const;

		static ImportLevelCell* create(ImportListView*, const char* key, CCSize size);
};

class ImportListView : public CustomListView {
    protected:
        ImportLayer* m_pLayer;

        void setupList() override;
        TableViewCell* getListCell(const char* key) override;
        void loadCell(TableViewCell* cell, unsigned int index) override;
    
    public:
        ImportLayer* getLayer() const;

        static ImportListView* create(ImportLayer*, CCArray* objs, float width, float height);
};

