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
                while (it != line.end() && *it == '#' || std::isspace(*it)) {
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
                    res += "\n# " + ver.toString() + "\n\n";
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

bool ChangelogPopup::setup(std::optional<VersionInfo> const& since) {
    m_noElasticity = true;

    this->setTitle("BetterEdit Changelog");
    this->addCorners(Corner::Blue, Corner::Dark);

    auto full = parseChangelog(since);

    auto textArea = MDTextArea::create(full, ccp(250, 140));
    m_mainLayer->addChildAtPosition(textArea, Anchor::Center);

    if (since) {
        auto note = CCLabelBMFont::create(
            "View the full changelog history\nin the BetterEdit About menu!",
            "bigFont.fnt"
        );
        note->setColor({ 55, 255, 155 });
        note->setScale(.35f);
        m_mainLayer->addChildAtPosition(note, Anchor::Bottom, ccp(0, 25));
    }

    return true;
}

ChangelogPopup* ChangelogPopup::create(std::optional<VersionInfo> const& since) {
    auto ret = new ChangelogPopup();
    if (ret && ret->initAnchored(340, 225, since, "GJ_square02.png")) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}
