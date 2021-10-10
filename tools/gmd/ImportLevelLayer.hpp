#pragma once

#include "ImportListView.hpp"

class ImportLevelLayer : public CCLayer {
    protected:
        ImportListView* m_pList;
        ImportObject* m_pObject;
        CCMenu* m_pButtonMenu;

        bool init(ImportListView*, ImportObject*);
        void onExit(CCObject*);
        void onImport(CCObject*);
        void keyDown(enumKeyCodes) override;

        virtual ~ImportLevelLayer();

    public:
        static ImportLevelLayer* create(ImportListView*, ImportObject*);
        static ImportLevelLayer* scene(ImportListView*, ImportObject*);
};

