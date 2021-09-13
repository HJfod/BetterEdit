#include "SuperMouseManager.hpp"

SuperMouseManager* g_manager;

bool operator==(CCSize const& size, CCSize const& size2) {
    return
        size.width == size2.width &&
        size.height == size2.height;
}

bool operator!=(CCPoint const& size, CCPoint const& size2) {
    return
        size.x == size2.x &&
        size.y == size2.y;
}

void SuperMouseDelegate::mouseEnterSuper(CCPoint const&) {}
void SuperMouseDelegate::mouseLeaveSuper(CCPoint const&) {}
bool SuperMouseDelegate::mouseDownSuper(MouseButton, CCPoint const&) { return false; }
bool SuperMouseDelegate::mouseUpSuper(MouseButton, CCPoint const&) { return false; }
void SuperMouseDelegate::mouseMoveSuper(CCPoint const&) {}
void SuperMouseDelegate::mouseDownOutsideSuper(MouseButton, CCPoint const&) {}
bool SuperMouseDelegate::mouseScrollSuper(float, float) { return false; }
void SuperMouseDelegate::mouseScrollOutsideSuper(float, float) {}
void SuperMouseDelegate::setSuperMouseHitSize(CCSize const& size) {
    this->m_obSuperMouseHitSize = size;
}
void SuperMouseDelegate::setSuperMouseHitOffset(CCPoint const& pos) {
    this->m_obSuperMouseHitOffset = pos;
}

SuperMouseDelegate::SuperMouseDelegate() {
    SuperMouseManager::get()->pushDelegate(this);
}

SuperMouseDelegate::~SuperMouseDelegate() {
    SuperMouseManager::get()->popDelegate(this);
}


bool SuperMouseManager::init() {
    return true;
}

bool SuperMouseManager::initGlobal() {
    if (!g_manager) {
        g_manager = new SuperMouseManager;
        if (!g_manager || !g_manager->init()) {
            CC_SAFE_DELETE(g_manager);
            return false;
        }
    }
    return true;
}

SuperMouseManager* SuperMouseManager::get() {
    return g_manager;
}

void SuperMouseManager::pushDelegate(SuperMouseDelegate* delegate) {
    this->m_vDelegates.push_back(delegate);
}

void SuperMouseManager::popDelegate(SuperMouseDelegate* delegate) {
    this->m_vDelegates.erase(std::remove(
        this->m_vDelegates.begin(), this->m_vDelegates.end(), delegate
    ), this->m_vDelegates.end());
}

bool SuperMouseManager::delegateIsHovered(SuperMouseDelegate* delegate, CCPoint const& mpos) {
    auto size = delegate->m_obSuperMouseHitSize;

    auto p = dynamic_cast<CCNode*>(delegate);

    if (p && size == CCSizeZero)
        size = p->getScaledContentSize();

    auto pos = p ? p->getPosition() : CCPointZero;

    if (p && p->getParent())
        pos = p->getParent()->convertToWorldSpace(p->getPosition());
    
    pos = pos + delegate->m_obSuperMouseHitOffset;

    auto rect = CCRect {
        pos.x - size.width / 2,
        pos.y - size.height / 2,
        size.width,
        size.height
    };

    return rect.containsPoint(mpos);
}

bool SuperMouseManager::dispatchClickEvent(MouseButton btn, bool down, CCPoint const& pos) {
    auto ret = false;
    for (auto const& d : m_vDelegates) {
        if (delegateIsHovered(d, pos)) {
            if (down) {
                if (d->mouseDownSuper(btn, pos))
                    ret = true;
            } else {
                if (d->mouseUpSuper(btn, pos))
                    ret = true;
            }
        } else {
            d->mouseDownOutsideSuper(btn, pos);
        }
    }
    return ret;
}

bool SuperMouseManager::dispatchScrollEvent(float y, float x, CCPoint const& pos) {
    auto ret = false;
    for (auto const& d : m_vDelegates) {
        if (delegateIsHovered(d, pos)) {
            if (d->mouseScrollSuper(y, x))
                ret = true;
        } else
            d->mouseScrollOutsideSuper(y, x);
    }
    return ret;
}

void SuperMouseManager::dispatchMoveEvent(CCPoint const& pos) {
    if (this->m_obLastPosition != pos) {
        for (auto const& d : m_vDelegates) {
            auto hover = this->delegateIsHovered(d, pos);

            if (d->m_bSuperMouseHovered != hover) {
                d->m_bSuperMouseHovered = hover;

                if (hover) {
                    d->mouseEnterSuper(pos);
                } else {
                    d->mouseLeaveSuper(pos);
                }
            }

            if (hover) {
                d->mouseMoveSuper(pos);
            }
        }
    }

    this->m_obLastPosition = pos;
}
