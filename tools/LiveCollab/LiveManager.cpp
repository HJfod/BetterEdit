#include "LiveManager.hpp"

using namespace gd;
using namespace gdmake;
using namespace gdmake::extra;
using namespace cocos2d;
using namespace cocos2d::extension;

LiveManager* g_manager;

bool LiveManager::init() {
    return true;
}

LiveManager* LiveManager::sharedState() {
    return g_manager;
}

bool LiveManager::goLive(const char* serverURL, const char* roomName, const char* roomPassword) {
    this->getRequest(serverURL, {  }, &LiveManager::onResponse);
    
    return true;
}

void LiveManager::onResponse(CCHttpClient*, CCHttpResponse* res) {
    std::cout << res->getResponseCode() << "\n";
    std::cout << std::string(res->getResponseData()->begin(), res->getResponseData()->end()) << "\n";
    std::cout << res->getErrorBuffer() << "\n";
}

void LiveManager::getRequest(
    const char* url,
    std::vector<std::string> const& headers,
    void(LiveManager::* cb)(CCHttpClient*, CCHttpResponse*)
) {
    auto req = new CCHttpRequest();

    req->setUrl(url);
    req->setRequestType(CCHttpRequest::kHttpGet);
    req->setHeaders(headers);
    req->setResponseCallback(this, (SEL_HttpResponse)cb);

    CCHttpClient::getInstance()->send(req);
}

bool LiveManager::initGlobal() {
    g_manager = new LiveManager();

    if (g_manager && g_manager->init()) {
        g_manager->retain();
        return true;
    }

    CC_SAFE_DELETE(g_manager);
    return false;
}
