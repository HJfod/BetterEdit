#pragma once

#include <string>
#include <GDMake.h>

using namespace gdmake;
using namespace gdmake::extra;
using namespace cocos2d;
using namespace gd;

static constexpr const char* inputf_Default =
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 ,/\\{}[]()&%!?\"#¤$€:;'*^¨~+<>|.-_";
static constexpr const char* inputf_Alphabet =
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ ";
static constexpr const char* inputf_AlphabetNoSpaces =
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
static constexpr const char* inputf_Numeral =
    "0123456789";
static constexpr const char* inputf_ColorChannel =
    "0123456789bglineojdpwhtackrBGLINEOJDPWHTACKR ";
static constexpr const char* inputf_NumeralSigned =
    "0123456789-+";
static constexpr const char* inputf_NumeralFloat =
    "0123456789.";
static constexpr const char* inputf_NumeralFloatSigned =
    "0123456789.-+";

template<typename E, typename T>
static constexpr E enum_cast(T type) { return static_cast<E>(reinterpret_cast<int>(type)); }

static std::unordered_map<uintptr_t, std::vector<uint8_t>> g_patchedBytes;

static std::vector<uint8_t> patch(uintptr_t addr, std::vector<uint8_t> bytes, bool hardOverwrite = false, bool repatch = false) {
    if (!g_patchedBytes[addr].size())
        g_patchedBytes[addr] = patchBytesEx(addr, bytes, hardOverwrite);
    else if (repatch)
        patchBytesEx(addr, bytes, hardOverwrite);

    return g_patchedBytes[addr];
}

static void unpatch(uintptr_t addr, bool hardOverwrite = false) {
    if (addr == 0) {
        for (auto const& [key, val] : g_patchedBytes) {
            if (val.size())
                patchBytesEx(key, val, true);
        
            g_patchedBytes[key] = {};
        }
        return;
    }

    if (g_patchedBytes[addr].size())
        patchBytes(addr, g_patchedBytes[addr]);
    
    g_patchedBytes.erase(addr);
}

static bool ispatched(uintptr_t addr) {
    return g_patchedBytes.count(addr);
}

static CCSprite* createBESprite(const char* name, const char* fallback = nullptr) {
    auto spr = CCSprite::createWithSpriteFrameName(name);

    if (spr) return spr;
    if (fallback) {
        spr = CCSprite::createWithSpriteFrameName(fallback);

        if (spr) return spr;
    }

    auto missingTextureSpr = CCSprite::create();

    auto missingTextureLabel = CCLabelBMFont::create("Missing\nTexture", "bigFont.fnt");

    missingTextureLabel->setColor({ 255, 50, 50 });
    missingTextureLabel->setScale(.3f);
    missingTextureLabel->setPosition(missingTextureLabel->getScaledContentSize() / 2);

    missingTextureSpr->addChild(missingTextureLabel);

    missingTextureSpr->setContentSize(missingTextureLabel->getScaledContentSize());

    return missingTextureSpr;
}

static constexpr ccColor3B cc3x(int hexValue) {
    if (hexValue <= 0xf)
        return ccColor3B {
            static_cast<GLubyte>(hexValue * 17),
            static_cast<GLubyte>(hexValue * 17),
            static_cast<GLubyte>(hexValue * 17)
        };
    if (hexValue <= 0xff)
        return ccColor3B {
            static_cast<GLubyte>(hexValue),
            static_cast<GLubyte>(hexValue),
            static_cast<GLubyte>(hexValue)
        };
    if (hexValue <= 0xfff)
        return ccColor3B {
            static_cast<GLubyte>((hexValue >> 8 & 0xf) * 17),
            static_cast<GLubyte>((hexValue >> 4 & 0xf) * 17),
            static_cast<GLubyte>((hexValue >> 0 & 0xf) * 17)
        };
    else
        return ccColor3B {
            static_cast<GLubyte>(hexValue >> 16 & 0xff),
            static_cast<GLubyte>(hexValue >> 8  & 0xff),
            static_cast<GLubyte>(hexValue >> 0  & 0xff)
        };
}

