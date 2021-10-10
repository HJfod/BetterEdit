#pragma once

#include "gmd.hpp"

namespace gmd {
    std::string sanitizeString(std::string const& str, bool actuallyDoIt = true);
    std::string decodeCompression(std::string const& str, std::string const& compArgs);
    std::string applyCompression(std::string const& str, std::string const& compArgs);
    std::string fixPlist(std::string const& str);
}
