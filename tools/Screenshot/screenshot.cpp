#include "screenshot.hpp"
#include <GdiPlus.h>

#pragma comment(lib, "GdiPlus")

float g_fQuality = 1.f;
bool g_bIncludeGrid = false;
size_t g_nScreenshotTitle = 0;

CCSize operator*=(CCSize & size, float mul) {
    size.width *= mul;
    size.height *= mul;
    return size;
}

CCRect operator*=(CCRect & size, float mul) {
    size.origin.x *= mul;
    size.origin.y *= mul;
    size.size.width *= mul;
    size.size.height *= mul;
    return size;
}

CCPoint operator/=(CCPoint & pos, float mul) {
    pos.x /= mul;
    pos.y /= mul;
    return pos;
}

CCPoint operator*=(CCPoint & pos, float mul) {
    pos.x *= mul;
    pos.y *= mul;
    return pos;
}

CCPoint operator+=(CCPoint & pos, CCPoint const& add) {
    pos.x += add.x;
    pos.y += add.y;
    return pos;
}

// from https://stackoverflow.com/questions/39200677/how-to-copy-a-picture-from-disk-into-the-clipboard-with-win32
bool copyimage(const wchar_t* filename)
{
    bool result = false;
    Gdiplus::Bitmap *gdibmp = Gdiplus::Bitmap::FromFile(filename);
    if (gdibmp)
    {
        HBITMAP hbitmap;
        gdibmp->GetHBITMAP(0, &hbitmap);
        if (OpenClipboard(NULL))
        {
            EmptyClipboard();
            DIBSECTION ds;
            if (GetObject(hbitmap, sizeof(DIBSECTION), &ds))
            {
                HDC hdc = GetDC(HWND_DESKTOP);
                //create compatible bitmap (get DDB from DIB)
                HBITMAP hbitmap_ddb = CreateDIBitmap(hdc, &ds.dsBmih, CBM_INIT,
                    ds.dsBm.bmBits, (BITMAPINFO*)&ds.dsBmih, DIB_RGB_COLORS);
                ReleaseDC(HWND_DESKTOP, hdc);
                SetClipboardData(CF_BITMAP, hbitmap_ddb);
                DeleteObject(hbitmap_ddb);
                result = true;
            }
            CloseClipboard();
        }

        //cleanup:
        DeleteObject(hbitmap);  
        delete gdibmp;              
    }
    return result;
}

void limitSpriteSize(CCSprite* spr, CCSize const& size, float def, float min);

bool ScreenFlash::init(ccColor4B const& col, float time) {
    if (!CCLayerColor::initWithColor(col))
        return false;
    
    this->runAction(CCSequence::create(
        CCFadeTo::create(time, 0),
        CCCallFunc::create(this, callfunc_selector(ScreenFlash::removeFromParent)),
        nullptr
    ));

    return true;
}

ScreenFlash* ScreenFlash::create(ccColor4B const& col, float time) {
    auto ret = new ScreenFlash;

    if (ret && ret->init(col, time)) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}

ScreenFlash* ScreenFlash::show(ccColor4B const& col, float time) {
    auto ret = ScreenFlash::create(col, time);
    CCDirector::sharedDirector()->getRunningScene()->addChild(ret, 99999);
    return ret;
}


void ScreenShotPopup::setup() {
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    this->m_pTexture->retain();
    
    this->m_pShot = this->m_pTexture->getSprite();
    this->m_pShot->setPosition(winSize / 2);
    limitSpriteSize(
        this->m_pShot,
        { this->m_pLrSize.width - 55.0f, this->m_pLrSize.height - 85.0f },
        1.0f, .01f
    );
    this->m_pShot->setFlipY(true);
    this->m_pLayer->addChild(this->m_pShot);
    
    this->m_pButtonMenu->addChild(
        CCNodeConstructor<CCMenuItemSpriteExtra*>()
            .fromNode(CCMenuItemSpriteExtra::create(
                CCNodeConstructor<ButtonSprite*>()
                    .fromNode(ButtonSprite::create(
                        "Copy", 0, 0, "goldFont.fnt", "GJ_button_01.png", 0, .8f
                    ))
                    .scale(.8f)
                    .done(),
                this,
                (SEL_MenuHandler)&ScreenShotPopup::onCopy
            ))
            .move(-30.0f, - this->m_pLrSize.height / 2 + 25.0f)
            .done()
    );
    this->m_pButtonMenu->addChild(
        CCNodeConstructor<CCMenuItemSpriteExtra*>()
            .fromNode(CCMenuItemSpriteExtra::create(
                CCNodeConstructor<ButtonSprite*>()
                    .fromNode(ButtonSprite::create(
                        "Save", 0, 0, "goldFont.fnt", "GJ_button_01.png", 0, .8f
                    ))
                    .scale(.8f)
                    .done(),
                this,
                (SEL_MenuHandler)&ScreenShotPopup::onSave
            ))
            .move(30.0f, - this->m_pLrSize.height / 2 + 25.0f)
            .done()
    );
}

