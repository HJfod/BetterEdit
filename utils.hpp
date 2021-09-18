#pragma once

#include <string>
#include <GDMake.h>

using namespace gdmake;
using namespace gdmake::extra;

static constexpr const char* inputf_Default =
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 ,/\\{}[]()&%!?\"#¤$€:;'*^¨~+<>|.-_";
static constexpr const char* inputf_Alphabet =
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ ";
static constexpr const char* inputf_AlphabetNoSpaces =
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
static constexpr const char* inputf_Numeral =
    "0123456789";
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

static cocos2d::CCSprite* createBESprite(const char* name, const char* fallback = nullptr) {
    auto spr = cocos2d::CCSprite::createWithSpriteFrameName(name);

    if (spr) return spr;
    if (fallback) {
        spr = cocos2d::CCSprite::createWithSpriteFrameName(fallback);

        if (spr) return spr;
    }

    auto missingTextureSpr = cocos2d::CCSprite::create();

    auto missingTextureLabel = cocos2d::CCLabelBMFont::create("Missing\nTexture", "bigFont.fnt");

    missingTextureLabel->setColor({ 255, 50, 50 });
    missingTextureLabel->setScale(.3f);
    missingTextureLabel->setPosition(missingTextureLabel->getScaledContentSize() / 2);

    missingTextureSpr->addChild(missingTextureLabel);

    missingTextureSpr->setContentSize(missingTextureLabel->getScaledContentSize());

    return missingTextureSpr;
}

static constexpr cocos2d::ccColor3B cc3x(int hexValue) {
    if (hexValue <= 0xf)
        return cocos2d::ccColor3B {
            static_cast<GLubyte>(hexValue * 17),
            static_cast<GLubyte>(hexValue * 17),
            static_cast<GLubyte>(hexValue * 17)
        };
    if (hexValue <= 0xff)
        return cocos2d::ccColor3B {
            static_cast<GLubyte>(hexValue),
            static_cast<GLubyte>(hexValue),
            static_cast<GLubyte>(hexValue)
        };
    if (hexValue <= 0xfff)
        return cocos2d::ccColor3B {
            static_cast<GLubyte>((hexValue >> 8 & 0xf) * 17),
            static_cast<GLubyte>((hexValue >> 4 & 0xf) * 17),
            static_cast<GLubyte>((hexValue >> 0 & 0xf) * 17)
        };
    else
        return cocos2d::ccColor3B {
            static_cast<GLubyte>(hexValue >> 16 & 0xff),
            static_cast<GLubyte>(hexValue >> 8  & 0xff),
            static_cast<GLubyte>(hexValue >> 0  & 0xff)
        };
}

static gd::EffectGameObject* asEffectGameObject(gd::GameObject* obj) {
    if (obj != nullptr) {
        const auto vtable = *as<uintptr_t*>(obj) - gd::base;
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
            return as<gd::EffectGameObject*>(obj);
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
    if (obj && (*as<uintptr_t*>(obj) - gd::base == vtable))
        return as<R>(obj);
    
    return nullptr;
}

static cocos2d::CCPoint getMousePos() {
    auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    auto winSizePx = cocos2d::CCDirector::sharedDirector()->getOpenGLView()->getViewPortRect();
    auto ratio_w = winSize.width / winSizePx.size.width;
    auto ratio_h = winSize.height / winSizePx.size.height;
    auto mpos = cocos2d::CCDirector::sharedDirector()->getOpenGLView()->getMousePosition();
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


template<class T = cocos2d::CCSprite*>
class CCNodeConstructor {
    protected:
        T node;

    public:
        inline CCNodeConstructor<cocos2d::CCSprite*> & fromFrameName(const char* fname) {
            this->node = cocos2d::CCSprite::createWithSpriteFrameName(fname);
            return *this;
        }
        inline CCNodeConstructor<cocos2d::CCSprite*> & fromBESprite(const char* fname, const char* fback = nullptr) {
            this->node = createBESprite(fname, fback);
            return *this;
        }
        inline CCNodeConstructor<cocos2d::CCSprite*> & fromFile(const char* fname) {
            this->node = cocos2d::CCSprite::create(fname);
            return *this;
        }
        inline CCNodeConstructor<cocos2d::CCLabelBMFont*> & fromText(const char* text, const char* font) {
            this->node = cocos2d::CCLabelBMFont::create(text, font);
            return *this;
        }
        inline CCNodeConstructor<cocos2d::CCMenu*> & fromMenu() {
            this->node = cocos2d::CCMenu::create();
            return *this;
        }
        inline CCNodeConstructor<gd::ButtonSprite*> & fromButtonSprite(const char* text, const char* bg, const char* font) {
            this->node = gd::ButtonSprite::create(text, 0, 0, font, bg, 0, .8f);
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
        inline CCNodeConstructor<T> & moveR(cocos2d::CCSize winSize, float x, float y) {
            node->setPosition(winSize / 2 + cocos2d::CCPoint { x, y });
            return *this;
        }
        inline CCNodeConstructor<T> & move(cocos2d::CCPoint const& pos) {
            node->setPosition(pos);
            return *this;
        }
        inline CCNodeConstructor<T> & csize(float x, float y) {
            node->setContentSize({ x, y });
            return *this;
        }
        inline CCNodeConstructor<T> & csize(cocos2d::CCSize const& size) {
            node->setContentSize(size);
            return *this;
        }
        inline CCNodeConstructor<T> & anchor(cocos2d::CCPoint const& pos) {
            node->setAnchorPoint(pos);
            return *this;
        }
        inline CCNodeConstructor<T> & color(cocos2d::ccColor3B color) {
            node->setColor(color);
            return *this;
        }
        inline CCNodeConstructor<T> & text(const char* text) {
            node->setString(text);
            return *this;
        }
        inline CCNodeConstructor<T> & delegate(cocos2d::CCObject* pDelegate) {
            node->setDelegate(pDelegate);
            return *this;
        }
        inline CCNodeConstructor<T> & add(cocos2d::CCNode* anode) {
            node->addChild(anode);
            return *this;
        }
        inline CCNodeConstructor<T> & add(std::function<cocos2d::CCNode*(T)> func) {
            node->addChild(func(node));
            return *this;
        }
        inline CCNodeConstructor<T> & exec(std::function<void(T)> func) {
            func(node);
            return *this;
        }
        inline CCNodeConstructor<T> & uobj(cocos2d::CCObject* obj) {
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
