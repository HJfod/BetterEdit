#pragma once

#include <Geode/DefaultInclude.hpp>
#include <Geode/utils/cocos.hpp>
#include <Geode/ui/TextInput.hpp>
#include <Geode/binding/TextInputDelegate.hpp>

using namespace geode::prelude;

class EditableBMLabelProxy : public CCLabelBMFont, TextInputDelegate {
protected:
    Ref<TextInput> m_input = nullptr;
    CCNode* m_inputParent = nullptr;
    std::function<void(std::string const&)> m_onSetValue = nullptr;
    std::function<void(std::string const&)> m_onUpdate = nullptr;
    bool m_ignoreLabelUpdate = false;
    
    static EditableBMLabelProxy* create();

public:
    static EditableBMLabelProxy* replace(
        CCLabelBMFont* existing,
        CCNode* inputParent,
        float width, std::string const& placeholder,
        std::function<void(std::string const&)> onSetValue = nullptr,
        std::function<void(std::string const&)> onUpdate = nullptr
    );

    void update(float) override;

#define PROXY_FWD_1(fun, param_ty) \
    void fun(param_ty x) override { \
        CCLabelBMFont::fun(x); \
        if (m_input) m_input->fun(x); \
    }

    // PROXY_FWD_1(setContentSize, CCSize const&);
    PROXY_FWD_1(setAnchorPoint, CCPoint const&);
    PROXY_FWD_1(setRotation, float);
    PROXY_FWD_1(setRotationX, float);
    PROXY_FWD_1(setRotationY, float);
    PROXY_FWD_1(setScaleX, float);
    PROXY_FWD_1(setScaleY, float);
    PROXY_FWD_1(setSkewX, float);
    PROXY_FWD_1(setSkewY, float);
    PROXY_FWD_1(setVisible, bool);
    PROXY_FWD_1(setZOrder, int);
    PROXY_FWD_1(setOrderOfArrival, unsigned int);

    void setPosition(CCPoint const& pos) override;
    void setScale(float scale) override;
    void setColor(ccColor3B const& color) override;

    void updateLabel() override;
    void setString(const char* str) override;
    void setString(const char* str, bool needUpdateLabel) override;

    void textChanged(CCTextInputNode*) override;
};
