#pragma once

#include "../../../BetterEdit.hpp"

static constexpr const BoomListType kBoomListType_Import
    = static_cast<BoomListType>(0x42c);

class ImportObject : public CCObject {
    public:
        GJGameLevel* m_pLevel;
        std::string m_sFilePath;

        inline ImportObject(
            GJGameLevel* lvl,
            std::string path
        ) {
            this->m_pLevel = lvl;
            this->m_pLevel->retain();
            this->m_sFilePath = path;
            this->autorelease();
        }

        virtual inline ~ImportObject() {
            this->m_pLevel->release();
        }
};

class ImportListView;
class ImportLayer;

class ImportLevelCell : public TableViewCell {
    protected:
        ImportListView* m_pList;
        ImportObject* m_pObject;

		ImportLevelCell(const char* name, CCSize size);
        ~ImportLevelCell();

        void draw() override;
        void onView(CCObject*);
        void onSelect(CCObject*);
	
	public:
        void loadFromObject(ImportObject*);

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