void ScreenShotPopup::onCopy(CCObject*) {
    if (!this->m_pTexture->saveToFile("be_temp_screenshot.jpg")) {
        return FLAlertLayer::create(
            nullptr, "Unable to Copy",
            "OK", nullptr,
            "<cr>Unable to save temp file</c>"
        )->show();
    }
    
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    if (!copyimage(L"be_temp_screenshot.jpg")) {
        return FLAlertLayer::create(
            nullptr, "Unable to Copy",
            "OK", nullptr,
            "<cr>Unable to copy image to clipboard</c>"
        )->show();
    }

    Gdiplus::GdiplusShutdown(gdiplusToken);
}

void ScreenShotPopup::onSave(CCObject*) {
    if (this->m_pTexture->saveToFile("screenshot.jpg")) {
        FLAlertLayer::create(
            nullptr, "Saved Screenshot",
            "OK", nullptr,
            "Succesfully saved screenshot to <cy>screenshot.jpg</c>"
        )->show();
    } else {
        FLAlertLayer::create(
            nullptr, "Error Saving",
            "OK", nullptr,
            "Unable to save screenshot :("
        )->show();
    }
}

ScreenShotPopup::~ScreenShotPopup() {
    this->m_pTexture->release();
}

ScreenShotPopup* ScreenShotPopup::create(CCRenderTexture* tex) {
    auto ret = new ScreenShotPopup;

    auto titles = std::vector<const char*> {
        "Wow! what a terrible pic",
        "Yeah I said it. It looks stupid.",
        "Ok I'm sorry for calling that last one\n"
        "terrible, that was so rude of me",
        "I've just been going through a rough\n"
        "time lately, y'know",
        "Eh...",
        "Nice shot!",
        "Nice shot!",
        "Nice shot!",
        "Nice shot!",
        "N I C E   C O C K",
        "Nice shot!",
    };
    auto title = titles.at(g_nScreenshotTitle);
    if (g_nScreenshotTitle < titles.size() - 1)
        g_nScreenshotTitle++;
    
    if (BetterEdit::getNoEasterEggs())
        title = "Screenshot Preview";

    if (
        ret &&
        ((ret->m_pTexture = tex) || true) &&
    ret->init(
        340.0f, 240.0f,
        "GJ_square01.png", title
    )) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}


bool ScreenShotOverlay::init() {
    if (!CCNode::init())
        return false;

    this->m_fQuality = g_fQuality;
    this->m_bIncludeGrid = g_bIncludeGrid;
    
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    this->setContentSize(winSize);
    this->setSuperMouseHitSize(winSize);
    this->setSuperMouseHitOffset(winSize / 2);
    SuperMouseManager::get()->captureMouse(this);

    this->m_pLayer = CCLayer::create();
    this->addChild(this->m_pLayer);

    this->m_pInfoLabel = CCLabelBMFont::create("", "bigFont.fnt", 500.0f, kCCTextAlignmentCenter);
    this->m_pInfoLabel->setPosition(
        this->getContentSize().width / 2,
        this->getContentSize().height - 40.0f
    );
    this->m_pInfoLabel->setScale(.35f);
    this->m_pLayer->addChild(this->m_pInfoLabel);

    this->m_pExtendedInfoLabel = CCLabelBMFont::create("", "bigFont.fnt", 500.0f, kCCTextAlignmentCenter);
    this->m_pExtendedInfoLabel->setPosition(
        this->getContentSize().width / 2,
        60.0f
    );
    this->m_pExtendedInfoLabel->setScale(.25f);
    this->m_pLayer->addChild(this->m_pExtendedInfoLabel);

    this->m_pOptionsLabel = CCLabelBMFont::create("", "bigFont.fnt", 500.0f, kCCTextAlignmentRight);
    this->m_pOptionsLabel->setAnchorPoint({ 1.f, .5f });
    this->m_pOptionsLabel->setPosition(winSize.width - 20.0f, 40.0f);
    this->m_pOptionsLabel->setScale(.35f);
    this->m_pLayer->addChild(this->m_pOptionsLabel);

    auto qualityInfo = CCLabelBMFont::create(
        "Increase Quality: H\n"
        "Decrease Quality: G\n",
        "bigFont.fnt"
    );
    qualityInfo->setAnchorPoint({ .0f, .5f });
    qualityInfo->setPosition(20.0f, 40.0f);
    qualityInfo->setScale(.35f);
    this->m_pLayer->addChild(qualityInfo);

    this->m_pQualityLabel = CCLabelBMFont::create("Quality: x", "bigFont.fnt");
    this->m_pQualityLabel->setAnchorPoint({ .0f, .5f });
    this->m_pQualityLabel->setPosition(20.0f, 20.0f);
    this->m_pQualityLabel->setScale(.35f);
    this->m_pLayer->addChild(this->m_pQualityLabel);

    this->updateLabels();

    this->setZOrder(99999);

    return true;
}

