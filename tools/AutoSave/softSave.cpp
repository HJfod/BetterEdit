#include "softSave.hpp"
#include <fstream>

SoftSaveManager* g_pManager;

template<typename T>
void bin_write(std::ofstream & stream, T val) {
    stream.write(reinterpret_cast<char*>(&val), sizeof T);
}

template<typename T>
void bin_read(std::ifstream & stream, T & val) {
    stream.read(reinterpret_cast<char*>(&val), sizeof T);
}

template<typename T>
T bin_read(std::ifstream & stream) {
    T val = T();
    stream.read(reinterpret_cast<char*>(&val), sizeof T);
    return val;
}

#define M_BIN_READ(_stream_, _val_) \
    _val_ = bin_read<decltype(_val_)>(_stream_)

enum DataSaveType : byte {
    kSaveTypeAdd,
    kSaveTypeRemove,
};

enum GameObjectClassType : byte {
    kClassTypeGameObject,
    kClassTypeEffectGameObject,
    kClassTypeLabelGameObject,
};

bool SoftSaveManager::init() {
    m_pManagedObjects = CCArray::create();
    m_pManagedObjects->retain();

    m_pManagedColors = CCArray::create();
    m_pManagedColors->retain();

    m_nCurrentActionCount = 0;

    return true;
}

bool SoftSaveManager::initGlobal() {
    if (g_pManager)
        return true;

    g_pManager = new SoftSaveManager;

    if (g_pManager && g_pManager->init())
        return true;
    
    CC_SAFE_DELETE(g_pManager);

    return false;
}

void SoftSaveManager::saveObject(GameObject* obj) {
    if (!BetterEdit::isEditorInitialized() || g_pManager->m_bDontSave)
        return;

    g_pManager->m_pManagedObjects->addObject(obj);

    // "what do i hook when i need to know if a color
    // has been modified and `GJEffectManager::getColorAction`
    // is inlined on Windows?"

    // you're a fucking genious, fod

    if (obj->m_pBaseColor)
        g_pManager->saveColor(obj->m_pBaseColor->colorID);
    if (obj->m_pDetailColor)
        g_pManager->saveColor(obj->m_pDetailColor->colorID);

    g_pManager->save();
}

void SoftSaveManager::saveColor(int color) {
    if (!BetterEdit::isEditorInitialized() || g_pManager->m_bDontSave)
        return;

    g_pManager->m_pManagedColors->addObject(CCInteger::create(color));

    g_pManager->save();
}

void SoftSaveManager::removeObject(GameObject* obj) {
    if (!BetterEdit::isEditorInitialized() || g_pManager->m_bDontSave)
        return;
        
    g_pManager->m_pManagedObjects->removeObject(obj);

    g_pManager->save();
}

