#ifdef GEODE_IS_WINDOWS

#include <Geode/Geode.hpp>
#include <Geode/modify/CreateParticlePopup.hpp>

using namespace geode::prelude;

class $modify (ExportParticles, CreateParticlePopup)
{
    ParticleGameObject* p0;

    void postImport(std::string path)
    {
        log::info("particle path: {}", path);

        auto pp = CCParticleSystemQuad::create((path).c_str(), false);

        auto ss = GameToolbox::saveParticleToString(pp);
        m_fields->p0->setParticleString(ss);
        m_fields->p0->updateParticle();

        this->removeFromParent();
    }

    void importFile(CCObject*)
    {
        std::vector<file::FilePickOptions::Filter> format = { 
            {
                "Cocos2d-x Particle File",
                { "*.plist" }
            }
        };

        file::FilePickOptions options = {
            "",
            format
        };

        auto callback = [this](ghc::filesystem::path result) {
            auto path = result.relative_path().string();
            #ifdef GEODE_IS_WINDOWS
            auto strPath = geode::utils::string::wideToUtf8(result.c_str());
            #else
            std::string strPath = result.c_str();
            #endif

            postImport(strPath);
        };

        auto failedCallback = []() {
            FLAlertLayer::create("Error", "<cr>Failed</c> to open particle file.\nDid you cancel the <cl>open</c>?", "OK")->show();
        };

        file::pickFile(file::PickMode::OpenFile, options, callback, failedCallback);
    }

    /*void exportFile(CCObject*)
    {
        if (auto preview = getChildOfType<ParticlePreviewLayer>(as<CCLayer*>(this->getChildren()->objectAtIndex(0)), 0))
        {
            auto pp = as<CCParticleSystemQuad*>(preview->getChildren()->objectAtIndex(0));

            // writeToFile isn't in cocos2d 2 so i have to use "my own" ai generated version
            exportParticleSystemToDictionary(pp, "square.png")->writeToFile("particleExport.plist");

            CCScene::get()->addChild(TextAlertPopup::create("Saved particle to 'particleExport.plist' in the Resources folder", 0.5f, 0.6f, 150, ""), 9999999);
        }
    }*/

    bool init(ParticleGameObject* p0, cocos2d::CCArray* p1, gd::string p2)
    {
        if (!CreateParticlePopup::init(p0, p1, p2))
            return false;

        m_fields->p0 = p0;

        auto m = CCMenu::create();
        m->setContentSize(ccp(460, 300));
        m->ignoreAnchorPointForPosition(false);
        m->setPosition(CCDirector::get()->getWinSize() / 2);

        auto btnImp = ButtonSprite::create("Import", "goldFont.fnt", "GJ_button_04.png");
        btnImp->setScale(0.5f);
        m->addChildAtPosition(CCMenuItemSpriteExtra::create(btnImp, this, menu_selector(ExportParticles::importFile)), Anchor::BottomLeft, ccp(38.5f, 20));

        //auto btnExp = ButtonSprite::create("Export", "goldFont.fnt", "GJ_button_04.png");
        //btnExp->setScale(0.5f);
        //m->addChildAtPosition(CCMenuItemSpriteExtra::create(btnExp, this, menu_selector(ExportParticles::exportFile)), Anchor::BottomLeft, ccp(99.5f, 20));

        this->addChild(m);
        m->setTouchPriority(-569);
        return true;
    }
};

#endif