static EffectGameObject* asEffectGameObject(GameObject* obj) {
    if (obj != nullptr) {
        const auto vtable = *as<uintptr_t*>(obj) - base;
        if (
            // EffectGameObject::vftable
            vtable == 0x2e4190 ||
            // RingObject::vftable
            vtable == 0x2e390c ||
            // LabelGameObject::vftable
            vtable == 0x2e3ed8 ||
            // StartPosObject::vftable
            vtable == 0x25aa40
        )
            return as<EffectGameObject*>(obj);
    }
    return nullptr;
}

static std::string timePointAsString(const std::chrono::system_clock::time_point& tp) {
    std::time_t t = std::chrono::system_clock::to_time_t(tp);
    char buf[128];
    ctime_s(buf, sizeof buf, &t);
    buf[strlen(buf) - 1] = ' ';
    return buf;
}

template<typename T, typename R = T>
static constexpr R vtable_cast(T obj, uintptr_t vtable) {
    if (obj && (*as<uintptr_t*>(obj) - base == vtable))
        return as<R>(obj);
    
    return nullptr;
}

static CCPoint getMousePos() {
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    auto winSizePx = CCDirector::sharedDirector()->getOpenGLView()->getViewPortRect();
    auto ratio_w = winSize.width / winSizePx.size.width;
    auto ratio_h = winSize.height / winSizePx.size.height;
    auto mpos = CCDirector::sharedDirector()->getOpenGLView()->getMousePosition();
    mpos.y = winSizePx.size.height - mpos.y;
    mpos.x *= ratio_w;
    mpos.y *= ratio_h;

    return mpos;
}

template<typename R, typename T>
static constexpr R union_cast(T v) {
    static_assert(sizeof T == sizeof R, "union_cast: R and T don't match in size!");
    union {
        R r;
        T t;
    } x;
    x.t = v;
    return x.r;
}

template <typename T>
bool bool_cast(T const v) { return static_cast<bool>(reinterpret_cast<int>(v)); }

template<typename T, typename R>
static constexpr T offset_cast(R const v, uintptr_t offset) {
    return reinterpret_cast<T>(reinterpret_cast<uintptr_t>(v) + offset);
}

#define CATCH_NULL(x) if (x) x


template<class T = CCSprite*>
class CCNodeConstructor {
    protected:
        T node;

