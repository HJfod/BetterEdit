#pragma once

#include <GDMake.h>
#include "../../BetterEdit.hpp"
#include <BrownAlertDelegate.hpp>
#include <InputNode.hpp>

class GroupIDInputLayer : public BrownAlertDelegate {
    public:
        struct IDFilter {
            struct Match {
                int startID;
                int endID;  // only used in range
                bool negate;

                bool match(int id) {
                    if (startID == endID)
                        return (startID == endID) == !negate;
                    
                    if (startID <= id && id <= endID)
                        return !negate;
                    
                    return false;
                }
            };

            bool strict;
            std::vector<std::vector<Match>> filters;

            bool match(int id) {
                bool res = true;

                for (auto f : filters) {
                    bool mc = true;

                    for (auto m : f)
                        if (m.match(id))
                            mc = false;
                    
                    if (!mc)
                        res = false;  // i did not think this logic through
                                      // i have zero clue if it works
                }

                return res;
            }
        };

    protected:
        InputNode* m_pGroupInput;
        InputNode* m_pColorInput;
        gd::CCMenuItemToggler* m_pGroupStrict;
        gd::CCMenuItemToggler* m_pColorStrict;

        void setup() override;

        void reset(cocos2d::CCObject*);
        void onClose(cocos2d::CCObject*);

        static std::vector<std::string> splitString(std::string const& str, char split);
        static IDFilter parseString(std::string const& str, bool strict);
    
    public:
        static IDFilter const& getGroupFilter();
        static IDFilter const& getColorFilter();

        static GroupIDInputLayer* create();
};
