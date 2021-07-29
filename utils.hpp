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

static std::vector<uint8_t> patch(uintptr_t addr, std::vector<uint8_t> bytes, bool hardOverwrite = false) {
    if (!g_patchedBytes[addr].size())
        g_patchedBytes[addr] = patchBytesEx(addr, bytes, hardOverwrite);

    return g_patchedBytes[addr];
}

static void unpatch(uintptr_t addr, bool hardOverwrite = false) {
    if (addr == 0) {
        for (auto const& [key, val] : g_patchedBytes) {
            if (val.size())
                patchBytesEx(key, val, hardOverwrite);
        
            g_patchedBytes[key] = {};
        }
        return;
    }

    if (g_patchedBytes[addr].size())
        patchBytes(addr, g_patchedBytes[addr]);
    
    g_patchedBytes[addr] = {};
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

inline int strToInt(const char* str, bool* is = nullptr) {
    bool isStr = true;
    for (auto i = 0u; i < strlen(str); i++)
        if (!isdigit(str[i]) && str[i] != '-')
            isStr = false;
        
    if (is) *is = isStr;

    return isStr ? std::atoi(str) : -1;
}

inline std::vector<uint8_t> intToBytes(int paramInt) {
    std::vector<uint8_t> arrayOfByte(4);
    for (int i = 0; i < 4; i++)
        arrayOfByte[3 - i] = (paramInt >> (i * 8));
    
    std::reverse(arrayOfByte.begin(), arrayOfByte.end());
    return arrayOfByte;
}
