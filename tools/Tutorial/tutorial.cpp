#include "tutorial.hpp"
#include "../CustomKeybinds/ContextPopup.hpp"
#include "../VisibilityTab/loadVisibilityTab.hpp"

static int g_nPage = 0;
bool g_bShowing = false;

bool operator==(CCPoint const& s1, CCPoint const& s2) {
    return s1.x == s2.x && s1.y == s2.y;
}

bool operator==(CCRect const& r1, CCRect const& r2) {
    return r1.origin == r2.origin && r1.size == r2.size;
}

class DarkOverlay : public CCNode {
    protected:
        CCRect m_obArea;

        void draw() override {
            auto winSize = CCDirector::sharedDirector()->getWinSize();
            ccDrawSolidRect(
                { 0, winSize.height },
                {
                    winSize.width,
                    this->m_obArea.origin.y +
                    this->m_obArea.size.height
                },
                { 0, 0, 0, .8f }
            );
            ccDrawSolidRect(
                { 0, this->m_obArea.origin.y },
                {
                    this->m_obArea.origin.x,
                    this->m_obArea.origin.y +
                    this->m_obArea.size.height
                },
                { 0, 0, 0, .8f }
            );
            ccDrawSolidRect(
                {
                    this->m_obArea.origin.x +
                    this->m_obArea.size.width,
                    this->m_obArea.origin.y
                },
                {
                    winSize.width,
                    this->m_obArea.origin.y +
                    this->m_obArea.size.height
                },
                { 0, 0, 0, .8f }
            );
            ccDrawSolidRect(
                { 0, 0 },
                {
                    winSize.width,
                    this->m_obArea.origin.y
                },
                { 0, 0, 0, .8f }
            );
        }

        bool init(CCRect const& area) {
            if (!CCNode::init())
                return false;
            
            this->m_obArea = area;
            this->setZOrder(105);

            return true;
        }
    
    public:
        void setArea(CCRect const& rect) {
            this->m_obArea = rect;
        }

        static DarkOverlay* create(CCRect const& area) {
            auto ret = new DarkOverlay;

            if (ret && ret->init(area)) {
                ret->autorelease();
                return ret;
            }

            CC_SAFE_DELETE(ret);
            return nullptr;
        }
};

enum PageType {
    kPageTypeArrow,
    kPageTypeAlert
};

struct TutorialPage {
    PageType type;
    CCNode* targetNode = nullptr;
    CCPoint targetPos;
    CCMenuItem* clickNode = nullptr;
    std::string text;
    CCSize size;
    ContextPopupDirection dir;
    const char* alertTitle;
    CCRect highlightArea = CCRectZero;
};

class TutorialPopup : public ContextPopup {
    protected:
        TutorialPage* m_pPage;

        void setup() override {
            auto area = TextArea::create(
                "bigFont.fnt", false, m_pPage->text,
                .3f, m_pPage->size.width * .8f, m_pPage->size.height * .1f,
                { .5f, .5f }
            );
            area->setPosition({
                area->getScaledContentSize().width / 4,
                area->getScaledContentSize().height / 2
            });
            this->addChild(area);
        }
        void onHide() override {
            showNextTutorialPage();
        }
    
    public:
        static TutorialPopup* create(TutorialPage* page) {
            auto ret = new TutorialPopup;

            if (
                ret &&
                (ret->m_pPage = page) &&
                ret->init(
                    page->targetNode ?
                    page->targetNode->getParent()->convertToWorldSpace(
                        page->targetNode->getPosition()
                    ) + page->targetPos :
                    page->targetPos,
                    page->size,
                    page->dir,
                    kContextPopupTypeBrown
                )
            ) {
                ret->autorelease();
                return ret;
            }

            CC_SAFE_DELETE(ret);
            return nullptr;
        }
};

TutorialPopup* g_pLastPopup = nullptr;
FLAlertLayer* g_pLastAlert = nullptr;
DarkOverlay* g_pOverlay = nullptr;

std::vector<std::function<TutorialPage()>> g_vPages {
    []() -> TutorialPage {
        auto winSize = CCDirector::sharedDirector()->getWinSize();
        auto ui = LevelEditorLayer::get()->getEditorUI();
        return {
            kPageTypeArrow,
            ui->getChildByTag(VIEWBUTTONBAR_TAG),
            { winSize.width / 2, 80.0f },
            as<CCMenuItem*>(ui->m_pBuildModeBtn->getParent()->getChildByTag(4)),
            "This is the <cr>view tab</c>. Here you can quickly toggle on/off visual "
            "elements, such as <cy>Preview Mode</c>, <cb>Grid</c> & <cg>Sawblade Rotation</c>.",
            { 150.0f, 100.0f },
            kContextPopupDirectionDown,
            nullptr,
            { 5.f, 5.f, winSize.width - 110.f, 80.0f }
        };
    },
};

class TutorialAlertDelegate : public FLAlertLayerProtocol {
    void FLAlert_Clicked(FLAlertLayer*, bool btn2) override {
        if (btn2) {
            showNextTutorialPage();
            g_bShowing = true;
        }

        // BetterEdit::setShownTutorial(true);
        
        delete this;
    }
};

bool showingTutorial() {
    return g_bShowing;
}

void askToShowTutorial() {
    auto d = new TutorialAlertDelegate;
    auto l = FLAlertLayer::create(
        d, "Welcome to BetterEdit",
        "Nah", "Sure!",
        "It looks like you are new to <cy>BetterEdit</c>!\n\n "
        "Would you like a <cy>tutorial</c> for the new features?"
    );
    l->m_pTargetLayer = LevelEditorLayer::get()->getEditorUI();
    l->show();
    g_nPage = 0;
}

void showNextTutorialPage() {
    if (g_pLastPopup) {
        g_pLastPopup->hide();
        g_pLastPopup = nullptr;
    }
    if (g_pLastAlert) {
        g_pLastAlert->removeFromParent();
        g_pLastAlert = nullptr;
    }
    
    if (static_cast<int>(g_vPages.size()) <= g_nPage) {
        g_bShowing = false;
        if (g_pOverlay) {
            g_pOverlay->removeFromParent();
            g_pOverlay = nullptr;
        }
        return;
    }

    auto page = g_vPages.at(g_nPage)();

    if (page.clickNode)
        page.clickNode->activate();
    
    CCRect hlSize;
    
    if (page.targetNode && page.highlightArea == CCRectZero) {
        auto size = page.targetNode->getScaledContentSize();
        hlSize = {
            page.targetNode->getPositionX() - size.width / 2,
            page.targetNode->getPositionY() - size.height / 2,
            size.width,
            size.height
        };
    } else {
        hlSize = {
            page.highlightArea.origin.x,
            page.highlightArea.origin.y,
            page.highlightArea.size.width,
            page.highlightArea.size.height
        };
    }

    if (!g_pOverlay) {
        g_pOverlay = DarkOverlay::create(hlSize);
        LevelEditorLayer::get()->addChild(g_pOverlay);
    } else {
        g_pOverlay->setArea(hlSize);
    }
    
    if (page.type == kPageTypeAlert) {
        auto popup = FLAlertLayer::create(
            nullptr, page.alertTitle, "OK", nullptr,
            page.text
        );
        popup->show();
        g_pLastAlert = popup;
    }

    if (page.type == kPageTypeArrow) {
        auto popup = TutorialPopup::create(&page);
        popup->show();
        g_pLastPopup = popup;
    }

    g_nPage++;
}