void SoftSaveManager::saveObjectBinary(std::ofstream & stream, GameObject* obj) {
    bin_write(stream, kSaveTypeAdd);

    auto eobj = asEffectGameObject(obj);

    bin_write(stream, eobj ?
        kClassTypeEffectGameObject :
        kClassTypeGameObject
    );

    bin_write(stream, obj->m_nObjectID);
    bin_write(stream, obj->getPosition());
    bin_write(stream, obj->getScale());
    bin_write(stream, obj->getRotation());

    bin_write(stream, obj->getBaseColor() ? true : false);
    if (obj->getBaseColor()) {
        bin_write(stream, obj->getBaseColor()->colorID);
        bin_write(stream, obj->getBaseColor()->defaultColorID);
        bin_write(stream, obj->getBaseColor()->hue);
        bin_write(stream, obj->getBaseColor()->saturation);
        bin_write(stream, obj->getBaseColor()->brightness);
        bin_write(stream, obj->getBaseColor()->saturationChecked);
        bin_write(stream, obj->getBaseColor()->brightnessChecked);
    }

    bin_write(stream, obj->getDetailColor() ? true : false);
    if (obj->getDetailColor()) {
        bin_write(stream, obj->getDetailColor()->colorID);
        bin_write(stream, obj->getDetailColor()->defaultColorID);
        bin_write(stream, obj->getDetailColor()->hue);
        bin_write(stream, obj->getDetailColor()->saturation);
        bin_write(stream, obj->getDetailColor()->brightness);
        bin_write(stream, obj->getDetailColor()->saturationChecked);
        bin_write(stream, obj->getDetailColor()->brightnessChecked);
    }

    bin_write(stream, obj->m_nGameZOrder);
    bin_write(stream, obj->m_nZLayer);
    bin_write(stream, obj->m_nEditorLayer);
    bin_write(stream, obj->m_nEditorLayer2);

    if (eobj) {
        bin_write(stream, eobj->m_colColor);
        bin_write(stream, eobj->m_nTargetColorID);
        bin_write(stream, eobj->m_nTargetGroupID);
        bin_write(stream, eobj->m_nCenterGroupID);
        bin_write(stream, eobj->m_fDuration);
        bin_write(stream, eobj->m_fOpacity);
        bin_write(stream, eobj->m_fShakeStrength);
        bin_write(stream, eobj->m_fShakeInterval);
        bin_write(stream, eobj->m_bPlayerColor1);
        bin_write(stream, eobj->m_bPlayerColor2);
        bin_write(stream, eobj->m_bBlending);
        bin_write(stream, eobj->m_fMoveX);
        bin_write(stream, eobj->m_fMoveY);
        bin_write(stream, eobj->m_nEasingType);
        bin_write(stream, eobj->m_fEasingRate);
        bin_write(stream, eobj->m_bLockToPlayerX);
        bin_write(stream, eobj->m_bLockToPlayerY);
        bin_write(stream, eobj->m_bUseTarget);
        bin_write(stream, eobj->m_nMoveTargetType);
        bin_write(stream, eobj->m_nRotateDegrees);
        bin_write(stream, eobj->m_nTimes360);
        bin_write(stream, eobj->m_bLockObjectRotation);
        bin_write(stream, eobj->m_fFollowXMod);
        bin_write(stream, eobj->m_fFollowYMod);
        bin_write(stream, eobj->m_fFollowYSpeed);
        bin_write(stream, eobj->m_fFollowYDelay);
        bin_write(stream, eobj->m_nFollowYOffset);
        bin_write(stream, eobj->m_fFollowYMaxSpeed);
        bin_write(stream, eobj->m_fFadeInTime);
        bin_write(stream, eobj->m_fHoldTime);
        bin_write(stream, eobj->m_fFadeOutTime);
        bin_write(stream, eobj->m_bPulseHSVMode);
        bin_write(stream, eobj->m_bPulseGroupMode);
        bin_write(stream, eobj->m_fColorHue);
        bin_write(stream, eobj->m_fColorSaturation);
        bin_write(stream, eobj->m_fColorBrightness);
        bin_write(stream, eobj->m_bAbsoluteSaturation);
        bin_write(stream, eobj->m_bAbsoluteBrightness);
        bin_write(stream, eobj->m_nCopyColorID);
        bin_write(stream, eobj->m_bCopyOpacity);
        bin_write(stream, eobj->m_bPulseMainOnly);
        bin_write(stream, eobj->m_bPulseDetailOnly);
        bin_write(stream, eobj->m_bPulseExclusive);
        bin_write(stream, eobj->m_bActivateGroup);
        bin_write(stream, eobj->m_bTouchHoldMode);
        bin_write(stream, eobj->m_nTouchToggleMode);
        bin_write(stream, eobj->m_bTouchDualMode);
        bin_write(stream, eobj->m_nAnimationID);
        bin_write(stream, eobj->m_fSpawnDelay);
        bin_write(stream, eobj->m_bMultiTrigger);
        bin_write(stream, eobj->m_bEditorDisabled);
        bin_write(stream, eobj->m_nTargetCount);
        bin_write(stream, eobj->m_nComparisonType);
        bin_write(stream, eobj->m_bMultiActivate);
        bin_write(stream, eobj->m_bTriggerOnExit);
        bin_write(stream, eobj->m_nBlockBID);
        bin_write(stream, eobj->m_bDynamicBlock);
        bin_write(stream, eobj->m_nTargetItemID);
    }

    // depending on type, write more
}

