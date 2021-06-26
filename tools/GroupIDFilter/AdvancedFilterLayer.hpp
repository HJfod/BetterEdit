#pragma once

#include <GDMake.h>
#include "../../BetterEdit.hpp"
#include <BrownAlertDelegate.hpp>
#include <InputNode.hpp>
#include <type_traits>

#define AFL_IORF(T, c1) \
    std::is_same<T, int>::value ? static_cast<T>(std::stoi(c1)) : static_cast<T>(std::stof(c1))

class AdvancedFilterLayer : public BrownAlertDelegate {
    public:
        struct Parseable {
            virtual void parse(std::string const&) = 0;
            std::string parseString = "";
        };

        struct IDFilter : Parseable {
            enum MatchResult { resMatch, resNegateMatch, resOptOutMatch, resOutsideMatch };

            struct Match {
                int startID = -1;
                int endID = -1;  // only used in range
                bool negate = false;
                bool optional = false;

                inline MatchResult match(int id) {
                    if (startID <= id && id <= endID) {
                        if (optional) return resOptOutMatch;
                        return negate ? resNegateMatch : resMatch;
                    }
                    
                    return resOutsideMatch;
                }

                inline Match(std::string const& str) {
                    std::string num1 = "", num2 = "";
                    bool nextNum = false;

                    for (auto c : str)
                        switch (c) {
                            case '-': nextNum = true; break;
                            case '!': this->negate = true; break;
                            case '?': this->optional = true; break;
                            case '+': this->endID = 9999; break;
                            default: if (nextNum) num2 += c; else num1 += c;
                        }
                    
                    if (num1 != "") this->startID = std::stoi(num1);
                    if (num2 != "") this->endID = std::stoi(num2);
                    if (this->startID == -1) this->startID = 0;
                    if (this->endID == -1) this->endID = this->startID;
                }
            };

            bool strict = false;
            std::vector<std::vector<Match>> filters;
            unsigned int requiredMatches = 0u;

            void setStrict(bool sc) {
                this->strict = sc;
            }

            void parse(std::string const& str) override {
                this->filters.clear();
                this->parseString = "";
                this->requiredMatches = 0u;
                
                if (!str.size()) return;

                for (auto f : splitString(str, ',')) {
                    std::vector<IDFilter::Match> matches;

                    if (f.find('|') != std::string::npos)
                        for (auto m : splitString(f, '|'))
                            matches.push_back(IDFilter::Match(m));
                    else
                        matches.push_back(IDFilter::Match(f));
                    
                    this->filters.push_back(matches);
                }

                // this is stupid
                for (auto filter : this->filters) {
                    bool isOptional = false;

                    for (auto option : filter)
                        if (option.optional)
                            isOptional = true;
                    
                    if (!isOptional)
                        requiredMatches++;
                }

                this->parseString = str;
            }

            void parse(std::string const& str, bool sc) {
                this->setStrict(sc);
                this->parse(str);
            }

            inline bool match(std::vector<int> const& ids) {
                // if there are no filters, all matches are succesful
                if (filters.empty())
                    return true;

                auto idsMatched = 0u;
                auto matches = 0u;

                for (auto id : ids) {
                    for (auto filter : this->filters)
                        for (auto option : filter) 
                            switch (option.match(id)) {
                                case resMatch: matches++; idsMatched++; goto filter_done;
                                case resNegateMatch: return false;
                                case resOptOutMatch: idsMatched++; goto filter_done;
                                case resOutsideMatch: break; // it's already resOutsideMatch
                                                             // if we reach this point and it
                                                             // isn't optional
                            }

                filter_done:
                    true;
                    // because for some reason MSVC
                    // doesn't allow empty labels
                }

                if (this->strict) return (matches >= this->requiredMatches) && (idsMatched >= ids.size());
                return matches >= this->requiredMatches;
            }
        };

        struct ObjFilter {
            template<typename T>
            struct Range : Parseable {
                bool none = true;
                T start = 0;
                T end = -1;

                void clear() {
                    this->none = true;
                    this->parseString = "";
                    this->start = 0;
                    this->end = -1;
                }

                bool in(T val) {
                    if (none) return true;
                    return start <= val && val <= end;
                }

