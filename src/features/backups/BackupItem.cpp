#include "BackupItem.hpp"
#include "BackupListPopup.hpp"
#include <fmt/chrono.h>
#include <utils/EditorViewOnlyMode.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>

bool BackupItem::init(BackupPtr backup) {
    if (!CCNode::init())
        return false;

    this->setContentSize({ 275, 35 });
    
    m_backup = backup;

    auto bg = CCScale9Sprite::create("square02c_001.png");
    bg->setColor({ 25, 25, 25 });
    bg->setOpacity(90);
    bg->setScale(.5f);
    bg->setContentSize(m_obContentSize / bg->getScale());
    this->addChildAtPosition(bg, Anchor::Center);

    auto title = CCLabelBMFont::create(backup->getLevel()->m_levelName.c_str(), "bigFont.fnt");
    title->setAnchorPoint({ 0, .5f });
    title->setScale(.5f);
    this->addChildAtPosition(title, Anchor::Left, ccp(5, 7));

    auto time = CCLabelBMFont::create(fmt::format("{:%Y/%m/%d at %H:%M}", backup->getCreateTime()).c_str(), "goldFont.fnt");
    time->setAnchorPoint({ 0, .5f });
    time->setScale(.4f);
    this->addChildAtPosition(time, Anchor::Left, ccp(5, -7));

    auto menu = CCMenu::create();
    menu->setAnchorPoint({ 1, .5f });
    menu->setContentWidth(100);

    auto deleteSpr = CCSprite::createWithSpriteFrameName("GJ_trashBtn_001.png");
    auto deleteBtn = CCMenuItemSpriteExtra::create(
        deleteSpr, this, menu_selector(BackupItem::onDelete)
    );
    menu->addChild(deleteBtn);

    auto restoreSpr = CCSprite::createWithSpriteFrameName("GJ_undoBtn_001.png");
    auto restoreBtn = CCMenuItemSpriteExtra::create(
        restoreSpr, this, menu_selector(BackupItem::onRestore)
    );
    menu->addChild(restoreBtn);

    auto viewSpr = CircleButtonSprite::createWithSpriteFrameName(
        "eye-white.png"_spr, 1.f, CircleBaseColor::Green, CircleBaseSize::Small
    );
    auto viewBtn = CCMenuItemSpriteExtra::create(
        viewSpr, this, menu_selector(BackupItem::onView)
    );
    menu->addChild(viewBtn);

    if (backup->isAutomated()) {
        bg->setColor({ 30, 93, 156 });

        auto convertSpr = CCSprite::createWithSpriteFrameName("GJ_rotationControlBtn02_001.png");
        auto convertBtn = CCMenuItemSpriteExtra::create(
            convertSpr, this, menu_selector(BackupItem::onConvertAutomated)
        );
        menu->addChild(convertBtn);
    }

    menu->setLayout(
        RowLayout::create()
            ->setAxisReverse(true)
            ->setAxisAlignment(AxisAlignment::End)
            ->setDefaultScaleLimits(.1f, .55f)
    );
    this->addChildAtPosition(menu, Anchor::Right, ccp(-5, 0));

    return true;
}

void BackupItem::onConvertAutomated(CCObject*) {
    createQuickPopup(
        "Preserve Backup",
        "Do you want to <cj>preserve this automated backup</c>?\n"
        "By default, <cy>automated backups are deleted after three newer backups have been made</c>.\n"
        "Preserving the backup turns it into a normal backup, <cp>preventing it from being deleted automatically</c>.",
        "Cancel", "Preserve",
        [this](auto*, bool btn2) {
            if (btn2) {
                auto res = m_backup->preserveAutomated();
                if (!res) {
                    FLAlertLayer::create(
                        "Unable to Preserve Backup",
                        fmt::format("Unable to preserve backup: {}", res.unwrapErr()),
                        "OK"
                    )->show();
                }
                UpdateBackupListEvent().post();
            }
        }
    );
}
void BackupItem::onView(CCObject*) {
    auto scene = CCScene::create();
    scene->addChild(createViewOnlyEditor(m_backup->getLevel(), [level = m_backup->getOriginalLevel()]() {
        auto layer = EditLevelLayer::create(level);
        auto popup = BackupListPopup::create(level);
        popup->m_scene = layer;
        popup->m_noElasticity = true;
        popup->show();
        return static_cast<CCLayer*>(layer);
    }));
    CCDirector::get()->replaceScene(CCTransitionFade::create(.5f, scene));
}
void BackupItem::onRestore(CCObject*) {
    createQuickPopup(
        "Restore Backup",
        "Are you sure you want to <cy>restore this backup</c>?\n"
        "<co>This action will</c> <cr>override</c> <co>the current level!</c>\n"
        "<cj>Create a new backup first if you want to preserve changes!</c>",
        "Cancel", "Restore",
        [this](auto*, bool btn2) {
            if (btn2) {
                auto res = m_backup->restoreThis();
                if (!res) {
                    FLAlertLayer::create(
                        "Unable to Restore Backup",
                        fmt::format("Unable to restore backup: {}", res.unwrapErr()),
                        "OK"
                    )->show();
                }
                auto ev = UpdateBackupListEvent();
                ev.closeList = true;
                ev.post();
                FLAlertLayer::create("Backup Restored", "The backup has been restored.", "OK")->show();
            }
        }
    );
}
void BackupItem::onDelete(CCObject*) {
    createQuickPopup(
        "Delete Backup",
        "Are you sure you want to <cr>delete this backup</c>?\n"
        "<co>This action is irreversible!</c>",
        "Cancel", "Delete",
        [this](auto*, bool btn2) {
            if (btn2) {
                auto res = m_backup->deleteThis();
                if (!res) {
                    FLAlertLayer::create(
                        "Unable to Delete Backup",
                        fmt::format("Unable to delete backup: {}", res.unwrapErr()),
                        "OK"
                    )->show();
                }
                UpdateBackupListEvent().post();
            }
        }
    );
}

BackupItem* BackupItem::create(BackupPtr backup) {
    auto ret = new BackupItem();
    if (ret && ret->init(backup)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}
