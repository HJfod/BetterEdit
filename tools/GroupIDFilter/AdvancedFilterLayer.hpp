#pragma once

#include <GDMake.h>
#include "../../BetterEdit.hpp"
#include <BrownAlertDelegate.hpp>
#include <InputNode.hpp>

class AdvancedFilterLayer : public BrownAlertDelegate {
    public:
        struct Parseable {
            virtual void parse(std::string const&) = 0;
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
            std::string filterString = "";
            unsigned int requiredMatches = 0u;

            inline void parse(std::string const& str, bool sc) {
                this->strict = sc;
                this->filters.clear();
                this->filterString = "";
                this->requiredMatches = 0u;

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

                this->filterString = str;
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

                bool in(T val) {
                    if (none) return true;
                    return start <= val <= end;
                }

                void parse(std::string const& text) {
                    if (!text.size()) {
                        this->none = true;
                        return;
                    }

                    this->none = false;
                    std::string clct = "";
                    bool clctb = false;
                    for (auto c : text)
                        switch (c) {
                            case '-':
                                clctb = true;
                                if (clct.size())
                                    this->start = std::stoi(clct);
                                break;
                            default:
                                clct += c;
                        }
                    
                    if (clct.size()) this->end = std::stoi(clct);
                    if (this->end == -1) this->end = this->start;
                }
            };

            IDFilter* groups;
            Range<float> scale;
            Range<int> zOrder;
            Range<int> color1;
            Range<int> color2;
            enum { None, Low, High } detail;

            bool isEmpty() {
                if (groups->filters.size()) return false;
                
                if (!scale.none) return false;
                if (!zOrder.none) return false;
                if (!color1.none) return false;
                if (!color2.none) return false;
                if (detail != None) return false;

                return true;
            }

            void clearFilters() {
                groups->filters.clear();
                groups->filterString = "";

                scale.none = true;
                zOrder.none = true;
                color1.none = true;
                color2.none = true;
                
                detail = None;
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

                return true;
            }
        };

    protected:
        std::vector<std::function<void(Parseable&)>> m_vInputs;
        gd::CCMenuItemToggler* m_pGroupStrict;
        gd::CCMenuItemSpriteExtra* m_pSenderBtn;

        void setup() override;

        void reset(cocos2d::CCObject*);
        void onClose(cocos2d::CCObject*);
        void onStrict(cocos2d::CCObject*);

        static std::vector<std::string> splitString(std::string const& str, char split);
    
    public:
        static ObjFilter* getFilter();
        static bool noFilter();

        static AdvancedFilterLayer* create(gd::CCMenuItemSpriteExtra* pSender = nullptr);
};
