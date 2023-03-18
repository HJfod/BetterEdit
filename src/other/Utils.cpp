#include "Utils.hpp"

std::string zLayerToString(ZLayer z) {
    switch (z) {
        case ZLayer::B4:      return "B4";
        case ZLayer::B3:      return "B3";
        case ZLayer::B2:      return "B2";
        case ZLayer::B1:      return "B1";
        case ZLayer::Default: return "Default";
        case ZLayer::T1:      return "T1";
        case ZLayer::T2:      return "T2";
        case ZLayer::T3:      return "T3";
    }
    return "Unknown";
}
