#include "ScriptSelectLayer.hpp"
#include "LuaManager.hpp"

#include <Geode/loader/Mod.hpp>


using namespace cocos2d;
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
    try
    {
        auto& script = this->scripts.at(index);
        LuaManager::runScript(script.path);

        geode::Notification::create(script.name, geode::NotificationIcon::Success, 1.0f)->show();

        this->onClose(nullptr);
    }
    catch(std::runtime_error& e)
    {
        std::string str = fmt::format("{}: {} | Internal error type: {}", scripts[index].name, e.what(), typeid(e).name());
        geode::log::error("{}", str);
        geode::Notification::create(scripts[index].name, geode::NotificationIcon::Error, 1.0f)->show();
    }
    catch(std::exception& e)
    {
        geode::log::error("Unknown error running script: {}", e.what());
    }
}


void ScriptSelectLayer::onOpenPath(cocos2d::CCObject*)
{
    geode::utils::file::openFolder(scripts_dir);
}
void ScriptSelectLayer::onSelectFile(cocos2d::CCObject*)
{
    auto onSelect = [this](fs::path file)
    {
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
    auto MOD = geode::Mod::get();
    scripts_dir = MOD->getConfigDir() / "scripts";

    if(fs::is_directory(scripts_dir))
    {
        scripts = ScriptInfo::getScriptsFromFolder(scripts_dir);
        return;
    }

    if(!fs::is_symlink(scripts_dir)) return;

    auto sympath = fs::read_symlink(scripts_dir);
    if(!fs::is_directory(sympath)) return;

    scripts_dir = sympath;
    scripts = ScriptInfo::getScriptsFromFolder(scripts_dir);
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

    for(const auto& btndata : std::initializer_list<std::pair<const char*, SEL_MenuHandler>>
        {{"Open Path", menu_selector(ScriptSelectLayer::onOpenPath)},
        {"Add File", menu_selector(ScriptSelectLayer::onSelectFile)},
        {"Copy Path", menu_selector(ScriptSelectLayer::onCopyPath)}}
    )
    {
        auto spr = ButtonSprite::create(btndata.first, "bigFont.fnt", "GJ_button_04.png", 0.7f);
        spr->setScale(.7);
        auto btn = CCMenuItemSpriteExtra::create(spr, this, btndata.second);
        btnMenu->addChild(btn);
    }

    btnMenu->updateLayout();


    if(scripts.empty() || !fs::exists(scripts_dir))
    {
        bool created = fs::is_directory(scripts_dir) || fs::create_directory(scripts_dir);
        const char* str = created ? "Add scripts to folder :)" : "Could not create directory";
        auto label = CCLabelBMFont::create(str, "bigFont.fnt");
        label->setScale(.7f);
        label->setPosition(btnMenu->getPosition() + CCPoint(0, -60));
        m_mainLayer->addChild(label);

        if(created)
        {
            auto pathLabel = CCLabelBMFont::create(scripts_dir.string().c_str(), "chatFont.fnt");
            pathLabel->limitLabelWidth(m_size.width - 50, 1, 0.1);
            pathLabel->setPosition(label->getPosition() + CCPoint(0, -30));
            m_mainLayer->addChild(pathLabel);
        }
        else
        {
            this->scripts_dir = fs::path();
        }
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
        geode::log::info("{}", limitWidth);
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