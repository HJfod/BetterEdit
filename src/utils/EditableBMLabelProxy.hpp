#pragma once

#include <Geode/DefaultInclude.hpp>
#include <Geode/utils/cocos.hpp>

using namespace geode::prelude;

class EditableBMLabelProxy : public CCLabelBMFont, TextInputDelegate {
protected:
    Ref<InputNode> m_input = nullptr;
    std::function<void(std::string const&)> m_setValue = nullptr;
    bool m_ignoreLabelUpdate = false;
    
    static EditableBMLabelProxy* create();

public:
    static EditableBMLabelProxy* replace(
        CCLabelBMFont* existing,
        float width, std::string const& placeholder,
        std::function<void(std::string const&)> setValue
    );

#define PROXY_FWD_1(fun, param_ty) \
    void fun(param_ty x) override { \
        CCLabelBMFont::fun(x); \
        if (m_input) m_input->fun(x); \
    }

    // PROXY_FWD_1(setContentSize, CCSize const&);
    PROXY_FWD_1(setAnchorPoint, CCPoint const&);
    PROXY_FWD_1(setPosition, CCPoint const&);
    PROXY_FWD_1(setRotation, float);
    PROXY_FWD_1(setRotationX, float);
    PROXY_FWD_1(setRotationY, float);
    PROXY_FWD_1(setScale, float);
    PROXY_FWD_1(setScaleX, float);
    PROXY_FWD_1(setScaleY, float);
    PROXY_FWD_1(setVisible, bool);
    PROXY_FWD_1(setSkewX, float);
    PROXY_FWD_1(setSkewY, float);
    PROXY_FWD_1(setZOrder, int);

    void updateLabel() override;
    void setString(const char* str) override;
    void setString(const char* str, bool needUpdateLabel) override;

    void textChanged(CCTextInputNode*) override;
};
