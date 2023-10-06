#include <Geode/Geode.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>

#include <sstream>
#include <string>

using namespace geode::prelude;

class $modify(LDMCountEditorPauseLayer, EditorPauseLayer) {
    bool init(LevelEditorLayer* lel) {
        if (!EditorPauseLayer::init(lel)) {
            return false;
        }

        auto menu = static_cast<CCMenu*>(this->getChildByID("info-menu"));
        auto objectCountLabel = static_cast<CCLabelBMFont*>(menu->getChildByID("object-count-label"));

        std::string objectCountText = objectCountLabel->getString();
        std::stringstream ss;
        ss << std::string(objectCountLabel->getString());
        int ldmCount = this->countLDMObjects();
        float percentage = static_cast<float>(ldmCount) / this->countValidObjects() * 100;

        ss << " | " << ldmCount << " LDM (" << std::fixed << std::setprecision(2) << percentage << "%)";
        objectCountLabel->setString(ss.str().c_str());

        return true;
    }

    // Counts all objects that matter to the object count
    int countValidObjects() {
        int count = 0;
        for (auto object : CCArrayExt<GameObject*>(m_editorLayer->getAllObjects())) {
            if (object->m_objectID == 31) {
                continue;
            }

            count++;
        }

        return count;
    }

    int countLDMObjects() {
        int count = m_editorLayer->getAllObjects()->count();
        for (auto object : CCArrayExt<GameObject*>(m_editorLayer->getAllObjects())) {
            // 31 - startpos
            if (object->m_highDetail || object->m_objectID == 31) {
                count--;
            }
        }
        return count;
    }
};