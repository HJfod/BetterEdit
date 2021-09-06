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
void SuperMouseDelegate::mouseDownSuper(MouseButton, CCPoint const&) {}
void SuperMouseDelegate::mouseUpSuper(MouseButton, CCPoint const&) {}
void SuperMouseDelegate::mouseMoveSuper(CCPoint const&) {}
void SuperMouseDelegate::setSuperMouseHitSize(CCSize const& size) {
    this->m_obSuperMouseHitSize = size;
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

    if (size == CCSizeZero)
        size = delegate->getScaledContentSize();

    auto pos = delegate->getPosition();

    auto rect = CCRect {
        pos.x - size.width / 2,
        pos.y - size.height / 2,
        size.width,
        size.height
    };

    return rect.containsPoint(mpos);
}

void SuperMouseManager::dispatchClickEvent(MouseButton btn, bool down, CCPoint const& pos) {
    for (auto const& d : m_vDelegates) {
        if (delegateIsHovered(d, pos))
            if (down) {
                d->mouseDownSuper(btn, pos);
            } else {
                d->mouseUpSuper(btn, pos);
            }
    }
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
