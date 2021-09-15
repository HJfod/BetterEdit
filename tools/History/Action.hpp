#pragma once

#include "../../BetterEdit.hpp"

class UndoHistoryManager;

static GameObject* getObjectByUUID(LevelEditorLayer* lel, int uuid) {
    CCARRAY_FOREACH_B_TYPE(lel->getAllObjects(), obj, GameObject) {
        if (obj->m_nUniqueID == uuid)
            return obj;
    }
    return nullptr;
}

class ActionObject : public CCObject {
    protected:
        inline static int g_nID = 0;
        int m_nID;
        std::chrono::system_clock::time_point m_obTime;
        bool m_bUndone;

        inline ActionObject() {
            this->m_nID = g_nID;
            this->m_obTime = std::chrono::system_clock::now();
            this->autorelease();

            g_nID++;
        }

        friend class UndoHistoryManager;

    public:
        virtual CCNode* createActionCellItem() = 0;
        virtual void undoAction() {
            this->m_bUndone = true;
        }
        virtual void redoAction() {
            this->m_bUndone = false;
        }
};

class ObjectAction : public ActionObject {
    protected:
        int m_nObjectUUID = 0;
        std::vector<int> m_vObjectUUIDs;

        inline ObjectAction(int uuid) : m_nObjectUUID(uuid) {}
        inline ObjectAction(CCArray* objs) {
            CCARRAY_FOREACH_B_TYPE(objs, obj, GameObject) {
                m_vObjectUUIDs.push_back(obj->m_nUniqueID);
            }
        }
};

class AddObjectAction : public ObjectAction {
    protected:
        CCPoint m_obPosition;
        int m_nObjectID;

    public:
        inline virtual CCNode* createActionCellItem() override {
            return CCLabelBMFont::create("Add Object", "bigFont.fnt");
        }

        inline AddObjectAction(GameObject* obj) : ObjectAction(obj->m_nUniqueID) {
            this->m_obPosition = obj->getPosition();
            this->m_nObjectID = obj->m_nObjectID;
        }

        inline virtual void undoAction() {
            auto obj = getObjectByUUID(
                LevelEditorLayer::get(),
                this->m_nObjectUUID
            );

            if (obj) LevelEditorLayer::get()->removeObject(obj, false);
        }
};

class RemoveObjectAction : public ObjectAction {
    public:
        inline virtual CCNode* createActionCellItem() override {
            return CCLabelBMFont::create("Add Object", "bigFont.fnt");
        }

        inline RemoveObjectAction(GameObject* obj) :
            ObjectAction(obj->m_nUniqueID) {}

        inline RemoveObjectAction(CCArray* objs) :
            ObjectAction(objs) {}

        inline RemoveObjectAction(int uuid) :
            ObjectAction(uuid) {}
};

class MoveObjectAction : public ObjectAction {
    protected:
        CCPoint m_obFrom;
        CCPoint m_obTo;
    
    public:
        inline virtual CCNode* createActionCellItem() override {
            auto str = "Move Object to "_s;
            str += std::to_string(m_obTo.x) + ", " + std::to_string(m_obTo.y);
            str += " from ";
            str += std::to_string(m_obFrom.x) + ", " + std::to_string(m_obFrom.y);

            return CCLabelBMFont::create(str.c_str(), "bigFont.fnt");
        }

        inline MoveObjectAction(GameObject* obj, CCPoint const& from, CCPoint const& to) :
            ObjectAction(obj->m_nUniqueID), m_obTo(to), m_obFrom(from) {}

        inline MoveObjectAction(CCArray* objs, CCPoint const& from, CCPoint const& to) :
            ObjectAction(objs), m_obTo(to), m_obFrom(from) {}
};

class ScaleObjectAction : public ObjectAction {
    protected:
        float m_fScaleTo;
    
    public:
        inline virtual CCNode* createActionCellItem() override {
            return CCLabelBMFont::create("Scale Object", "bigFont.fnt");
        }

        inline ScaleObjectAction(GameObject* obj) :
            ObjectAction(obj->m_nUniqueID),
            m_fScaleTo(obj->getScale()) {}

        inline ScaleObjectAction(CCArray* objs, float scale) :
            ObjectAction(objs),
            m_fScaleTo(scale) {}
};

class RotateObjectAction : public ObjectAction {
    protected:
        float m_fRotateTo;
    
    public:
        inline virtual CCNode* createActionCellItem() override {
            return CCLabelBMFont::create("Rotate Object", "bigFont.fnt");
        }

        inline RotateObjectAction(GameObject* obj) :
            ObjectAction(obj->m_nUniqueID),
            m_fRotateTo(obj->getRotation()) {}

        inline RotateObjectAction(CCArray* objs, float rot) :
            ObjectAction(objs),
            m_fRotateTo(rot) {}
};

class SelectObjectAction : public ObjectAction {
    public:
        inline virtual CCNode* createActionCellItem() override {
            return CCLabelBMFont::create("Select Object", "bigFont.fnt");
        }

        inline SelectObjectAction(GameObject* obj) :
            ObjectAction(obj->m_nUniqueID) {}

        inline SelectObjectAction(CCArray* objs) :
            ObjectAction(objs) {}
};
