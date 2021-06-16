#pragma once

#include <GDMake.h>
#include "../../BetterEdit.hpp"
#include <BrownAlertDelegate.hpp>
#include <InputNode.hpp>

class GroupIDInputLayer : public BrownAlertDelegate {
    public:
        struct IDFilter {
            enum MatchResult { resMatch, resNegateMatch, resOptOutMatch, resOutsideMatch };
            constexpr const char* resToString(MatchResult m) {
                switch (m) {
                    case resMatch: return "resMatch";
                    case resNegateMatch: return "resNegateMatch";
                    case resOptOutMatch: return "resOptOutMatch";
                    case resOutsideMatch: return "resOutsideMatch";
                }
                return "";
            }

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

                std::cout << "\n";

                auto idsMatched = 0u;

                for (auto id : ids) {
                    std::cout << "id: " << id << "\n";
                    auto match = resOutsideMatch;

                    for (auto filter : this->filters)
                        for (auto option : filter) {
                            auto m = option.match(id);

                            std::cout << option.startID << ".." << option.endID << "\n";
                            std::cout << option.optional << "? " << option.negate << "!\n";
                            std::cout << resToString(m) << "\n";

                            switch (m) {
                                case resMatch: match = resMatch; goto filter_done;
                                case resNegateMatch: return false;
                                case resOptOutMatch: match = resOptOutMatch; break;
                                case resOutsideMatch: break; // it's already resOutsideMatch
                                                             // if we reach this point and it
                                                             // isn't optional
                            }
                        }

                filter_done:

                    std::cout << "> " << resToString(match) << "\n";

                    switch (match) {
                        case resMatch: idsMatched++; break;
                        case resNegateMatch: return false;
                        case resOptOutMatch: if (this->strict) idsMatched++; break;
                        case resOutsideMatch: break;
                    }
                }
                
                std::cout << "=> " << idsMatched << " == " << ids.size() << "\n";

                if (this->strict) return idsMatched == ids.size();
                return idsMatched > 0;
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
