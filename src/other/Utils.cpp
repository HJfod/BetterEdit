#include "Utils.hpp"
#include <Geode/binding/EditorUI.hpp>
#include <clipper2/clipper.h>

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

CCArrayExt<GameObject> iterTargets(GameObject* target, CCArray* targets) {
    if (target) {
        return CCArrayExt<GameObject>(CCArray::createWithObject(target));
    }
    else {
        return targets;
    }
}

CCArrayExt<GameObject> iterSelected(EditorUI* ui) {
    return iterTargets(ui->m_selectedObject, ui->m_selectedObjects);
}

bool polygonIntersect(std::vector<CCPoint> const& a, CCPoint const& b) {
    using namespace Clipper2Lib;
    PointD pb { b.x, b.y };
    PathD path;
    for (auto& pt : a) {
        path.push_back({ pt.x, pt.y });
    }
    return PointInPolygon(pb, { path }) != PointInPolygonResult::IsOutside;
}
