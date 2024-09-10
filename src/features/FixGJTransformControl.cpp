#include <Geode/modify/GJTransformControl.hpp>
#include <Geode/utils/cocos.hpp>

using namespace geode::prelude;

class $modify(GJTransformControl) {

    struct Fields {
        bool m_isPrioFixed = false;
    };

    void updateButtons(bool p0, bool p1) {

        GJTransformControl::updateButtons(p0, p1);

        if (!m_fields->m_isPrioFixed) {
            if (CCMenu* warpButtonMenu = typeinfo_cast<CCMenu*>(m_warpLockButton->getParent())) {
                if (auto delegate = typeinfo_cast<CCTouchDelegate*>(warpButtonMenu)) {
                    if (auto handler = CCTouchDispatcher::get()->findHandler(delegate)) {
                        CCTouchDispatcher::get()->setPriority(handler->getPriority()-1, handler->getDelegate());
                    }
                }
            }
            m_fields->m_isPrioFixed = true;
        }
    }
};

