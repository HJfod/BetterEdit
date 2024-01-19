#include <Geode/Bindings.hpp>
#include <Geode/modify/EditorUI.hpp>

using namespace geode::prelude;

class $modify (EditorUI)
{
    bool init(LevelEditorLayer* p0)
    {
        if (!EditorUI::init(p0))
            return false;

        float scale = Mod::get()->getSettingValue<double>("scale-factor");
        auto size = CCDirector::get()->getWinSize();

        auto slider = reinterpret_cast<Slider*>(this->getChildren()->objectAtIndex(0));
        slider->ignoreAnchorPointForPosition(false);
        slider->setContentSize(ccp(0, 0));
        slider->setScale(scale);
        slider->setPositionY(size.height - (size.height - slider->getPositionY()) * scale);

        auto rightMenu = reinterpret_cast<CCMenu*>(this->getChildren()->objectAtIndex(this->getChildrenCount() - 4));

        log::info("kids: {}", rightMenu->getChildrenCount());

        for (size_t i = 0; i < rightMenu->getChildrenCount(); i++)
        {
            auto n = reinterpret_cast<CCNode*>(rightMenu->getChildren()->objectAtIndex(i));
            n->setPositionX(n->getPositionX() * scale);

            if (static_cast<CCMenuItemSpriteExtra*>(n))
            {
                static_cast<CCMenuItemSpriteExtra*>(n)->m_baseScale *= scale;
            }

            n->setScale(n->getScale() * scale);
        }
        
        auto layerText = reinterpret_cast<CCLabelBMFont*>(this->getChildren()->objectAtIndex(this->getChildrenCount() - 3));
        layerText->setScale(layerText->getScale() * scale);
        layerText->setPositionX(size.width - (size.width - layerText->getPositionX()) * scale);
        //todo:fix text scaling
        //layerText->limitLabelWidth()

        //auto pause = reinterpret_cast<CCMenuItemSpriteExtra*>(rightMenu->getChildren()->objectAtIndex(0));
        //pause->setScale(scale);
        //pause->m_baseScale = scale;

        //auto options = reinterpret_cast<CCMenuItemSpriteExtra*>(rightMenu->getChildren()->objectAtIndex(1));
        //options->setScale(scale);
        //options->m_baseScale = scale;

        return true;
    }
};