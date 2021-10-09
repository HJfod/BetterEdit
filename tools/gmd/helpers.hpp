#pragma once

#include "gmd.hpp"

namespace gmd {
    std::string sanitizeString(std::string const& str, bool actuallyDoIt = true);
    std::string decodeCompression(std::string str, std::string compArgs);
    std::string applyCompression(std::string str, std::string compArgs);
    std::string fixPlist(std::string const& str);
}
