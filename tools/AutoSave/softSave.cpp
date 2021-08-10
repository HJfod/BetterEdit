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

    // i'm such a genious

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
    // object type
    bin_write(stream, kSaveTypeAdd);
    bin_write(stream, kClassTypeGameObject);

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

    // depending on type, write more
}

void SoftSaveManager::loadObjectBinary(std::ifstream & stream) {
    GameObjectClassType otype;
    bin_read(stream, otype);

    int objID;
    CCPoint pos;

    bin_read(stream, objID);
    bin_read(stream, pos);

    auto obj = LevelEditorLayer::get()->createObject(objID, pos, false);

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

    // depending on otype, read more
}

void saveColorBinary(std::ofstream & stream, int colorID) {
    auto color = LevelEditorLayer::get()
        ->m_pLevelSettings
        ->m_effectManager
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

void loadColorBinary(std::ifstream & stream) {
    int colorID = bin_read<int>(stream);

    auto color = LevelEditorLayer::get()
        ->m_pLevelSettings
        ->m_effectManager
        ->getColorAction(colorID);
    
    color->m_color = bin_read<ccColor3B>(stream);
    color->m_blending = bin_read<bool>(stream);
    color->m_opacity = bin_read<float>(stream);
    color->m_copyHue = bin_read<float>(stream);
    color->m_copySaturation = bin_read<float>(stream);
    color->m_copyBrightness = bin_read<float>(stream);
    color->m_saturationChecked = bin_read<bool>(stream);
    color->m_brightnessChecked = bin_read<bool>(stream);
    color->m_copyID = bin_read<int>(stream);
    color->m_copyOpacity = bin_read<float>(stream);
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

    auto lvlName = LevelEditorLayer::get()->m_pLevelSettings->m_level->levelName.c_str();

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

    g_pManager->clear();
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
