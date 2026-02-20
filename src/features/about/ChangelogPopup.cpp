#include "ChangelogPopup.hpp"
#include <Geode/utils/string.hpp>
#include <Geode/ui/MDTextArea.hpp>

static std::string parseChangelog(std::optional<VersionInfo> const& since) {
    auto changelog = Mod::get()->getMetadata().getChangelog().value_or(std::string());
    auto res = std::string();

    bool validHeader = false;
    for (auto line : string::split(changelog, "\n")) {
        string::trimIP(line);
        // Skip empty lines
        if (line.empty()) {
            continue;
        }
        auto it = line.begin();
        switch (*it) {
            case '#': {
                // Skip to header content
                while (it != line.end() && (*it == '#' || std::isspace(*it))) {
                    it++;
                }
                auto parse = VersionInfo::parse(std::string(it, line.end()));
                // Skip if the header is not a version
                if (parse.isErr()) {
                    continue;
                }
                auto ver = parse.unwrap();
                if (!since || since.value() < ver) {
                    validHeader = true;
                    res += "\n# " + ver.toNonVString() + "\n\n";
                }
                else {
                    validHeader = false;
                }
            } break;

            default: {
                // Skip items without a header or ones that belong to a version not to be included
                if (res.empty() || !validHeader) {
                    continue;
                }
                res += line + "\n";
            } break;
        }
    }

    return res;
}

bool ChangelogPopup::init(std::optional<VersionInfo> const& since) {
    if (!Popup::init(340, 230, "GJ_square02.png"))
        return false;

    m_noElasticity = true;

    this->setTitle("BetterEdit Changelog");
    this->addCorners(Corner::Blue, Corner::Dark);

    auto full = parseChangelog(since);

    if (since) {
        full =
            "> View the full changelog history in the "
            "<cg>BetterEdit popup</c> from the editor pause layer!\n\n" + full;
        
        auto toggleMenu = CCMenu::create();
        toggleMenu->setContentWidth(230);

        auto toggleLabel = CCLabelBMFont::create("Show Changelog on Startup", "bigFont.fnt");
        toggleLabel->setScale(.4f);
        toggleMenu->addChildAtPosition(toggleLabel, Anchor::Left, ccp(25, 0), ccp(0, .5f));
        
        m_showToggle = CCMenuItemToggler::createWithStandardSprites(
            this, menu_selector(ChangelogPopup::onToggleShow), .5f
        );
        m_showToggle->m_notClickable = true;
        m_showToggle->toggle(Mod::get()->getSettingValue<bool>("enable-changelog-popup"));
        toggleMenu->addChildAtPosition(m_showToggle, Anchor::Left, ccp(10, 0));

        m_mainLayer->addChildAtPosition(toggleMenu, Anchor::Bottom, ccp(0, 22));
    }

    auto textArea = MDTextArea::create(full, ccp(250, 150));
    m_mainLayer->addChildAtPosition(textArea, Anchor::Center);

    return true;
}

void ChangelogPopup::onToggleShow(CCObject*) {
    Mod::get()->setSettingValue("enable-changelog-popup", !m_showToggle->m_toggled);
    m_showToggle->toggle(Mod::get()->getSettingValue<bool>("enable-changelog-popup"));
}

ChangelogPopup* ChangelogPopup::create(std::optional<VersionInfo> const& since) {
    auto ret = new ChangelogPopup();
    if (ret && ret->init(since)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}
