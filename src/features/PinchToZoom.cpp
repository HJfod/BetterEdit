
#include <Geode/modify/EditorUI.hpp>
#include <Geode/loader/Mod.hpp>
#include <numbers>

#undef min
#undef max

using namespace geode::prelude;

#ifdef GEODE_IS_MOBILE

// thank you camila icreate 🙏

class $modify(EditorUI) {
    std::unordered_set<Ref<CCTouch>> touches;

    $override
    bool ccTouchBegan(CCTouch* touch, CCEvent* event) override {
        if (!Mod::get()->getSettingValue<bool>("pinch-to-zoom")) {
            return EditorUI::ccTouchBegan(touch, event);
        }
        if (m_fields->touches.size() == 1 || EditorUI::ccTouchBegan(touch, event)) {
            m_fields->touches.insert(touch);
            return true;
        }
        return false;
    }

    $override
    void ccTouchMoved(CCTouch* touch, CCEvent* event) override {
        if (!Mod::get()->getSettingValue<bool>("pinch-to-zoom")) {
            return EditorUI::ccTouchMoved(touch, event);
        }
        if (m_fields->touches.size() == 2) {
            // thanks https://math.stackexchange.com/questions/4408515/calculate-coordinates-after-pinch-to-zoom-gesture!!

            auto winSize = CCDirector::get()->getWinSize();
            auto objLayer = m_editorLayer->m_objectLayer;

            auto it = m_fields->touches.begin();
            auto a = *it;
            auto b = *++it;

            auto aPrev = a->getPreviousLocationInView();
            if (aPrev == CCPointZero) {
                aPrev = a->getLocationInView();
            }
            auto bPrev = b->getPreviousLocationInView();
            if (bPrev == CCPointZero) {
                bPrev = b->getLocationInView();
            }

            auto prevAspectDiv = fabsf(aPrev.x - bPrev.x);
            if (fpclassify(prevAspectDiv) == FP_ZERO) {
                return;
            }
            auto prevAspect = fabsf(aPrev.y - bPrev.y) / prevAspectDiv;

            auto aNext = a->getLocationInView();
            auto bNext = b->getLocationInView();

            auto bMapped = ccp(bNext.x, fabsf(prevAspect * fabsf(aNext.x - bNext.x) - aNext.y));

            auto zoomDiv = fabsf(aPrev.y - bPrev.y);
            if (fpclassify(zoomDiv) == FP_ZERO) {
                return;
            }
            auto zoom = fabsf(aNext.y - bMapped.y) / zoomDiv;

            auto objPos = objLayer->getPosition();
            this->updateZoom(objLayer->getScale() * zoom);
            objLayer->setPosition(ccp(
                aNext.x + zoom * (objPos.x - aPrev.x),
                (winSize.height - aNext.y) + zoom * (objPos.y - (winSize.height - aPrev.y))
            ));
        }
        else {
            EditorUI::ccTouchMoved(touch, event);
        }
    }

    $override
    void ccTouchEnded(CCTouch* touch, CCEvent* event) override {
        if (!Mod::get()->getSettingValue<bool>("pinch-to-zoom")) {
            return EditorUI::ccTouchEnded(touch, event);
        }
        EditorUI::ccTouchEnded(touch, event);
        m_fields->touches.erase(touch);
    }

    $override
    void ccTouchCancelled(CCTouch* touch, CCEvent* event) override {
        if (!Mod::get()->getSettingValue<bool>("pinch-to-zoom")) {
            return EditorUI::ccTouchCancelled(touch, event);
        }
        EditorUI::ccTouchCancelled(touch, event);
        m_fields->touches.erase(touch);
    }
};

#endif
