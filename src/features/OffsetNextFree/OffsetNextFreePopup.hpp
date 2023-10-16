#include <Geode/Geode.hpp>
#include <Geode/utils/cocos.hpp>

#include "OffsetType.hpp"
#include "OffsetManager.hpp"

using namespace geode::prelude;

class OffsetNextFreePopup : public Popup<OffsetType, FLAlertLayer*> {
protected:
    OffsetType m_offsetType;
    CCTextInputNode* m_offsetInput = nullptr;
    FLAlertLayer* m_parent = nullptr;
    bool setup(OffsetType offsetType, FLAlertLayer* parent) override;

    virtual void onClose(CCObject*) override;
public:
    size_t getOffsetFromInput();
    void updateOffset();
    void updateParentButton();
    void onClear(CCObject*);

    static OffsetNextFreePopup* create(OffsetType offsetType, FLAlertLayer* parent) {
        auto ret = new OffsetNextFreePopup;
        if (ret && ret->init(175.f, 150.f, offsetType, parent, "GJ_square02.png")) {
            ret->autorelease();
            return ret;
        }

        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};