                void parse(std::string const& rtext) {
                    this->clear();

                    if (!rtext.size())
                        return;

                    this->parseString = rtext;

                    auto text = stringToLower(rtext);
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

                    this->none = false;
                    std::string clct = "";
                    bool clctb = false;
                    for (auto c : text)
                        switch (c) {
                            case '-':
                                clctb = true;
                                if (clct.size())
                                    this->start = AFL_IORF(T, clct);
                                break;
                                
                            case '0': case '1': case '2': case '3': case '4':
                            case '5': case '6': case '7': case '8': case '9':
                                clct += c;
                                break;
                        }
                    
                    if (clct.size())
                        if (clctb)
                            this->end = AFL_IORF(T, clct);
                        else
                            this->start = AFL_IORF(T, clct);

                    if (this->end == -1) this->end = this->start;
                }
            };

            enum ELayer : int {
                NA = 0,
                B4 = 0b0000001,
                B3 = 0b0000010,
                B2 = 0b0000100,
                B1 = 0b0001000,
                T1 = 0b0010000,
                T2 = 0b0100000,
                T3 = 0b1000000,
            };
            ELayer fromZLayer(int z, int dz) {
                switch (z) {
                    case -3: return B4;
                    case -1: return B3;
                    case 1: return B2;
                    case 3: return B1;
                    case 5: return T1;
                    case 7: return T2;
                    case 9: return T3;
                    case 0: return fromZLayer(dz, 999);
                    default: return NA;
                }
            }

            template<typename T>
            struct BoolList {
                static_assert(std::is_enum<T>::value, "BoolList requires an enum");
                protected:
                    int value = 0;

                public:
                    bool hasSet() { return value; }
                    bool isSet(T bit) { return value & bit; }
                    void set(T bit, bool set) {
                        if (set) value |= bit; else value &= ~bit;
                    }
                    void set(T bit) { value ^= bit; }
                    void clear() { value = 0; }
            };

            IDFilter* groups;
            Range<float> scale;
            Range<int> zOrder;
            Range<int> color1;
            Range<int> color2;
            enum EDetail : int { None=0, High=1, Low=2 } detail = None;
            BoolList<ELayer> layers;

            ObjFilter() {
                groups = new IDFilter;
            }

            bool isEmpty() {
                if (groups->filters.size()) return false;
                
                if (!scale.none) return false;
                if (!zOrder.none) return false;
                if (!color1.none) return false;
                if (!color2.none) return false;
                if (detail != None) return false;
                if (layers.hasSet()) return false;

                return true;
            }

            void clearFilters() {
                groups->filters.clear();
                groups->parseString = "";
                groups->strict = false;

                scale.clear();
                zOrder.clear();
                color1.clear();
                color2.clear();
                
                detail = None;

                layers.clear();
            }

            bool match(gd::GameObject* obj) {
                auto gids = obj->getGroupIDs();
                if (!groups->match(std::vector<int> ( gids.begin(), gids.end() )))
                    return false;
                
                if (!(scale.in(obj->m_fScale))) return false;
                if (!(zOrder.in(obj->getZOrder()))) return false;
                if (!(color1.in(obj->getBaseColor()->colorID))) return false;
                
                if (obj->getDetailColor())
                    if (!(color2.in(obj->getDetailColor()->colorID))) return false;
                
                if (detail == Low && obj->m_bHighDetail) return false;
                if (detail == High && !obj->m_bHighDetail) return false;

                if (layers.hasSet() && !layers.isSet(fromZLayer(obj->m_nZLayer, obj->m_nDefaultZLayer))) return false;

                return true;
            }
        };

    protected:
        std::vector<InputNode*> m_vInputs;
        std::vector<gd::CCMenuItemToggler*> m_vToggles;
        gd::CCMenuItemSpriteExtra* m_pSenderBtn;

        void setup() override;

        void reset(cocos2d::CCObject*);

        void onClose(cocos2d::CCObject*);
        void onStrict(cocos2d::CCObject*);
        void onDetails(cocos2d::CCObject*);
        void onLayer(cocos2d::CCObject*);
        void onInfo(cocos2d::CCObject*);
        void onGroupInfo(cocos2d::CCObject*);

        static std::vector<std::string> splitString(std::string const& str, char split);
    
    public:
        static ObjFilter* getFilter();
        static bool noFilter();

        static AdvancedFilterLayer* create(gd::CCMenuItemSpriteExtra* pSender = nullptr);
};
