#include "ScriptSelectLayer.hpp"
#include "LuaManager.hpp"

#include <Geode/loader/Mod.hpp>
#include <geode.custom-keybinds/include/Keybinds.hpp>


using namespace cocos2d;
using namespace keybinds;
namespace fs = ghc::filesystem;

ScriptSelectLayer* ScriptSelectLayer::create() {
    auto ret = new ScriptSelectLayer();
    if (ret && ret->initAnchored(400.0f, 250.0f, "GJ_Square02.png")) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

void ScriptSelectLayer::onScript(CCObject* btn)
{
    int index = btn->getTag();    
    if(index < 0 || static_cast<size_t>(index) >= scripts.size())
    {
        geode::log::error("index script tag out of bounds. index: {}, size: {}", index, scripts.size());
    }

    geode::Loader::get()->queueInMainThread(
        [scriptPath = scripts[index]]{ LuaManager::runScript(scriptPath); }
    );

    this->onClose(nullptr);
}


void ScriptSelectLayer::onOpenPath(cocos2d::CCObject*)
{
    geode::utils::file::openFolder(scripts_dir);
}
void ScriptSelectLayer::onSelectFile(cocos2d::CCObject*)
{
    auto onSelect = [this](fs::path file)
    {
        if(!fs::exists(scripts_dir))
        {
            return geode::Loader::get()->queueInMainThread([]{
                geode::Notification::create("Path does not exist", geode::NotificationIcon::Error)->show();
            });
        }
        fs::copy_file(file, scripts_dir / file.filename(), fs::copy_options::overwrite_existing);
        onClose(nullptr);
        geode::Loader::get()->queueInMainThread([]{ ScriptSelectLayer::create()->show(); });
    };

    auto filter = geode::utils::file::FilePickOptions{};
    geode::utils::file::pickFile(geode::utils::file::PickMode::OpenFile, filter, onSelect);

}

void ScriptSelectLayer::onCopyPath(cocos2d::CCObject*)
{
    if(fs::exists(scripts_dir))
    {
        geode::utils::clipboard::write(scripts_dir.string());
        geode::Notification::create("Copied to clipboard", geode::NotificationIcon::Success)->show();
    }
}


void ScriptSelectLayer::setScriptMembers()
{
    if(auto scriptspath = ScriptInfo::getScriptsPath())
    {
        scripts_dir = *scriptspath;
        scripts = ScriptInfo::getScriptsFromFolder(scripts_dir);
    }
}

void ScriptSelectLayer::onUpdateBindings(CCObject*)
{
    updateBindings();
}

bool ScriptSelectLayer::setup()
{
    setScriptMembers();

    this->setTitle("Run script file", "bigFont.fnt", 0.7f);


    auto winSize = CCDirector::get()->getWinSize();
    auto btnMenu = CCMenu::create();
    btnMenu->setLayout(RowLayout::create());
    btnMenu->setPosition(m_mainLayer->convertToNodeSpace(winSize / 2 + CCPoint(0, 65)));
    m_mainLayer->addChild(btnMenu);

    if(!fs::exists(scripts_dir))
    {
        auto label = CCLabelBMFont::create("Could not create scripts directory", "bigFont.fnt");
        label->setScale(.7f);
        label->setPosition(btnMenu->getPosition() + CCPoint(0, -60));
        m_mainLayer->addChild(label);
        return true;
    }

    for(const auto& btndata : std::initializer_list<std::pair<const char*, SEL_MenuHandler>>
        {{"Open Path", menu_selector(ScriptSelectLayer::onOpenPath)},
        {"Add File", menu_selector(ScriptSelectLayer::onSelectFile)},
        {"Copy Path", menu_selector(ScriptSelectLayer::onCopyPath)},
        {"Update Bindings", menu_selector(ScriptSelectLayer::onUpdateBindings)}}
    )
    {
        auto spr = ButtonSprite::create(btndata.first, "bigFont.fnt", "GJ_button_04.png", 0.7f);
        spr->setScale(.6);
        auto btn = CCMenuItemSpriteExtra::create(spr, this, btndata.second);
        btnMenu->addChild(btn);
    }

    btnMenu->updateLayout();



    if(scripts.empty())
    {
        auto label = CCLabelBMFont::create("Add scripts to folder :)", "bigFont.fnt");
        label->setScale(.7f);
        label->setPosition(btnMenu->getPosition() + CCPoint(0, -60));
        m_mainLayer->addChild(label);

        auto pathLabel = CCLabelBMFont::create(scripts_dir.string().c_str(), "chatFont.fnt");
        pathLabel->limitLabelWidth(m_size.width - 50, 1, 0.1);
        pathLabel->setPosition(label->getPosition() + CCPoint(0, -30));
        m_mainLayer->addChild(pathLabel);
        return true;
    }


    auto scroll = geode::ScrollLayer::create({320, 146});
    scroll->m_contentLayer->setContentSize({320, static_cast<float>(30 * scripts.size())});

    auto pos = winSize / 2 - scroll->getContentSize() / 2 + ccp(0, -30);
    scroll->setPosition(m_mainLayer->convertToNodeSpace(pos));
    m_mainLayer->addChild(scroll);

    auto playtestSpr = CCSprite::createWithSpriteFrameName("GJ_playEditorBtn_001.png");
    playtestSpr->setScale(0.6f);

    for(int i = 0; const auto& s : this->scripts)
    {
        auto node = CCLayerColor::create();
        node->setColor({0, 0, 0});
        node->setOpacity((i % 2 == 0) ? 100 : 50);
        node->setContentSize(ccp(320, 30));
        node->setPosition(ccp(0, 30 * scripts.size() - ((i + 1) * 30)));

        auto menu = CCMenu::create();
        menu->setPosition(ccp(0, 0));
        menu->setContentSize({0, 0});
        menu->ignoreAnchorPointForPosition(false);
        menu->setAnchorPoint(ccp(0, 0));
        node->addChild(menu);

        auto label = CCLabelBMFont::create(s.name.c_str(), "bigFont.fnt");
        label->limitLabelWidth(200.0f, 0.6f, 0.1f);
        label->setAnchorPoint(ccp(0, 0.5f));
        label->setPosition(ccp(2, 15));
        node->addChild(label);

        auto authorlabel = CCLabelBMFont::create(fmt::format("By {}", s.dev).c_str(), "goldFont.fnt");

        float limitWidth = 320 - label->getScaledContentSize().width - playtestSpr->getContentWidth();
        authorlabel->limitLabelWidth(limitWidth, 0.6f, 0.1f);
        authorlabel->setAnchorPoint(ccp(0, 0.5f));
        authorlabel->setPosition(label->getPosition() + CCPoint(label->getScaledContentSize().width + (limitWidth / 20), 0));
        node->addChild(authorlabel);

        auto btn = CCMenuItemSpriteExtra::create(playtestSpr, this, menu_selector(ScriptSelectLayer::onScript));
        btn->setPosition(node->getContentSize().width - 20, 15);
        btn->setTag(i);
        menu->addChild(btn);

        scroll->m_contentLayer->addChild(node);
        i++;
    }

    if(auto count = scripts.size(); count < 5)
    {
        float offset = (5 - count) * 25;
        scroll->setPositionY(scroll->getPositionY() + offset);
        scroll->m_disableMovement = true;
    }
    else
    {
        m_mainLayer->addChildAtPosition(geode::Scrollbar::create(scroll), Anchor::Right, CCPoint(-30, -30));
        scroll->moveToTop();
    }

    return true;
}

void ScriptSelectLayer::updateBindings()
{
    auto bm = BindManager::get();
    auto scriptBindings = bm->getBindablesIn("Scripts");

    auto path = ScriptInfo::getScriptsPath();
    if(!path) return;
    auto scripts = ScriptInfo::getScriptsFromFolder(*path);


    auto scriptInBindingsExists = [&](const keybinds::BindableAction& binding) -> bool
    {
        for(const auto& existingScript : scripts)
        {
            auto id = binding.getID();
            auto id2 = existingScript.getUniqueString();
            if(id == id2)
                return true;
        }
        return false;
    };

    for(const auto& scriptInBinding : scriptBindings)
    {
        if(!scriptInBindingsExists(scriptInBinding))
        {
            bm->removeBindable(scriptInBinding.getID());
        }
    }

    for(const auto& script : scripts)
    {
        bm->registerBindable({
            //ID
            script.getUniqueString(),
            // Name
            fmt::format("{} by {}", script.name, script.dev),
            // Description, leave empty for none
            "",
            // Default binds
            { },
            // Category; use slashes for specifying subcategories. See the
            // Category class for default categories
            "Scripts"
        });
    }
}