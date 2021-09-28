#include <GDMake.h>
#include <ZipUtils.h>

using namespace gdmake;

GDMAKE_HOOK(ProfilePage::init, "_ZN11ProfilePage4initEib")
bool __fastcall ProfilePage_init(gd::ProfilePage* self, edx_t edx, int accountID, bool idk) {
    if (!GDMAKE_ORIG(self, edx, accountID, idk))
        return false;
    
    if (accountID == 104257) {
        auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

        auto discordText = cocos2d::CCLabelBMFont::create("My Discord is HJfod#1795 :)", "chatFont.fnt");

        discordText->setScale(.6f);
        discordText->setColor({ 90, 255, 200 });
        discordText->setPosition(winSize.width / 2, winSize.height / 2 + 8.0f);

        self->getLayer()->addChild(discordText);
    }

    return true;
}

GDMAKE_HOOK(0x5f3d0, "_ZN11CommentCell15loadFromCommentEP9GJComment")
void __fastcall CommentCell_loadFromComment(gd::CommentCell* self, edx_t edx, gd::GJComment* comment) {
    GDMAKE_ORIG_V(self, edx, comment);

    if (comment->m_nCommentID == 0) {
        auto layer = extra::getChild<cocos2d::CCNode*>(self, 1);
        if (!layer) return;

        auto textarea = extra::getChild<gd::TextArea*>(layer, 3);
        if (!textarea) return;

        textarea->colorAllCharactersTo({ 140, 255, 185 });
    }
}

GDMAKE_HOOK(0x213830, "_ZN11ProfilePage20setupCommentsBrowserEPN7cocos2d7CCArrayE")
void __fastcall ProfilePage_setupCommentsBrowser(gd::ProfilePage* self, edx_t edx, cocos2d::CCArray* comments) {
    if (comments && self->m_nAccountID == 104257) {
        CCARRAY_FOREACH_B(comments, c)
            if (c->getTag() == 6969)
                goto skip_new_comment;

        auto dict = cocos2d::CCDictionary::create();

        dict->setObject(cocos2d::CCString::create("0"), "4");
        dict->setObject(cocos2d::CCString::create("0"), "6");
        dict->setObject(cocos2d::CCString::create("You're epic"), "9");
        dict->setObject(cocos2d::CCString::create(
            cocos2d::ZipUtils::base64URLEncode(
                "Hey! You're using BetterEdit! That's awesome! :)"
            )
        ), "2");

        auto comment = gd::GJComment::create(dict);
        comment->setTag(6969);
        comment->m_nAccountID = self->m_nAccountID;

        comments->insertObject(comment, 0u);
    }
skip_new_comment:

    GDMAKE_ORIG_V(self, edx, comments);
}