    public:
        inline CCNodeConstructor<CCSprite*> & fromFrameName(const char* fname) {
            this->node = CCSprite::createWithSpriteFrameName(fname);
            return *this;
        }
        inline CCNodeConstructor<CCSprite*> & fromBESprite(const char* fname, const char* fback = nullptr) {
            this->node = createBESprite(fname, fback);
            return *this;
        }
        inline CCNodeConstructor<CCSprite*> & fromFile(const char* fname) {
            this->node = CCSprite::create(fname);
            return *this;
        }
        inline CCNodeConstructor<CCLabelBMFont*> & fromText(const char* text, const char* font) {
            this->node = CCLabelBMFont::create(text, font);
            return *this;
        }
        inline CCNodeConstructor<CCMenu*> & fromMenu() {
            this->node = CCMenu::create();
            return *this;
        }
        inline CCNodeConstructor<ButtonSprite*> & fromButtonSprite(const char* text, const char* bg, const char* font) {
            this->node = ButtonSprite::create(text, 0, 0, font, bg, 0, .8f);
            return *this;
        }
        inline CCNodeConstructor<T> & fromNode(T node) {
            this->node = node;
            return *this;
        }
        inline CCNodeConstructor<T> & flipX(bool flip = true) {
            node->setFlipX(flip);
            return *this;
        }
        inline CCNodeConstructor<T> & flipY(bool flip = true) {
            node->setFlipY(flip);
            return *this;
        }
        inline CCNodeConstructor<T> & visible(bool show) {
            node->setVisible(show);
            return *this;
        }
        inline CCNodeConstructor<T> & z(int zix) {
            node->setZOrder(zix);
            return *this;
        }
        inline CCNodeConstructor<T> & limit(float width, float scale, float mscale) {
            node->limitLabelWidth(width, scale, mscale);
            return *this;
        }
        inline CCNodeConstructor<T> & tag(int tag) {
            node->setTag(tag);
            return *this;
        }
        inline CCNodeConstructor<T> & rotate(float rot) {
            node->setRotation(rot);
            return *this;
        }
        inline CCNodeConstructor<T> & scale(float scale) {
            node->setScale(scale);
            return *this;
        }
        inline CCNodeConstructor<T> & alpha(GLubyte opacity) {
            node->setOpacity(opacity);
            return *this;
        }
        inline CCNodeConstructor<T> & move(float x, float y) {
            node->setPosition(x, y);
            return *this;
        }
        inline CCNodeConstructor<T> & moveR(CCSize winSize, float x, float y) {
            node->setPosition(winSize / 2 + CCPoint { x, y });
            return *this;
        }
        inline CCNodeConstructor<T> & move(CCPoint const& pos) {
            node->setPosition(pos);
            return *this;
        }
        inline CCNodeConstructor<T> & csize(float x, float y) {
            node->setContentSize({ x, y });
            return *this;
        }
        inline CCNodeConstructor<T> & csize(CCSize const& size) {
            node->setContentSize(size);
            return *this;
        }
        inline CCNodeConstructor<T> & anchor(CCPoint const& pos) {
            node->setAnchorPoint(pos);
            return *this;
        }
        inline CCNodeConstructor<T> & color(ccColor3B color) {
            node->setColor(color);
            return *this;
        }
        inline CCNodeConstructor<T> & text(const char* text) {
            node->setString(text);
            return *this;
        }
        inline CCNodeConstructor<T> & delegate(CCObject* pDelegate) {
            node->setDelegate(pDelegate);
            return *this;
        }
        inline CCNodeConstructor<T> & add(CCNode* anode) {
            node->addChild(anode);
            return *this;
        }
        inline CCNodeConstructor<T> & add(std::function<CCNode*(T)> func) {
            node->addChild(func(node));
            return *this;
        }
        inline CCNodeConstructor<T> & exec(std::function<void(T)> func) {
            func(node);
            return *this;
        }
        inline CCNodeConstructor<T> & uobj(CCObject* obj) {
            node->setUserObject(obj);
            return *this;
        }
        template<typename T2>
        inline CCNodeConstructor<T> & udata(T2 data) {
            node->setUserData(reinterpret_cast<void*>(data));
            return *this;
        }
        inline CCNodeConstructor<T> & save(T* another) {
            *another = node;
            return *this;
        }
        inline T done() {
            return node;
        }
};

inline std::string stringToLower(std::string const& orig) {
    auto res = orig;
    std::transform(res.begin(), res.end(), res.begin(), [](unsigned char c){ return std::tolower(c); });
    return res;
}

inline std::string stringReplace(std::string const& orig, std::string const& subs, std::string const& reps) {
    std::string::size_type n = 0;
    std::string res = orig;

    while ( ( n = res.find( subs, n ) ) != std::string::npos ) {
        res.replace( n, subs.size(), reps );
        n += reps.size();
    }

    return res;
}

inline std::vector<std::string> stringSplit(std::string const& orig, std::string const& split) {
    std::vector<std::string> res;

    if (orig.size()) {
        auto s = orig;

        size_t pos = 0;

        while ((pos = s.find(split)) != std::string::npos) {
            res.push_back(s.substr(0, pos));
            s.erase(0, pos + split.length());
        }
    }

    return res;
}

template<typename t> inline void vectorMove(std::vector<t>& v, size_t oldIndex, size_t newIndex) {
    if (oldIndex > newIndex)
        std::rotate(v.rend() - oldIndex - 1, v.rend() - oldIndex, v.rend() - newIndex);
    else        
        std::rotate(v.begin() + oldIndex, v.begin() + oldIndex + 1, v.begin() + newIndex + 1);
}

template<typename T>
static void vectorRemoveByValue(std::vector<T> & vec, T const& value) {
    vec.erase(std::remove(vec.begin(), vec.end(), value), vec.end());
}

inline int strToInt(const char* str, bool* is = nullptr) {
    bool isStr = true;
    for (auto i = 0u; i < strlen(str); i++)
        if (!isdigit(str[i]) && str[i] != '-')
            isStr = false;
        
    if (is) *is = isStr;

    return isStr ? std::atoi(str) : -1;
}

