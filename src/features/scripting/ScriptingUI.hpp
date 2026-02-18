#include "Scripting.hpp"
#include <Geode/loader/Mod.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/binding/CCMenuItemToggler.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>
#include <Geode/ui/Popup.hpp>
#include <Geode/ui/ScrollLayer.hpp>
#include <Geode/utils/ranges.hpp>
#include <Geode/ui/General.hpp>

using namespace geode::prelude;

class RunScriptPopup;

class ScriptNode : public CCNode {
protected:
    RunScriptPopup* m_popup;
    std::shared_ptr<JsScript> m_script;
    CCScale9Sprite* m_bg;
    CCSprite* m_runSpr;
    CCMenuItemSpriteExtra* m_runBtn;
    CCMenuItemToggler* m_logsToggle;
    CCLabelBMFont* m_logsLabel;
    ListenerHandle m_logListener;
    CCScale9Sprite* m_selectionSprite;

    bool init(RunScriptPopup* popup, std::shared_ptr<JsScript> script, float width);

    void onRun(CCObject*);
    void onLogs(CCObject*);

public:
    static ScriptNode* create(RunScriptPopup* popup, std::shared_ptr<JsScript> script, float width);

    void updateState();
};

class RunScriptPopup : public Popup {
protected:
    ScrollLayer* m_list;
    ScrollLayer* m_logsList;
    ListenerHandle m_logListener;
    static std::weak_ptr<JsScript> s_selected;

    bool init();
    void reloadList();
    void updateLogs();

    void onReload(CCObject*);

    friend class ScriptNode;

public:
    static RunScriptPopup* create();

    void view(std::shared_ptr<JsScript> script);
};
