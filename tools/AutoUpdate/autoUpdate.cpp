#include "autoUpdate.hpp"
#undef snprintf
#include "../../include/json.hpp"

using namespace nlohmann;

static int getVersionInt(std::string const& verstr) {
    std::string ns;
    int n = 0;
    int v = 2;

    for (auto const& c : verstr)
        if (isdigit(c))
            ns += c;
        else if (c == '.') {
            n += std::stoi(ns) << (v-- * 10);
            ns = "0";
        }
    
    return n;
}

void UpdateChecker::onCheckLatestRelease(CCHttpClient*, CCHttpResponse* res) {
    std::cout << res->getResponseCode() << "\n";
    std::cout << res->getErrorBuffer() << "\n";

    if (res->getResponseCode() == 200) {
        auto data = res->getResponseData();

        std::string jsonData (data->begin(), data->end());

        json jsond (jsonData);

        std::cout << jsond.dump(4) << "\n";

        auto oldVer = getVersionInt(jsond["tag_name"]);
        auto newVer = getVersionInt("GDMAKE_PROJECT_VERSION");

    } else
        BetterEdit::sharedState()->scheduleError(
            BetterEdit::kScheduleTimeMenuLayer,
            "<cr>Unable to check updates</c>: Request returned " +
            std::to_string(res->getResponseCode())
        );
}

void checkForUpdates() {
    auto checker = new UpdateChecker;
    checker->autorelease();
    checker->retain();

    auto req = new CCHttpRequest();
    req->setUrl("http://api.github.com/repos/HJfod/BetterEdit/releases/latest");
    req->setResponseCallback(
        checker, (SEL_HttpResponse)&UpdateChecker::onCheckLatestRelease
    );
    req->setRequestType(CCHttpRequest::kHttpGet);
    req->setRequestData("", 0);
    req->setHeaders({ "'User-Agent': 'request'" });

    CCHttpClient::getInstance()->send(req);

    req->release();
}
