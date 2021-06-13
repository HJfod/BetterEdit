#pragma once

#include <GDMake.h>
#include "../../BetterEdit.hpp"
#include <BrownAlertDelegate.hpp>
#include <InputNode.hpp>

class GroupIDInputLayer : public BrownAlertDelegate {
    public:
        struct IDFilter {
            enum MatchResult { resMatch, resNegateMatch, resOptOutMatch, resOutsideMatch };

            struct Match {
                int startID = -1;
                int endID = -1;  // only used in range
                bool negate = false;
                bool optional = false;

                inline MatchResult match(int id) {
                    if (startID <= id && id <= endID)
                        return negate ? resNegateMatch : resMatch;
                    
                    return optional ? resOptOutMatch : resOutsideMatch;
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

        private:
            inline MatchResult filterMatch(std::vector<Match> const& filter, std::vector<int> const& ids) {
                auto res = resOutsideMatch;

                for (auto id : ids)
                    for (auto option : filter)
                        switch (option.match(id)) {

                        }
            }
        public:

            inline void parse(std::string const& str, bool sc) {
                this->strict = sc;
                this->filters.clear();
                this->filterString = "";

                for (auto f : splitString(str, ',')) {
                    std::vector<IDFilter::Match> matches;

                    if (f.find('|') != std::string::npos)
                        for (auto m : splitString(f, '|'))
                            matches.push_back(IDFilter::Match(m));
                    else
                        matches.push_back(IDFilter::Match(f));
                    
                    this->filters.push_back(matches);
                }

                this->filterString = str;
            }

            inline bool match(std::vector<int> const& ids) {
                // if there are no filters, all matches are succesful
                if (filters.empty())
                    return true;

                std::vector<MatchResult> matches;

                for (auto filter : filters)
                    matches.push_back(filterMatch(filter, ids));

                auto matchc = 0u;
                auto maxc = matches.size() - 1u;
                for (auto match : matches)
                    switch (match) {
                        case resNegateMatch: return false;
                        case resMatch: matchc++; break;
                        case resOutsideMatch: if (strict) return false; maxc--;
                    }

                return matchc == maxc;
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
        void onStrict(cocos2d::CCObject*);

        static std::vector<std::string> splitString(std::string const& str, char split);
    
    public:
        static IDFilter* getGroupFilter();
        static IDFilter* getColorFilter();
        static bool noFilters();

        static GroupIDInputLayer* create();
};
