#pragma once

#include "../../../BetterEdit.hpp"
#include "ImportObject.hpp"

class ImportLayer : public CCLayer, public FLAlertLayerProtocol {
    protected:
        CCArray* m_pImportLevels;
        CCMenu* m_pButtonMenu;
        GJListLayer* m_pList = nullptr;
        CCLabelBMFont* m_pInfoLabel;

        bool init(CCArray*);
        void onExit(CCObject*);
        void onImportSelected(CCObject*);
        void onDeleteSelected(CCObject*);
        void exitForReal();
        void FLAlert_Clicked(FLAlertLayer*, bool) override;
        void keyDown(enumKeyCodes) override;

        virtual ~ImportLayer();

    public:
        void reloadList();
        void addItemsToList(CCArray*);
        void removeItemFromList(ImportObject*);

        static ImportLayer* create(CCArray*);
        static ImportLayer* scene(CCArray*, bool transition = false);
        static ImportLayer* isOpen();
};

