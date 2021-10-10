#pragma once

#include "../../BetterEdit.hpp"

class GmdSaveManager {
    protected:
        std::string m_sDirName = "CCLocalLevels";

        bool init();

    public:
        static bool initGlobal();
        static GmdSaveManager* get();

        void load(LocalLevelManager*);
        void save(LocalLevelManager*);
        void backupCCLocalLevels(DS_Dictionary*);

        void setDirName(std::string const&);
        std::string const& getDirName();
};
