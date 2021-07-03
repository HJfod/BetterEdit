#include <GDMake.h>

class ButtonSelector : public cocos2d::CCMenu {
    public:
        using ButtonPair = struct { const char* name; cocos2d::CCArray* arr; };
        using ButtonList = std::initializer_list<ButtonPair>;

    protected:
        cocos2d::CCArray* m_pButtons;

        bool init(ButtonList const& list);
        void onSelect(cocos2d::CCObject*);

    public:
        static ButtonSelector* create(ButtonList const& list);

        virtual ~ButtonSelector();
};