inline std::vector<uint8_t> intToBytes(int paramInt, bool r = true) {
    std::vector<uint8_t> arrayOfByte(4);
    for (int i = 0; i < 4; i++)
        arrayOfByte[3 - i] = (paramInt >> (i * 8));
    
    if (r) std::reverse(arrayOfByte.begin(), arrayOfByte.end());
    return arrayOfByte;
}

// from https://github.com/adafcaefc/GDClipboardFix/blob/master/GDClipboardFix/Source.cpp
static bool copyToWin32Clipboard(std::string const& s) {
    if (!OpenClipboard(nullptr))
        return false;
    if (!EmptyClipboard()) {
        CloseClipboard();
        return false;
    }

    HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, s.size() + 1);
    
	if (!hg) {
		CloseClipboard();
		return false;
	}

	auto dest = GlobalLock(hg);

	if (!dest) {
		CloseClipboard();
		return false;
	}

	memcpy(dest, s.c_str(), s.size() + 1);

	GlobalUnlock(hg);

	SetClipboardData(CF_TEXT, hg);
	CloseClipboard();

	GlobalFree(hg);

    return true;
}

// from https://stackoverflow.com/questions/14762456/getclipboarddatacf-text
static std::string readWin32Clipboard() {
    if (!OpenClipboard(nullptr))
        return "";
    
    HANDLE hData = GetClipboardData(CF_TEXT);
    if (hData == nullptr) {
        CloseClipboard();
        return "";
    }

    char * pszText = static_cast<char*>(GlobalLock(hData));
    if (pszText == nullptr) {
        CloseClipboard();
        return "";
    }

    std::string text(pszText);

    GlobalUnlock(hData);
    CloseClipboard();

    return text; 
}

static std::ostream& operator<<(std::ostream& stream, CCPoint const& pos) {
    return stream << pos.x << ", " << pos.y;
}

static std::ostream& operator<<(std::ostream& stream, CCSize const& pos) {
    return stream << pos.width << " : " << pos.height;
}

static std::ostream& operator<<(std::ostream& stream, CCRect const& size) {
    return stream << size.origin << " | " << size.size;
}

static CCPoint operator "" _y (long double val) {
    return { .0f, static_cast<float>(val) };
}

static CCPoint operator "" _x (long double val) {
    return { static_cast<float>(val), .0f };
}

static std::string replaceColorChannelNames(std::string const& input) {
    auto text = stringToLower(input);
    text = stringReplace(text, "default", "0");
    text = stringReplace(text, "def", "0");
    text = stringReplace(text, "lbg", "1007");
    text = stringReplace(text, "bg", "1000");
    text = stringReplace(text, "g1", "1001");
    text = stringReplace(text, "line", "1002");
    text = stringReplace(text, "3dl", "1003");
    text = stringReplace(text, "obj", "1004");
    text = stringReplace(text, "p1", "1005");
    text = stringReplace(text, "p2", "1006");
    text = stringReplace(text, "g2", "1009");
    text = stringReplace(text, "black", "1010");
    text = stringReplace(text, "white", "1011");
    text = stringReplace(text, "lighter", "1012");
    return text;
}

static void ccDrawColor4B(ccColor4B const& c) {
    ccDrawColor4B(c.r, c.g, c.b, c.a);
}

static ccColor4F to4f(ccColor4B const& c) {
    return { c.r / 255.f, c.g / 255.f, c.b / 255.f, c.a / 255.f };
}

static ccColor3B to3B(ccColor4B const& c) {
    return { c.r, c.g, c.b };
}

static ccColor4B invert4B(ccColor4B const& c) {
    return {
        static_cast<GLubyte>(255 - c.r),
        static_cast<GLubyte>(255 - c.g),
        static_cast<GLubyte>(255 - c.b),
        c.a
    };
}

static void limitNodeSize(CCNode* spr, CCSize const& size, float def, float min) {
    spr->setScale(1.f);
    auto [cwidth, cheight] = spr->getContentSize();

    float scale = def;
    if (size.height && size.height < cheight) {
        scale = size.height / cheight;
    }
    if (size.width && size.width < cwidth) {
        if (size.width / cwidth < scale)
            scale = size.width / cwidth;
    }
    if (def && def < scale) {
        scale = def;
    }
    if (min && scale < min) {
        scale = min;
    }
    spr->setScale(scale);
}

