#include "LayerViewPopup.hpp"
#include "LayerEditPopup.hpp"
#include "LayerSettingsPopup.hpp"

int g_nPage = 0;
LayerViewPopup::ViewType g_nViewType = LayerViewPopup::kViewTypeList;
LayerViewPopup::GridView g_nGridView = LayerViewPopup::kGridViewNormal;
bool g_bHideEmptyLayers = false;

#define BOOL_TO_BG(var) \
    var ? "GJ_button_02.png" : "GJ_button_01.png"
#define VIEWTYPE_TO_BG(type) \
    BOOL_TO_BG(this->m_nViewType == kViewType##type)
#define GRIDVIEW_TO_BG(type) \
    BOOL_TO_BG(this->m_nGridView == kGridView##type)

#define GDML_FILE(...)

static constexpr const cocos2d::ccColor3B listBGLight { 142, 68, 28 };
static constexpr const cocos2d::ccColor3B listBGDark  { 114, 55, 22 };

int LayerViewPopup::maxItemsPerPage() {
    switch (this->m_nViewType) {
        case kViewTypeList: default:
            return list_max_item_count;
        case kViewTypeGrid:
            return grid_max_item_count;
    }
}

int LayerViewPopup::countPages() {
    if (this->m_bHideEmptyLayers)
        return this->m_mObjectsPerLayer.size() / maxItemsPerPage();
    
    return LayerManager::max_layer_num / maxItemsPerPage();
}

int LayerViewPopup::firstItemInPage() {
    if (this->m_bHideEmptyLayers) {
        auto max = maxItemsPerPage() * this->m_nCurrentPage;
        if (static_cast<int>(this->m_mObjectsPerLayer.size()) < max)
            return 0;
        
        auto it = this->m_mObjectsPerLayer.begin();
        std::advance(it, max);
        return it->first;
    }

    return this->m_nCurrentPage * maxItemsPerPage();
}

void LayerViewPopup::countObjectsPerLayer() {
    auto objs = LevelEditorLayer::get()->getAllObjects();

    this->m_mObjectsPerLayer = {};

    CCARRAY_FOREACH_B_TYPE(objs, obj, GameObject) {
        if (obj->m_nEditorLayer || !obj->m_nEditorLayer2)
            this->m_mObjectsPerLayer[obj->m_nEditorLayer]++;
        if (obj->m_nEditorLayer2)
            this->m_mObjectsPerLayer[obj->m_nEditorLayer2]++;
    }
}

int LayerViewPopup::getObjectsInLayer(int number) {
    if (m_mObjectsPerLayer.count(number))
        return m_mObjectsPerLayer[number];
    
    return 0;
}

void LayerViewPopup::setup() {
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    this->countObjectsPerLayer();

    this->m_bNoElasticity = true;
    this->m_nCurrentPage = g_nPage;
    this->m_nViewType = g_nViewType;
    this->m_nGridView = g_nGridView;
    this->m_bHideEmptyLayers = g_bHideEmptyLayers;

    { // arrows
        this->m_pButtonMenu->addChild(CCNodeConstructor<CCMenuItemSpriteExtra*>()
            .fromNode(CCMenuItemSpriteExtra::create(
                CCNodeConstructor()
                    .fromFrameName("GJ_arrow_01_001.png")
                    .scale(.8f)
                    .done(),
                this,
                (SEL_MenuHandler)&LayerViewPopup::onPage
            ))
            .udata(-1)
            .save(&m_pPrevPageBtn)
            .move(- s_fItemWidth / 2 - 25.0f, 0.0f)
            .done()
        );
        this->m_pButtonMenu->addChild(CCNodeConstructor<CCMenuItemSpriteExtra*>()
            .fromNode(CCMenuItemSpriteExtra::create(
                CCNodeConstructor()
                    .fromFrameName("GJ_arrow_01_001.png")
                    .scale(.8f)
                    .flipX()
                    .done(),
                this,
                (SEL_MenuHandler)&LayerViewPopup::onPage
            ))
            .udata(1)
            .save(&m_pNextPageBtn)
            .move(s_fItemWidth / 2 + 25.0f, 0.0f)
            .done()
        );
    }
    { // lock all
        this->m_pButtonMenu->addChild(CCNodeConstructor<CCMenuItemSpriteExtra*>()
            .fromNode(CCMenuItemSpriteExtra::create(
                CCNodeConstructor<ButtonSprite*>()
                    .fromNode(ButtonSprite::create(
                        "Lock All", 80, true, "bigFont.fnt", "GJ_button_04.png", 0, .9f
                    ))
                    .scale(.55f)
                    .done(),
                this,
                (SEL_MenuHandler)&LayerViewPopup::onLockAll
            ))
            .udata(true)
            .move(this->m_pLrSize.width / 2 - 40.0f, -this->m_pLrSize.height / 2 + 24.0f)
            .done()
        );
        this->m_pButtonMenu->addChild(CCNodeConstructor<CCMenuItemSpriteExtra*>()
            .fromNode(CCMenuItemSpriteExtra::create(
                CCNodeConstructor<ButtonSprite*>()
                    .fromNode(ButtonSprite::create(
                        "Unlock All", 80, true, "bigFont.fnt", "GJ_button_04.png", 0, .9f
                    ))
                    .scale(.55f)
                    .done(),
                this,
                (SEL_MenuHandler)&LayerViewPopup::onLockAll
            ))
            .udata(false)
            .move(this->m_pLrSize.width / 2 - 40.0f, -this->m_pLrSize.height / 2 + 50.0f)
            .done()  
        );
    }
    { // show all
        this->m_pButtonMenu->addChild(CCNodeConstructor<CCMenuItemSpriteExtra*>()
            .fromNode(CCMenuItemSpriteExtra::create(
                CCNodeConstructor<ButtonSprite*>()
                    .fromNode(ButtonSprite::create(
                        "Hide All", 80, true, "bigFont.fnt", "GJ_button_04.png", 0, .9f
                    ))
                    .scale(.55f)
                    .done(),
                this,
                (SEL_MenuHandler)&LayerViewPopup::onShowAll
            ))
            .udata(false)
            .move(- this->m_pLrSize.width / 2 + 40.0f, -this->m_pLrSize.height / 2 + 24.0f)
            .done()
        );
        this->m_pButtonMenu->addChild(CCNodeConstructor<CCMenuItemSpriteExtra*>()
            .fromNode(CCMenuItemSpriteExtra::create(
                CCNodeConstructor<ButtonSprite*>()
                    .fromNode(ButtonSprite::create(
                        "Show All", 80, true, "bigFont.fnt", "GJ_button_04.png", 0, .9f
                    ))
                    .scale(.55f)
                    .done(),
                this,
                (SEL_MenuHandler)&LayerViewPopup::onShowAll
            ))
            .udata(true)
            .move(- this->m_pLrSize.width / 2 + 40.0f, -this->m_pLrSize.height / 2 + 50.0f)
            .done()
        );
    }
    { // toggle, page number, options
        this->m_pButtonMenu->addChild(CCNodeConstructor<CCMenuItemToggler*>()
            .fromNode(CCMenuItemToggler::createWithStandardSprites(
                this,
                (SEL_MenuHandler)&LayerViewPopup::onToggler
            ))
            .udata(&LayerManager::get()->getLevel()->m_bMultipleVisible)
            .exec([](CCMenuItemToggler* t) -> void {
                t->toggle(LayerManager::get()->getLevel()->m_bMultipleVisible);
            })
            .move(this->m_pLrSize.width / 2 - 40.0f, -this->m_pLrSize.height / 2 + 90.0f)
            .done()
        );
        this->m_pLayer->addChild(
            CCNodeConstructor<CCLabelBMFont*>()
            .fromText("1", "goldFont.fnt")
            .move(winSize / 2 + this->m_pLrSize / 2 - CCPoint { 30.0f, 20.0f })
            .scale(.7f)
            .visible(false)
            .save(&m_pPageLabel)
            .done()
        );
        this->m_pButtonMenu->addChild(
            CCNodeConstructor<CCMenuItemSpriteExtra*>()
                .fromNode(CCMenuItemSpriteExtra::create(
                    CCNodeConstructor()
                        .fromFrameName("GJ_optionsBtn_001.png")
                        .scale(.5f)
                        .done(),
                    this,
                    (SEL_MenuHandler)&LayerViewPopup::onGlobalSettings
                ))
                .move(this->m_pLrSize.width / 2 - 30.0f, this->m_pLrSize.height / 2 - 30.0f)
                .done()
        );
    }
    { // view type
        this->m_pButtonMenu->addChild(CCNodeConstructor<CCMenuItemSpriteExtra*>()
            .fromNode(CCMenuItemSpriteExtra::create(
                CCNodeConstructor<ButtonSprite*>()
                    .fromNode(ButtonSprite::create(
                        createBESprite("BE_grid-icon.png"),
                        0x20, true, 1.0f, true, VIEWTYPE_TO_BG(Grid),
                        true, 32.0f
                    ))
                    .exec([](ButtonSprite* s) -> void {
                        s->m_obSpritePosition = CCPoint { 0.0f, -1.0f };
                        s->updateSpriteBGSize(s->m_obSpritePosition);
                    })
                    .scale(.6f)
                    .save(&m_pViewGridBtn)
                    .done(),
                this,
                (SEL_MenuHandler)&LayerViewPopup::onViewType
            ))
            .udata(kViewTypeGrid)
            .move(- this->m_pLrSize.width / 2 + 50.0f, this->m_pLrSize.height / 2 - 22.5f)
            .done()
        );
        this->m_pButtonMenu->addChild(CCNodeConstructor<CCMenuItemSpriteExtra*>()
            .fromNode(CCMenuItemSpriteExtra::create(
                CCNodeConstructor<ButtonSprite*>()
                    .fromNode(ButtonSprite::create(
                        CCSprite::createWithSpriteFrameName("GJ_extendedIcon_001.png"),
                        0x20, true, 1.0f, true, VIEWTYPE_TO_BG(List),
                        true, 32.0f
                    ))
                    .exec([](ButtonSprite* s) -> void {
                        s->m_obSpritePosition = CCPoint { 0.0f, -1.0f };
                        s->updateSpriteBGSize(s->m_obSpritePosition);
                    })
                    .scale(.6f)
                    .save(&m_pViewListBtn)
                    .done(),
                this,
                (SEL_MenuHandler)&LayerViewPopup::onViewType
            ))
            .udata(kViewTypeList)
            .move(- this->m_pLrSize.width / 2 + 80.0f, this->m_pLrSize.height / 2 - 22.5f)
            .done()
        );
        this->m_pButtonMenu->addChild(CCNodeConstructor<CCMenuItemSpriteExtra*>()
            .fromNode(CCMenuItemSpriteExtra::create(
                CCNodeConstructor<ButtonSprite*>()
                    .fromNode(ButtonSprite::create(
                        createBESprite("BE_0_objs.png"),
                        0x20, true, 1.0f, true, BOOL_TO_BG(m_bHideEmptyLayers),
                        false, 32.0f
                    ))
                    .exec([](ButtonSprite* s) -> void {
                        s->m_obSpritePosition = CCPoint { 0.0f, -1.0f };
                        s->updateSpriteBGSize(s->m_obSpritePosition);
                    })
                    .scale(.6f)
                    .save(&m_pViewEmptyBtn)
                    .done(),
                this,
                (SEL_MenuHandler)&LayerViewPopup::onToggleEmpty
            ))
            .move(- this->m_pLrSize.width / 2 + 110.0f, this->m_pLrSize.height / 2 - 22.5f)
            .done()
        );
    }
    { // grid view
        this->m_pButtonMenu->addChild(CCNodeConstructor<CCMenuItemSpriteExtra*>()
            .fromNode(CCMenuItemSpriteExtra::create(
                CCNodeConstructor<ButtonSprite*>()
                    .fromNode(ButtonSprite::create(
                        CCLabelBMFont::create("#", "bigFont.fnt"),
                        0x20, true, 1.0f, false, GRIDVIEW_TO_BG(Normal),
                        true, 32.0f
                    ))
                    .scale(.5f)
                    .exec([](ButtonSprite* s) -> void {
                        s->m_obSpritePosition = CCPoint { 0.0f, -1.0f };
                        s->updateSpriteBGSize(s->m_obSpritePosition);
                    })
                    .done(),
                this,
                (SEL_MenuHandler)&LayerViewPopup::onGridView
            ))
            .move(- this->m_pLrSize.width / 2 + 50.0f, this->m_pLrSize.height / 2 - 47.5f)
            .exec([this](CCMenuItemSpriteExtra* s) -> void {
                this->m_vGridViewBtns.push_back(s);
            })
            .udata(kGridViewNormal)
            .done()
        );
        this->m_pButtonMenu->addChild(CCNodeConstructor<CCMenuItemSpriteExtra*>()
            .fromNode(CCMenuItemSpriteExtra::create(
                CCNodeConstructor<ButtonSprite*>()
                    .fromNode(ButtonSprite::create(
                        CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png"),
                        0x20, true, 1.0f, false, GRIDVIEW_TO_BG(Alt),
                        true, 32.0f
                    ))
                    .scale(.5f)
                    .exec([](ButtonSprite* s) -> void {
                        s->m_obSpritePosition = CCPoint { 0.0f, -1.0f };
                        s->updateSpriteBGSize(s->m_obSpritePosition);
                    })
                    .done(),
                this,
                (SEL_MenuHandler)&LayerViewPopup::onGridView
            ))
            .move(- this->m_pLrSize.width / 2 + 50.0f, this->m_pLrSize.height / 2 - 70.0f)
            .exec([this](CCMenuItemSpriteExtra* s) -> void {
                this->m_vGridViewBtns.push_back(s);
            })
            .udata(kGridViewAlt)
            .done()
        );
        this->m_pButtonMenu->addChild(CCNodeConstructor<CCMenuItemSpriteExtra*>()
            .fromNode(CCMenuItemSpriteExtra::create(
                CCNodeConstructor<ButtonSprite*>()
                    .fromNode(ButtonSprite::create(
                        CCSprite::createWithSpriteFrameName("GJ_lock_001.png"),
                        0x20, true, 1.0f, false, GRIDVIEW_TO_BG(Locked),
                        true, 32.0f
                    ))
                    .scale(.5f)
                    .exec([](ButtonSprite* s) -> void {
                        s->m_obSpritePosition = CCPoint { 0.0f, -1.0f };
                        s->updateSpriteBGSize(s->m_obSpritePosition);
                    })
                    .done(),
                this,
                (SEL_MenuHandler)&LayerViewPopup::onGridView
            ))
            .move(- this->m_pLrSize.width / 2 + 50.0f, this->m_pLrSize.height / 2 - 92.5f)
            .exec([this](CCMenuItemSpriteExtra* s) -> void {
                this->m_vGridViewBtns.push_back(s);
            })
            .udata(kGridViewLocked)
            .done()
        );
        this->m_pButtonMenu->addChild(CCNodeConstructor<CCMenuItemSpriteExtra*>()
            .fromNode(CCMenuItemSpriteExtra::create(
                CCNodeConstructor<ButtonSprite*>()
                    .fromNode(ButtonSprite::create(
                        createBESprite("BE_eye-on.png"),
                        0x20, true, 1.0f, false, GRIDVIEW_TO_BG(Visible),
                        true, 32.0f
                    ))
                    .scale(.5f)
                    .exec([](ButtonSprite* s) -> void {
                        s->m_obSpritePosition = CCPoint { 0.0f, -1.0f };
                        s->updateSpriteBGSize(s->m_obSpritePosition);
                    })
                    .done(),
                this,
                (SEL_MenuHandler)&LayerViewPopup::onGridView
            ))
            .move(- this->m_pLrSize.width / 2 + 50.0f, this->m_pLrSize.height / 2 - 115.0f)
            .exec([this](CCMenuItemSpriteExtra* s) -> void {
                this->m_vGridViewBtns.push_back(s);
            })
            .udata(kGridViewVisible)
            .done()
        );
    }

    // this->m_pEditNode = LayerEditNode::create(this, nullptr);
    // this->m_pEditNode->setPosition(140.0f, 20.0f);
    // this->m_pButtonMenu->addChild(this->m_pEditNode);

    this->updatePage();

    for (auto const& node : m_vGridViewBtns)
        node->setVisible(m_nViewType == kViewTypeGrid);
}

void LayerViewPopup::updateLayerItem(int number, LayerManager::Layer* layer) {
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    auto btn = as<CCMenuItemSpriteExtra*>(
        this->m_pButtonMenu->getChildByTag(
            LayerViewPopup::lock_tag_offset + number
        )
    );
    if (btn) updateLockSprite(btn, layer);

    auto name = as<CCLabelBMFont*>(
        this->m_pLayer->getChildByTag(
            LayerViewPopup::name_tag_offset + number
        )
    );

    if (name) {
        if (layer->m_sName.size())
            name->setString(layer->m_sName.c_str());
        else
            name->setString("Layer");

        name->limitLabelWidth(s_fItemWidth - 75.0f, .3f, .1f);

        name->setPositionX(as<int>(name->getUserData()) + name->getScaledContentSize().width / 2);
    } else {
        auto nameBtn = as<CCMenuItemSpriteExtra*>(
            this->m_pButtonMenu->getChildByTag(
                name_tag_offset + number
            )
        );

        if (nameBtn) {
            name = as<CCLabelBMFont*>(nameBtn->getNormalImage()->getChildByTag(1));
        
            if (name) {
                if (layer->m_sName.size()) {
                    auto nameStr = layer->m_sName;

                    if (nameStr.size() > 7) {
                        nameStr = nameStr.substr(0, 5);
                        nameStr += "...";
                    }

                    name->setString(nameStr.c_str());
                    name->setOpacity(255);
                } else {
                    name->setString("-");
                    name->setOpacity(150);
                }

                name->limitLabelWidth(
                    nameBtn->getNormalImage()->getScaledContentSize().width - 5.0f, .3f, .1f
                );
            }
        }
    }

    auto view = as<CCMenuItemSpriteExtra*>(
        this->m_pButtonMenu->getChildByTag(
            LayerViewPopup::eye_tag_offset + number
        )
    );
    if (view) updateShowSprite(view, layer);
}

void LayerViewPopup::createGridItemAtPosition(float x, float y, bool color, int index) {
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    auto bg = CCSprite::createWithSpriteFrameName("GJ_colorBtn_001.png");

    bg->setPosition({ 0, 0 });
    bg->setScale(.8f);
    bg->setColor(listBGDark);
    bg->setOpacity(150);

    auto tinyIxLabel = CCLabelBMFont::create(std::to_string(index).c_str(), "bigFont.fnt");

    tinyIxLabel->setScale(.2f);
    tinyIxLabel->setPosition(
        5.0f + tinyIxLabel->getScaledContentSize().width / 2,
        bg->getScaledContentSize().height
    );

    bg->addChild(tinyIxLabel);

    SEL_MenuHandler callback = nullptr;
    int bindex = 0;

    switch (this->m_nGridView) {
        case kGridViewVisible: {
            auto viewSprite = createBESprite("BE_eye-on.png"); 

            viewSprite->setScale(.6f);
            viewSprite->setContentSize(viewSprite->getScaledContentSize());
            viewSprite->setPosition(bg->getScaledContentSize() / 2);
            viewSprite->setTag(1);

            bg->addChild(viewSprite);

            callback = (SEL_MenuHandler)&LayerViewPopup::onShow;
            bindex = eye_tag_offset + index;
        } break;

        case kGridViewLocked: {
            auto lockSprite = CCSprite::createWithSpriteFrameName("GJ_lock_001.png");

            lockSprite->setScale(.6f);
            lockSprite->setContentSize(lockSprite->getScaledContentSize());
            lockSprite->setPosition(bg->getScaledContentSize() / 2);
            lockSprite->setTag(1);

            bg->addChild(lockSprite);

            callback = (SEL_MenuHandler)&LayerViewPopup::onLock;
            bindex = lock_tag_offset + index;
        } break;

        case kGridViewNormal: {
            auto ixLabel = CCLabelBMFont::create(std::to_string(index).c_str(), "goldFont.fnt");

            ixLabel->setPosition(bg->getContentSize() / 2);
            ixLabel->limitLabelWidth(20.0f, .5f, .1f);
            ixLabel->setAnchorPoint({ .5f, .5f });

            tinyIxLabel->setVisible(false);

            bg->addChild(ixLabel);
        } break;
    
        case kGridViewAlt: {
            auto nameLabel = CCLabelBMFont::create("", "bigFont.fnt");

            nameLabel->setPosition(bg->getContentSize() / 2);
            nameLabel->limitLabelWidth(20.0f, .5f, .1f);
            nameLabel->setAnchorPoint({ .5f, .5f });
            nameLabel->setTag(1);

            bg->addChild(nameLabel);

            callback = (SEL_MenuHandler)&LayerViewPopup::onSettings;
            bindex = name_tag_offset + index;
        } break;
    }

    auto btn = CCMenuItemSpriteExtra::create(
        bg, this, callback
    );
    btn->setPosition(CCPoint { x, y });
    btn->setUserData(as<void*>(index));
    btn->setTag(bindex);

    this->m_pButtonMenu->addChild(btn);
    this->m_vPageContent.push_back(btn);

    this->updateLayerItem(index, LayerManager::get()->getLayer(index));
}

void LayerViewPopup::createListItemAtPosition(float x, float y, bool color, int index) {
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    auto bg = CCScale9Sprite::create(
        "square02b_001.png", { 0.0f, 0.0f, 80.0f, 80.0f }
    );

    bg->setPosition(CCPoint { x, y });
    bg->setScale(.5f);
    bg->setColor(color ? listBGLight : listBGDark);
    bg->setContentSize({ s_fItemWidth * 2, 60.0f });
    bg->setZOrder(-static_cast<int>(color));

    this->m_pButtonMenu->addChild(bg);
    this->m_vPageContent.push_back(bg);

    auto ixLabel = CCLabelBMFont::create(std::to_string(index).c_str(), "goldFont.fnt");

    ixLabel->limitLabelWidth(45.0f, .6f, .1f);

    ixLabel->setPositionX(x + s_fItemWidth / 2 - 20.0f);
    ixLabel->setPositionY(y);
    ixLabel->setUserData(as<void*>(index));

    this->m_pButtonMenu->addChild(ixLabel);
    this->m_vPageContent.push_back(ixLabel);

    auto lock = 
        CCNodeConstructor<CCMenuItemSpriteExtra*>()
            .fromNode(CCMenuItemSpriteExtra::create(
                CCNodeConstructor()
                    .fromFrameName("GJ_lock_001.png")
                    .scale(.7f)
                    .exec([](auto t) -> void {
                        t->setContentSize(t->getScaledContentSize());
                    })
                    .done(),
                this,
                (SEL_MenuHandler)&LayerViewPopup::onLock
            ))
            .move(x - s_fItemWidth / 2 + 20.0f, y)
            .udata(index)
            .tag(lock_tag_offset + index)
            .done();

    this->m_pButtonMenu->addChild(lock);
    this->m_vPageContent.push_back(lock);

    auto viewToggle = 
        CCNodeConstructor<CCMenuItemSpriteExtra*>()
            .fromNode(CCMenuItemSpriteExtra::create(
                CCNodeConstructor()
                    .fromBESprite("BE_eye-on.png")
                    .scale(.6f)
                    .exec([](auto t) -> void {
                        t->setContentSize(t->getScaledContentSize());
                    })
                    .done(),
                this,
                (SEL_MenuHandler)&LayerViewPopup::onShow
            ))
            .udata(index)
            .move(x - s_fItemWidth / 2 + 45.0f, y)
            .tag(eye_tag_offset + index)
            .done();

    this->m_pButtonMenu->addChild(viewToggle);
    this->m_vPageContent.push_back(viewToggle);

    // auto gotoBtn = 
    //     CCNodeConstructor<CCMenuItemToggler*>()
    //         .fromNode(CCMenuItemToggler::createWithStandardSprites(
    //             this,
    //             (SEL_MenuHandler)&LayerViewPopup::onViewLayer
    //         ))
    //         .move(x - s_fItemWidth / 2 + 70.0f, y)
    //         .scale(.5f)
    //         .udata(index)
    //         .tag(view_tag_offset + index)
    //         .done();

    // this->m_pButtonMenu->addChild(gotoBtn);
    // this->m_vPageContent.push_back(gotoBtn);

    auto settings = 
        CCNodeConstructor<CCMenuItemSpriteExtra*>()
            .fromNode(CCMenuItemSpriteExtra::create(
                CCNodeConstructor()
                    .fromFrameName("GJ_optionsBtn_001.png")
                    .scale(.35f)
                    .done(),
                this,
                (SEL_MenuHandler)&LayerViewPopup::onSettings
            ))
            .udata(index)
            .move(x + s_fItemWidth / 2 - 48.0f, y)
            .done();

    this->m_pButtonMenu->addChild(settings);
    this->m_vPageContent.push_back(settings);

    auto titleLabel = CCLabelBMFont::create("Layer", "bigFont.fnt");

    titleLabel->setPositionX(x + winSize.width / 2 - 60.0f);
    titleLabel->setPositionY(y + winSize.height / 2 + 6.0f);
    titleLabel->limitLabelWidth(s_fItemWidth - 75.0f, .3f, .1f);
    titleLabel->setTag(name_tag_offset + index);
    titleLabel->setUserData(as<void*>(static_cast<int>(titleLabel->getPositionX())));

    this->m_pLayer->addChild(titleLabel);
    this->m_vPageContent.push_back(titleLabel);

    auto objCountLabel = CCLabelBMFont::create((std::to_string(
        getObjectsInLayer(index)
    ) + " objs").c_str(), "goldFont.fnt");

    objCountLabel->limitLabelWidth(s_fItemWidth / 2 - 35.0f, .3f, .1f);
    objCountLabel->setPositionX(x + winSize.width / 2 + objCountLabel->getScaledContentSize().width / 2 - 60.0f);
    objCountLabel->setPositionY(y + winSize.height / 2 - 6.0f);

    this->m_pLayer->addChild(objCountLabel);
    this->m_vPageContent.push_back(objCountLabel);

    this->updateLayerItem(index, LayerManager::get()->getLayer(index));
}

void LayerViewPopup::updatePage() {
    for (auto const& node : this->m_vPageContent)
        node->removeFromParentAndCleanup(true);
    
    this->m_vPageContent.clear();

    int max_items = maxItemsPerPage();

    int item = -1;
    for (int i = 0; i < max_items; i++) {
        item++;
        auto a_item = item + firstItemInPage();

        if (a_item > LayerManager::max_layer_num)
            return;
        
        if (this->m_bHideEmptyLayers && !getObjectsInLayer(a_item)) {
            if (this->m_mObjectsPerLayer.rbegin()->first + 1 == item)
                return;
            i--;
            continue;
        }

        float posX, posY;

        switch (this->m_nViewType) {
            case kViewTypeGrid: {
                float w = s_fItemWidth - 40.0f, h = this->m_pLrSize.height - 100.0f;
                int column_count = grid_max_item_count / grid_row_item_count;

                posX = static_cast<float>(i % grid_row_item_count) / (grid_row_item_count - 1) * w - w / 2;
                posY = (static_cast<float>(column_count - 1 - floor(i / grid_row_item_count)) / (column_count - 1)) * h - h / 2;

                posX;
                posY -= 12.5f;

                this->createGridItemAtPosition(posX, posY, i % 2, a_item);
            } break;
            
            case kViewTypeList: default: {
                posX = 0.0f;
                posY = (list_max_item_count / 2.0f * 28.0f) - i * 28.0f - 25.0f;

                this->createListItemAtPosition(posX, posY, i % 2, a_item);
            } break;
        }
    }
    
    this->m_pPageLabel->setString(std::to_string(this->m_nCurrentPage + 1).c_str());
}

void LayerViewPopup::onPage(CCObject* pSender) {
    this->m_nCurrentPage += as<int>(as<CCNode*>(pSender)->getUserData());

    if (this->m_nCurrentPage < 0)
        this->m_nCurrentPage = 0;
    if (this->m_nCurrentPage > countPages())
        this->m_nCurrentPage = countPages();
    
    this->updatePage();
}

void LayerViewPopup::onClose(CCObject* pSender) {
    auto ui = LevelEditorLayer::get()->getEditorUI();

    if (LevelEditorLayer::get()->m_nCurrentLayer == -1)
        ui->m_pCurrentLayerLabel->setString("All");
    else
        ui->m_pCurrentLayerLabel->setString(CCString::createWithFormat("%d",
            LevelEditorLayer::get()->m_nCurrentLayer
        )->getCString());

    updateEditorLayerInputText(ui);

    g_nPage = this->m_nCurrentPage;
    g_nViewType = this->m_nViewType;
    g_nGridView = this->m_nGridView;
    g_bHideEmptyLayers = this->m_bHideEmptyLayers;

    BrownAlertDelegate::onClose(pSender);
}

void LayerViewPopup::onLock(CCObject* pSender) {
    auto btn = as<CCMenuItemSpriteExtra*>(pSender);
    auto index = as<int>(btn->getUserData());
    auto layer = LayerManager::get()->getLayer(index);

    if (!layer)
        return;
    
    layer->m_bLocked = !layer->m_bLocked;

    updateLockSprite(btn, layer);
}

void LayerViewPopup::onLockAll(CCObject* pSender) {
    for (auto i = 0; i < LayerManager::max_layer_num; i++) {
        auto layer = LayerManager::get()->getLayer(i);

        if (layer) {
            layer->m_bLocked = as<CCNode*>(pSender)->getUserData();

            auto btn = as<CCMenuItemSpriteExtra*>(this->m_pButtonMenu->getChildByTag(lock_tag_offset + i));
            
            if (btn) updateLockSprite(btn, layer);
        }
    }
}

void LayerViewPopup::onShow(CCObject* pSender) {
    auto btn = as<CCMenuItemSpriteExtra*>(pSender);
    auto index = as<int>(btn->getUserData());
    auto layer = LayerManager::get()->getLayer(index);

    if (!layer)
        return;
    
    layer->m_bVisible = !layer->m_bVisible;

    updateShowSprite(btn, layer);
}

void LayerViewPopup::onShowAll(CCObject* pSender) {
    for (auto i = 0; i < LayerManager::max_layer_num; i++) {
        auto layer = LayerManager::get()->getLayer(i);

        if (layer) {
            layer->m_bVisible = as<CCNode*>(pSender)->getUserData();

            auto btn = as<CCMenuItemSpriteExtra*>(this->m_pButtonMenu->getChildByTag(eye_tag_offset + i));
            
            if (btn) updateShowSprite(btn, layer);
        }
    }
}

void LayerViewPopup::onSettings(CCObject* pSender) {
    auto layer = LayerManager::get()->getLayer(
        as<int>(as<CCMenuItemSpriteExtra*>(pSender)->getUserData())
    );

    if (!layer)
        return;

    LayerEditPopup::create(this, layer)->show();
}

void LayerViewPopup::onGlobalSettings(CCObject*) {
    LayerSettingsPopup::create(this)->show();
}

void LayerViewPopup::onGoToLayer(CCObject* pSender) {
    auto btn = as<CCMenuItemSpriteExtra*>(pSender);
    auto index = as<int>(btn->getUserData());

    LevelEditorLayer::get()->setCurrentLayer(index);
}

void LayerViewPopup::onViewLayer(CCObject* pSender) {
    auto btn = as<CCMenuItemToggler*>(pSender);
    auto index = as<int>(btn->getUserData());

    if (!LayerManager::get()->getLevel()->m_bMultipleVisible)
        for (auto i = 0; i < LayerManager::max_layer_num; i++) {
            auto layer = LayerManager::get()->getLayer(i);

            if (layer) {
                auto btn = as<CCMenuItemToggler*>(this->m_pButtonMenu->getChildByTag(view_tag_offset + i));
                if (btn) btn->toggle(false);
            }
        }

    auto layer = LayerManager::get()->getLayer(index);
    
    if (!layer)
        return;
    
    LayerManager::get()->getLevel()->addVisible(index);

    LevelEditorLayer::get()->setCurrentLayer(index);
}

void LayerViewPopup::onEditLayer(CCObject* pSender) {
    auto btn = as<CCMenuItemToggler*>(pSender);
    auto index = as<int>(btn->getUserData());

    auto layer = LayerManager::get()->getLayer(index);
    
    if (!layer)
        return; 
    
    // this->m_pEditNode->updateNode(layer);
}

void LayerViewPopup::onViewType(CCObject* pSender) {
    auto btn = as<CCMenuItemSpriteExtra*>(pSender);
    auto type = enum_cast<ViewType>(btn->getUserData());

    this->m_nViewType = type;

    this->m_pViewGridBtn->updateBGImage(VIEWTYPE_TO_BG(Grid));
    this->m_pViewListBtn->updateBGImage(VIEWTYPE_TO_BG(List));

    for (auto const& node : m_vGridViewBtns)
        node->setVisible(m_nViewType == kViewTypeGrid);

    this->updatePage();
}

void LayerViewPopup::onGridView(CCObject* pSender) {
    auto btn = as<CCMenuItemSpriteExtra*>(pSender);
    auto type = enum_cast<GridView>(btn->getUserData());

    this->m_nGridView = type;

    for (auto const& node : m_vGridViewBtns)
        as<ButtonSprite*>(node->getNormalImage())->updateBGImage(BOOL_TO_BG(node == btn));

    this->updatePage();
}

void LayerViewPopup::onToggler(CCObject* pSender) {
    auto toggle = as<CCMenuItemToggler*>(pSender);
    
    *as<bool*>(toggle->getUserData()) = !toggle->isToggled();

    this->m_pViewEmptyBtn->updateBGImage(BOOL_TO_BG(this->m_bHideEmptyLayers));
    this->updatePage();
}

void LayerViewPopup::onToggleEmpty(CCObject* pSender) {
    this->m_bHideEmptyLayers = !this->m_bHideEmptyLayers;

    this->m_pViewEmptyBtn->updateBGImage(BOOL_TO_BG(this->m_bHideEmptyLayers));
    this->updatePage();
}

void LayerViewPopup::keyDown(enumKeyCodes key) {
    switch (key) {
        case KEY_Left:
            onPage(m_pPrevPageBtn);
            break;
        case KEY_Right:
            onPage(m_pNextPageBtn);
            break;
        default:
            BrownAlertDelegate::keyDown(key);
    }
}
LayerViewPopup* LayerViewPopup::create() {
    auto pRet = new LayerViewPopup();

    if (pRet && pRet->init(460.0f, 260.0f, "GJ_square01.png", "Editor Layers")) {
        pRet->autorelease();
        return pRet;
    }

    CC_SAFE_DELETE(pRet);
    return nullptr;
}
