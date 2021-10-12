#pragma once

#include "../../../BetterEdit.hpp"

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