void SoftSaveManager::loadObjectBinary(std::ifstream & stream) {
    DataSaveType stype;
    bin_read(stream, stype);

    GameObjectClassType otype;
    bin_read(stream, otype);

    int objID;
    CCPoint pos;

    bin_read(stream, objID);
    bin_read(stream, pos);

    auto obj = LevelEditorLayer::get()->createObject(objID, pos, false);
    auto eobj = asEffectGameObject(obj);

    obj->setScale(bin_read<float>(stream));
    obj->setRotation(bin_read<float>(stream));

    if (bin_read<bool>(stream)) {
        obj->getBaseColor()->colorID = bin_read<int>(stream);
        obj->getBaseColor()->defaultColorID = bin_read<int>(stream);
        obj->getBaseColor()->hue = bin_read<float>(stream);
        obj->getBaseColor()->saturation = bin_read<float>(stream);
        obj->getBaseColor()->brightness = bin_read<float>(stream);
        obj->getBaseColor()->saturationChecked = bin_read<bool>(stream);
        obj->getBaseColor()->brightnessChecked = bin_read<bool>(stream);
    }

    if (bin_read<bool>(stream)) {
        obj->getDetailColor()->colorID = bin_read<int>(stream);
        obj->getDetailColor()->defaultColorID = bin_read<int>(stream);
        obj->getDetailColor()->hue = bin_read<float>(stream);
        obj->getDetailColor()->saturation = bin_read<float>(stream);
        obj->getDetailColor()->brightness = bin_read<float>(stream);
        obj->getDetailColor()->saturationChecked = bin_read<bool>(stream);
        obj->getDetailColor()->brightnessChecked = bin_read<bool>(stream);
    }

    obj->m_nGameZOrder = bin_read<int>(stream);
    obj->m_nZLayer = bin_read<int>(stream);
    obj->m_nEditorLayer = bin_read<int>(stream);
    obj->m_nEditorLayer2 = bin_read<int>(stream);

    if (eobj) {
        M_BIN_READ(stream, eobj->m_colColor);
        M_BIN_READ(stream, eobj->m_nTargetColorID);
        M_BIN_READ(stream, eobj->m_nTargetGroupID);
        M_BIN_READ(stream, eobj->m_nCenterGroupID);
        M_BIN_READ(stream, eobj->m_fDuration);
        M_BIN_READ(stream, eobj->m_fOpacity);
        M_BIN_READ(stream, eobj->m_fShakeStrength);
        M_BIN_READ(stream, eobj->m_fShakeInterval);
        M_BIN_READ(stream, eobj->m_bPlayerColor1);
        M_BIN_READ(stream, eobj->m_bPlayerColor2);
        M_BIN_READ(stream, eobj->m_bBlending);
        M_BIN_READ(stream, eobj->m_fMoveX);
        M_BIN_READ(stream, eobj->m_fMoveY);
        M_BIN_READ(stream, eobj->m_nEasingType);
        M_BIN_READ(stream, eobj->m_fEasingRate);
        M_BIN_READ(stream, eobj->m_bLockToPlayerX);
        M_BIN_READ(stream, eobj->m_bLockToPlayerY);
        M_BIN_READ(stream, eobj->m_bUseTarget);
        M_BIN_READ(stream, eobj->m_nMoveTargetType);
        M_BIN_READ(stream, eobj->m_nRotateDegrees);
        M_BIN_READ(stream, eobj->m_nTimes360);
        M_BIN_READ(stream, eobj->m_bLockObjectRotation);
        M_BIN_READ(stream, eobj->m_fFollowXMod);
        M_BIN_READ(stream, eobj->m_fFollowYMod);
        M_BIN_READ(stream, eobj->m_fFollowYSpeed);
        M_BIN_READ(stream, eobj->m_fFollowYDelay);
        M_BIN_READ(stream, eobj->m_nFollowYOffset);
        M_BIN_READ(stream, eobj->m_fFollowYMaxSpeed);
        M_BIN_READ(stream, eobj->m_fFadeInTime);
        M_BIN_READ(stream, eobj->m_fHoldTime);
        M_BIN_READ(stream, eobj->m_fFadeOutTime);
        M_BIN_READ(stream, eobj->m_bPulseHSVMode);
        M_BIN_READ(stream, eobj->m_bPulseGroupMode);
        M_BIN_READ(stream, eobj->m_fColorHue);
        M_BIN_READ(stream, eobj->m_fColorSaturation);
        M_BIN_READ(stream, eobj->m_fColorBrightness);
        M_BIN_READ(stream, eobj->m_bAbsoluteSaturation);
        M_BIN_READ(stream, eobj->m_bAbsoluteBrightness);
        M_BIN_READ(stream, eobj->m_nCopyColorID);
        M_BIN_READ(stream, eobj->m_bCopyOpacity);
        M_BIN_READ(stream, eobj->m_bPulseMainOnly);
        M_BIN_READ(stream, eobj->m_bPulseDetailOnly);
        M_BIN_READ(stream, eobj->m_bPulseExclusive);
        M_BIN_READ(stream, eobj->m_bActivateGroup);
        M_BIN_READ(stream, eobj->m_bTouchHoldMode);
        M_BIN_READ(stream, eobj->m_nTouchToggleMode);
        M_BIN_READ(stream, eobj->m_bTouchDualMode);
        M_BIN_READ(stream, eobj->m_nAnimationID);
        M_BIN_READ(stream, eobj->m_fSpawnDelay);
        M_BIN_READ(stream, eobj->m_bMultiTrigger);
        M_BIN_READ(stream, eobj->m_bEditorDisabled);
        M_BIN_READ(stream, eobj->m_nTargetCount);
        M_BIN_READ(stream, eobj->m_nComparisonType);
        M_BIN_READ(stream, eobj->m_bMultiActivate);
        M_BIN_READ(stream, eobj->m_bTriggerOnExit);
        M_BIN_READ(stream, eobj->m_nBlockBID);
        M_BIN_READ(stream, eobj->m_bDynamicBlock);
        M_BIN_READ(stream, eobj->m_nTargetItemID);

        eobj->updateLabel();
    }

    // depending on otype, read more
}

