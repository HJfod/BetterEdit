#pragma once

#include <Geode/DefaultInclude.hpp>

using namespace cocos2d;

class Tag : public CCNode {
protected:
    bool init(std::string const& text);

public:
    static Tag* create(std::string const& text);
};
