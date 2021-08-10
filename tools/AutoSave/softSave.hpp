#pragma once

#include "../../BetterEdit.hpp"

static constexpr const char* g_softSave_file = "BE_softsave.dat";

class SoftSaveManager {
    protected:
        CCArray* m_pManagedObjects;
        CCArray* m_pManagedColors;
        bool m_bDontSave;
        int m_nCurrentActionCount;

        void saveObjectBinary(std::ofstream & stream, GameObject* obj);
        void loadObjectBinary(std::ifstream & stream);

        void saveColorBinary(std::ofstream & stream, int color);
        void loadColorBinary(std::ifstream & stream);

        bool init();
    
    public:
        static bool initGlobal();

        static void saveObject(GameObject* obj);
        static void removeObject(GameObject* obj);
        static void saveColor(int col);
        static void save();
        static void load();
        static void clear();

        static bool hasLoad(std::string const& level);
};

class SoftSaveAlertDelegate : public CCObject, public FLAlertLayerProtocol {
    public:
        void FLAlert_Clicked(FLAlertLayer*, bool) override;
        
        static SoftSaveAlertDelegate* create();
};
