#pragma once

#include <features/supporters/Pro.hpp>
#include <Geode/ui/Popup.hpp>
#include <server/Server.hpp>
#include <Geode/ui/LoadingSpinner.hpp>

using namespace geode::prelude;

namespace pro {
    class SupportersPopup : public Popup {
    protected:
        async::TaskHolder<Result<server::Supporters>> m_reqListener;
        async::TaskHolder<Result<server::MySupport>> m_mySupportListener;
        async::TaskHolder<Result<std::monostate>> m_updateSupportListener;
        CCMenuItemSpriteExtra* m_prevPageBtn;
        CCMenuItemSpriteExtra* m_nextPageBtn;
        CCLabelBMFont* m_pageLabel;
        LoadingSpinner* m_loadingCircle;
        CCLabelBMFont* m_errorLabel;
        CCNode* m_supportersList;
        size_t m_page = 0;
        size_t m_maxPage = 0;
        CCMenu* m_mySupportMenu = nullptr;

        bool init();
        void loadPage(size_t page);
        void onLoadPage(Result<server::Supporters> result);
        void onLoadMySupport(Result<server::MySupport> result);
        void updatePageInfo();

        void onPage(CCObject*);
        void onSupporter(CCObject*);
        void onShowMe(CCObject*);

    public:
        static SupportersPopup* create();
    };
}