static CCSize operator*=(CCSize & size, float mul) {
    size.width *= mul;
    size.height *= mul;
    return size;
}

static CCRect operator*=(CCRect & size, float mul) {
    size.origin.x *= mul;
    size.origin.y *= mul;
    size.size.width *= mul;
    size.size.height *= mul;
    return size;
}

static CCPoint operator/=(CCPoint & pos, float mul) {
    pos.x /= mul;
    pos.y /= mul;
    return pos;
}

static CCPoint operator*=(CCPoint & pos, float mul) {
    pos.x *= mul;
    pos.y *= mul;
    return pos;
}

static CCPoint operator+=(CCPoint & pos, CCPoint const& add) {
    pos.x += add.x;
    pos.y += add.y;
    return pos;
}

static CCSize operator-(CCSize const& size, float f) {
    return { size.width - f, size.height - f };
}

static CCSize operator-(CCSize const& size) {
    return { -size.width, -size.height };
}

static bool operator==(CCSize const& size, CCSize const& size2) {
    return
        size.width == size2.width &&
        size.height == size2.height;
}

static bool operator!=(CCPoint const& size, CCPoint const& size2) {
    return
        size.x != size2.x ||
        size.y != size2.y;
}

static bool operator==(CCPoint const& s1, CCPoint const& s2) {
    return s1.x == s2.x && s1.y == s2.y;
}

static bool operator==(CCRect const& r1, CCRect const& r2) {
    return r1.origin == r2.origin && r1.size == r2.size;
}

static GameObject* firstObject(CCArray* objs) {
    if (objs->count())
        return as<GameObject*>(objs->objectAtIndex(0));
    return nullptr;
}

static float dampenf(float x, float y) {
    return x + y / 10.f;
}

static constexpr size_t operator"" _h (const char* txt, size_t) {
    return hash(txt);
}

static CCRect getObjectRect(CCNode* node) {
    auto pos = node->getPosition();
    auto size = node->getScaledContentSize();

    return {
        pos.x - size.width / 2,
        pos.y - size.height / 2,
        size.width,
        size.height
    };
}

static bool objectsAreAdjacent(GameObject* o1, GameObject* o2) {
    auto rect1 = getObjectRect(o1);
    auto rect2 = getObjectRect(o2);

    return (
        rect1.origin.x <= rect2.origin.x + rect2.size.width &&
        rect2.origin.x <= rect1.origin.x + rect1.size.width &&
        rect1.origin.y <= rect2.origin.y + rect2.size.height &&
        rect2.origin.y <= rect1.origin.y + rect1.size.height
    );
}

static void recursiveAddNear(EditorUI* ui, GameObject* fromObj, std::vector<GameObject*> const& vnear, CCArray* arr) {
    for (auto const& obj : vnear) {
        if (objectsAreAdjacent(fromObj, obj)) {
            if (!arr->containsObject(obj)) {
                arr->addObject(obj);
                recursiveAddNear(ui, obj, vnear, arr);
            }
        }
    }
}

static void selectStructure(EditorUI* ui, GameObject* fromObj) {
    std::vector<GameObject*> nearby;
    auto pos = fromObj->getPosition();
    CCARRAY_FOREACH_B_TYPE(ui->m_pEditorLayer->getAllObjects(), obj, GameObject) {
        switch (obj->m_nObjectType) {
            case kGameObjectTypeSlope:
            case kGameObjectTypeSolid:
            case kGameObjectTypeSpecial:
            case kGameObjectTypeDecoration:
            case kGameObjectTypeHazard:
            case kGameObjectTypeGravityPad:
            case kGameObjectTypePinkJumpPad:
            case kGameObjectTypeYellowJumpPad:
            case kGameObjectTypeRedJumpPad:
                if (
                    obj->m_nEditorLayer == fromObj->m_nEditorLayer
                ) {
                    if (ccpDistance(obj->getPosition(), pos) < 500.0f) {
                        nearby.push_back(obj);
                    }
                }
                break;
        }
    }
    auto arr = CCArray::create();
    arr->addObject(fromObj);
    recursiveAddNear(ui, fromObj, nearby, arr);
    ui->deselectAll();
    ui->selectObjects(arr, false);
}