void ScreenShotOverlay::updateLabels() {
    auto qs = ("Quality: " + BetterEdit::formatToString(this->m_fQuality) + "x");
    if (this->m_fQuality > 4.0f) {
        qs += " (Warning: Very High - May Crash!)";
    }
    this->m_pQualityLabel->setString(qs.c_str());

    this->m_pOptionsLabel->setString((
        "[1] Include Grid: " + BetterEdit::formatToString(this->m_bIncludeGrid)
    ).c_str());

    if (this->m_bExtendedMode) {
        this->m_pInfoLabel->setString(
            this->m_bSelecting ?
            "Click to Select End Point  \nRight-click to move" :
            "Click to Select Start Point\nRight-click to move"
        );
        this->m_pExtendedInfoLabel->setString("Press N to take Normal Screenshot");
    } else {
        this->m_pInfoLabel->setString("Select area to screenshot");
        this->m_pExtendedInfoLabel->setString(
            // "Press E to take Extended Screenshot\n"
            "Press F to take Fullscreen Screenshot"
        );
    }
}

void ScreenShotOverlay::mouseMoveSuper(CCPoint const& pos) {
    if (this->m_bMoving) {
        auto diff = pos - this->m_obMoveLastPos;
        this->m_pUI->moveGameLayer(diff);
        this->m_obMoveLastPos = pos;
        if (this->m_bExtendedMode) {
            this->m_obStartPos += diff;
            this->m_obEndPos += diff;
        }
        return;
    }
    if (
        this->m_bSuperMouseDown &&
        this->m_bSelecting
    ) {
        this->m_obEndPos = pos;
    }
}

bool ScreenShotOverlay::mouseDownSuper(MouseButton btn, CCPoint const& pos) {
    if (btn == kMouseButtonRight) {
        this->m_bMoving = true;
        this->m_obMoveLastPos = pos;
    }
    if (btn == kMouseButtonLeft) {
        if (!this->m_bExtendedMode) {
            this->m_bSelecting = true;
            this->m_obStartPos = pos;
            this->m_pLayer->setVisible(false);
        }
    }
    this->updateLabels();
    return true;
}

bool ScreenShotOverlay::mouseUpSuper(MouseButton btn, CCPoint const& pos) {
    if (btn == kMouseButtonRight) {
        this->m_bMoving = false;
    }
    if (btn == kMouseButtonLeft) {
        if (this->m_bExtendedMode) {
            if (this->m_bSelecting) {
                this->screenshot();
                return true;
            } else {
                this->m_bSelecting = true;
                this->m_obStartPos = pos;
                this->m_obEndPos = pos;
            }
        } else {
            this->screenshot();
            return true;
        }
    }
    this->updateLabels();
    return true;
}

bool ScreenShotOverlay::mouseScrollSuper(float y, float x) {
    this->keyDownSuper(y > 0.0f ? KEY_G : KEY_H);
    return true;
}

bool ScreenShotOverlay::keyDownSuper(enumKeyCodes key) {
    int add = 0;
    switch (key) {
        case KEY_G: add = -1; break;
        case KEY_H: add = 1; break;
        case KEY_One:
            this->m_bIncludeGrid = !this->m_bIncludeGrid;
            this->updateLabels();
            return true;
        // case KEY_E:
        //     m_bExtendedMode = true;
        //     this->updateLabels();
        //     return true;
        case KEY_N:
            if (m_bExtendedMode) {
                m_bExtendedMode = false;
                m_bSelecting = false;
            }
            this->updateLabels();
            return true;
        case KEY_F:
            this->m_obStartPos = CCPointZero;
            this->m_obEndPos = CCDirector::sharedDirector()->getWinSize();
            this->screenshot();
            return true;
        case KEY_Escape:
            this->removeFromParent();
            return true;
        default:
            return true;
    }

    std::vector<float> q = { .5f, .75f, 1.f, 1.5f, 2.f, 3.f, 4.f, 6.f, 8.f, 12.f, };

    auto val = 0;

    auto ix = 0;
    for (auto const& f : q)
        if (f == this->m_fQuality) {
            val = ix;
        } else {
            ix++;
        }
    
    val += add;

    if (val < 0) val = 0;
    if (val > static_cast<int>(q.size() - 1)) val = q.size() - 1;

    this->m_fQuality = q[val];

    this->updateLabels();

    return true;
}