void SoftSaveManager::saveColorBinary(std::ofstream & stream, int colorID) {
    auto color = LevelEditorLayer::get()
        ->m_pLevelSettings
        ->m_pEffectManager
        ->getColorAction(colorID);

    bin_write(stream, colorID);

    bin_write(stream, color->m_color);
    bin_write(stream, color->m_blending);
    bin_write(stream, color->m_opacity);
    bin_write(stream, color->m_copyHue);
    bin_write(stream, color->m_copySaturation);
    bin_write(stream, color->m_copyBrightness);
    bin_write(stream, color->m_saturationChecked);
    bin_write(stream, color->m_brightnessChecked);
    bin_write(stream, color->m_copyID);
    bin_write(stream, color->m_copyOpacity);
}

void SoftSaveManager::loadColorBinary(std::ifstream & stream) {
    int colorID = bin_read<int>(stream);

    auto color = LevelEditorLayer::get()
        ->m_pLevelSettings
        ->m_pEffectManager
        ->getColorAction(colorID);
    
    M_BIN_READ(stream, color->m_color);
    M_BIN_READ(stream, color->m_blending);
    M_BIN_READ(stream, color->m_opacity);
    M_BIN_READ(stream, color->m_copyHue);
    M_BIN_READ(stream, color->m_copySaturation);
    M_BIN_READ(stream, color->m_copyBrightness);
    M_BIN_READ(stream, color->m_saturationChecked);
    M_BIN_READ(stream, color->m_brightnessChecked);
    M_BIN_READ(stream, color->m_copyID);
    M_BIN_READ(stream, color->m_copyOpacity);
}

