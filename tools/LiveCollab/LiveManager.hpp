#include "../../BetterEdit.hpp"
#include <network/HttpClient.h>

class LiveManager : public cocos2d::CCObject {
    protected:
        bool m_bIsLive = false;

        bool init();

        void getRequest(
            const char* url,
            std::vector<std::string> const& headers, 
            void(LiveManager::*)(cocos2d::extension::CCHttpClient*, cocos2d::extension::CCHttpResponse*)
        );
        void onResponse(cocos2d::extension::CCHttpClient*, cocos2d::extension::CCHttpResponse*);

    public:
        static bool initGlobal();
        static LiveManager* sharedState();

        bool goLive(const char* serverURL, const char* roomName, const char* roomPassword = nullptr);
};
