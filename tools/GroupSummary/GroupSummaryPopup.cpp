#include "GroupSummaryPopup.hpp"

void GroupSummaryPopup::setup() {
    
}

void GroupSummaryPopup::onPage(CCObject* pSender) {
    this->m_nPage += as<int>(as<CCNode*>(pSender)->getUserData());

    if (this->m_nPage < 0)
        this->m_nPage = 0;
    if (this->m_nPage > static_cast<int>(this->m_vPages.size() - 1))
        this->m_nPage = this->m_vPages.size() - 1;

    int ix = 0;
    for (auto const& pages : m_vPages) {
        CCARRAY_FOREACH_B_TYPE(pages, node, CCNode) {
            node->setVisible(ix == this->m_nPage);
        }
        ix++;
    }
}

GroupSummaryPopup* GroupSummaryPopup::create(LevelEditorLayer* lel) {
    auto ret = new GroupSummaryPopup;

    if (
        ret &&
        (ret->m_pEditor = lel) &&
        ret->init(300.0f, 280.0f, "GJ_square01.png", "Group Summary")
    ) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return ret;
}
