#pragma once

#include <Geode/ui/Popup.hpp>

#include <Geode/cocos/label_nodes/CCLabelBMFont.h>
#include <Geode/cocos/CCDirector.h>
#include "ScriptInfo.hpp"
#include <vector>
#include <ghc/filesystem.hpp>

// specify parameters for the setup function in the Popup<...> template
struct ScriptSelectLayer : public geode::Popup<>
{
    ghc::filesystem::path scripts_dir;
    std::vector<ScriptInfo> scripts;

    bool setup() override;
    void setScriptMembers();
    
    static ScriptSelectLayer* create();
    void onScript(cocos2d::CCObject*);
    void onOpenPath(cocos2d::CCObject*);
    void onSelectFile(cocos2d::CCObject*);
    void onCopyPath(cocos2d::CCObject*);
    void onUpdateBindings(cocos2d::CCObject*);
    
    static void updateBindings();
};
