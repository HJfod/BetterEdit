#include "Utils.hpp"
#include <Geode/binding/EditorUI.hpp>

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

bool polygonIntersect(std::vector<CCPoint> const& a, std::vector<CCPoint> const& b) {
    std::vector polygons { a, b };
    std::optional<float> minA, maxA;
    std::optional<float> minB, maxB;
    for (size_t i = 0; i < polygons.size(); i++) {
        auto polygon = polygons[i];
        for (size_t i1 = 0; i1 < polygon.size(); i1 += 2) {
            auto i2 = (i1 + 2) % polygon.size();
            auto normal = CCPoint { polygon[i2].y - polygon[i1].y, polygon[i1].x - polygon[i2].x };
            minA = maxA = std::nullopt;
            for (size_t j = 0; j < a.size(); j += 2) {
                auto projected = normal.x * a[j].x + normal.y * a[j].y;
                if (!minA || projected < minA.value()) {
                    minA = projected;
                }
                if (!maxA || projected > maxA.value()) {
                    maxA = projected;
                }
            }
            minB = maxB = std::nullopt;
            for (size_t j = 0; j < b.size(); j += 2) {
                auto projected = normal.x * b[j].x + normal.y * b[j].y;
                if (!minB || projected < minB.value()) {
                    minB = projected;
                }
                if (!maxB || projected > maxB.value()) {
                    maxB = projected;
                }
            }
            if (maxA < minB || maxB < minA) {
                return false;
            }
        }
    }
    return true;
}