bool SoftSaveManager::hasLoad(std::string const& level) {
    std::ifstream f(g_softSave_file, std::ios::binary | std::ios::in | std::ios::ate);

    if (!f.is_open())
        return false;
    
    if (f.tellg() < sizeof size_t) {
        f.close();
        return false;
    }

    size_t len;
    f.seekg(0);
    f.read(reinterpret_cast<char*>(&len), sizeof size_t);

    f.seekg(sizeof size_t);

    char* name = new char[len + 1];
    f.read(name, len);
    name[len] = '\0';

    f.close();

    auto res = name == level;

    free(name);

    return res;
}

void SoftSaveManager::save() {
    if (!BetterEdit::isEditorInitialized() || g_pManager->m_bDontSave)
        return;
    if (!BetterEdit::getEnableSoftAutoSave())
        return;

    g_pManager->m_nCurrentActionCount++;

    if (g_pManager->m_nCurrentActionCount < BetterEdit::getSoftSaveFrequency())
        return;
    
    std::ofstream f(g_softSave_file, std::ios::binary | std::ios::out);

    auto lvlName = LevelEditorLayer::get()->m_pLevelSettings->m_pLevel->levelName.c_str();

    if (f.is_open()) {
        bin_write(f, strlen(lvlName));
        f.write(lvlName, strlen(lvlName));

        bin_write(f, g_pManager->m_pManagedObjects->count());
        CCARRAY_FOREACH_B_TYPE(g_pManager->m_pManagedObjects, obj, GameObject)
            g_pManager->saveObjectBinary(f, obj);
            
        bin_write(f, g_pManager->m_pManagedColors->count());
        CCARRAY_FOREACH_B_TYPE(g_pManager->m_pManagedColors, obj, CCInteger)
            g_pManager->saveColorBinary(f, obj->getValue());

    } else return; // todo: show error

    f.close();
}

void SoftSaveManager::load() {
    if (!BetterEdit::isEditorInitialized())
        return;
    if (!BetterEdit::getEnableSoftAutoSave())
        return;
    
    std::ifstream f(g_softSave_file, std::ios::binary | std::ios::in | std::ios::ate);

    if (!f.is_open())
        return;
    
    if (f.tellg() < sizeof size_t) {
        f.close();
        return;
    }

    size_t len;
    f.seekg(0);
    f.read(reinterpret_cast<char*>(&len), sizeof size_t);

    f.seekg(sizeof size_t + len);

    int count;
    f.read(reinterpret_cast<char*>(&count), sizeof size_t);

    g_pManager->m_bDontSave = true;

    while (count--)
        g_pManager->loadObjectBinary(f);

    f.read(reinterpret_cast<char*>(&count), sizeof size_t);

    while (count--)
        g_pManager->loadColorBinary(f);

    f.close();

    g_pManager->m_bDontSave = false;

    // if the editor crashes again, maybe
    // we don't want to delete progress?
    
    // g_pManager->clear();
}

void SoftSaveManager::clear() {
    g_pManager->m_pManagedObjects->removeAllObjects();

    // clear file
    std::ofstream f(g_softSave_file, std::ios::binary | std::ios::out);
    if (f.is_open())
        f.close();
}

void SoftSaveAlertDelegate::FLAlert_Clicked(FLAlertLayer*, bool btn2) {
    if (btn2)
        SoftSaveManager::load();

    this->release();
}

SoftSaveAlertDelegate* SoftSaveAlertDelegate::create() {
    auto ret = new SoftSaveAlertDelegate;

    if (ret) {
        ret->autorelease();
        ret->retain();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}
