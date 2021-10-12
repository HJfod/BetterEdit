#pragma once

#include "../../../BetterEdit.hpp"

class ImportLayer : public CCLayer, public FLAlertLayerProtocol {
    protected:
        CCArray* m_pImportLevels;
        CCMenu* m_pButtonMenu;
        GJListLayer* m_pList = nullptr;

        bool init(CCArray*);
        void onExit(CCObject*);
        void onImportAll(CCObject*);
        void exitForReal();
        void FLAlert_Clicked(FLAlertLayer*, bool) override;
        void keyDown(enumKeyCodes) override;

        virtual ~ImportLayer();

    public:
        void reloadList();
        void addItemsToList(CCArray*);

        static ImportLayer* create(CCArray*);
        static ImportLayer* scene(CCArray*, bool transition = false);
        static ImportLayer* isOpen();
};

