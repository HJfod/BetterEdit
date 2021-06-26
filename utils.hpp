#pragma once

#include <string>
#include <GDMake.h>

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
        inline CCNodeConstructor<T> & rotate(float rot) {
            node->setRotation(rot);
            return *this;
        }
        inline CCNodeConstructor<T> & scale(float scale) {
            node->setScale(scale);
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
        inline CCNodeConstructor<T> & color(cocos2d::ccColor3B color) {
            node->setColor(color);
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

inline int strToInt(const char* str, bool* is = nullptr) {
    bool isStr = true;
    for (auto i = 0u; i < strlen(str); i++)
        if (!isdigit(str[i]) && str[i] != '-')
            isStr = false;
        
    if (is) *is = isStr;

    return isStr ? std::atoi(str) : -1;
}