void ScreenShotOverlay::screenshot() {
    this->setVisible(false);
    if (!this->m_bIncludeGrid)
        this->m_pUI->m_pEditorLayer->m_pDrawGridLayer->setVisible(false);
    
    auto area = CCRect {
        fabsf(min(this->m_obStartPos.x, this->m_obEndPos.x)),
        fabsf(min(this->m_obStartPos.y, this->m_obEndPos.y)),
        fabsf(this->m_obEndPos.x - this->m_obStartPos.x),
        fabsf(this->m_obEndPos.y - this->m_obStartPos.y)
    };

    // no ridiculously small screenshots allowed
    if (area.size.width * area.size.height < 250.0f) {
        this->m_pLayer->setVisible(true);
        this->m_bSelecting = false;
        this->m_pUI->m_pEditorLayer->m_pDrawGridLayer->setVisible(true);
        return this->setVisible(true);
    }

    area *= this->m_fQuality;

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    auto tex = CCRenderTexture::create(
        static_cast<int>(area.size.width),
        static_cast<int>(area.size.height)
    );
    tex->begin();
    auto scene = CCDirector::sharedDirector()->getRunningScene();
    scene->setScale(this->m_fQuality);

    // auto xCount = static_cast<int>(ceilf(
    //     (area.origin.x + area.size.width) /
    //     (winSize.width * this->m_fQuality)
    // ));
    // auto yCount = static_cast<int>(ceilf(
    //     (area.origin.y + area.size.height) /
    //     (winSize.height * this->m_fQuality)
    // ));

    // for (int i = 0; i < yCount; i++) {
    //     for (int j = 0; j < xCount; j++) {
    //         scene->setPosition(-area.origin + 
    //             (scene->getScaledContentSize() - scene->getContentSize()) / 2
    //             + CCPoint { winSize.width * j, winSize.height * i }
    //         );
    //         scene->visit();
    //     }
    // }
    
    scene->setPosition(-area.origin + 
        (scene->getScaledContentSize() - scene->getContentSize()) / 2
    );
    scene->visit();
    scene->setScale(1);
    scene->setPosition(0, 0);
    tex->end();

    ScreenShotPopup::create(tex)->show();

    ScreenFlash::show({ 255, 255, 255, 255 }, 1.0f);

    this->m_pUI->m_pEditorLayer->m_pDrawGridLayer->setVisible(true);

    this->removeFromParent();
}

void ScreenShotOverlay::draw() {
    auto area = CCRect {
        min(this->m_obStartPos.x, this->m_obEndPos.x),
        min(this->m_obStartPos.y, this->m_obEndPos.y),
        fabsf(this->m_obEndPos.x - this->m_obStartPos.x),
        fabsf(this->m_obEndPos.y - this->m_obStartPos.y)
    };

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    if (!this->m_bSelecting) {

        ccDrawSolidRect(
            { 0, 0, },
            {
                winSize.width,
                winSize.height
            },
            { 0, 0, 0, .8f }
        );

    } else {
        ccDrawSolidRect(
            { 0, winSize.height },
            {
                winSize.width,
                area.origin.y +
                area.size.height
            },
            { 0, 0, 0, .8f }
        );
        ccDrawSolidRect(
            { 0, area.origin.y },
            {
                area.origin.x,
                area.origin.y +
                area.size.height
            },
            { 0, 0, 0, .8f }
        );
        ccDrawSolidRect(
            {
                area.origin.x +
                area.size.width,
                area.origin.y
            },
            {
                winSize.width,
                area.origin.y +
                area.size.height
            },
            { 0, 0, 0, .8f }
        );
        ccDrawSolidRect(
            { 0, 0 },
            {
                winSize.width,
                area.origin.y
            },
            { 0, 0, 0, .8f }
        );

        ccDrawColor4B(200, 255, 200, 180);
        ccDrawRect(this->m_obStartPos, this->m_obEndPos);

        if (this->m_bExtendedMode) {
            ccPointSize(5.0f);
            ccDrawPoint(this->m_obStartPos);
        }
    }
}

void ScreenShotOverlay::setEditorUI(EditorUI* ui) {
    this->m_pUI = ui;
}

ScreenShotOverlay::~ScreenShotOverlay() {
    SuperMouseManager::get()->releaseCapture(this);

    g_fQuality = this->m_fQuality;
    g_bIncludeGrid = this->m_bIncludeGrid;

    if (this->m_pUI)
        this->m_pUI->setVisible(true);
}

ScreenShotOverlay* ScreenShotOverlay::create() {
    auto ret = new ScreenShotOverlay;

    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}

void takeScreenshot(EditorUI* ui) {
    auto overlay = ScreenShotOverlay::create();
    overlay->setEditorUI(ui);
    ui->m_pEditorLayer->addChild(overlay);
    ui->setVisible(false);
}
