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
        virtual std::string describe() = 0;
        virtual CCNode* createActionCellItem(float width, float height);
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

        virtual CCNode* createActionCellItem(float width, float height);

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
        inline virtual std::string describe() {
            return "Create Object " + BetterEdit::formatToString(this->m_nObjectID) +
                " at " + BetterEdit::formatToString(this->m_obPosition);
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
        inline virtual std::string describe() {
            return "Remove Object";
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
        CCPoint m_obBy;
    
    public:
        inline virtual std::string describe() {
            return "Move Object by "_s +
                BetterEdit::formatToString(m_obBy);
        }

        inline MoveObjectAction(GameObject* obj, CCPoint const& by) :
            ObjectAction(obj->m_nUniqueID), m_obBy(by) {}

        inline MoveObjectAction(CCArray* objs, CCPoint const& by) :
            ObjectAction(objs), m_obBy(by) {}
};

class ScaleObjectAction : public ObjectAction {
    protected:
        float m_fScaleTo;
    
    public:
        inline virtual std::string describe() {
            return "Scale Object to " +
                BetterEdit::formatToString(this->m_fScaleTo);
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
        inline virtual std::string describe() {
            return "Rotate Object to " +
                BetterEdit::formatToString(this->m_fRotateTo);
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
        inline virtual std::string describe() {
            return "Select Object";
        }

        inline SelectObjectAction(GameObject* obj) :
            ObjectAction(obj->m_nUniqueID) {}

        inline SelectObjectAction(CCArray* objs) :
            ObjectAction(objs) {}
};
