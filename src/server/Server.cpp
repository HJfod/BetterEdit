#include "Server.hpp"
#include <Geode/utils/ranges.hpp>
#include <Geode/utils/JsonValidation.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/binding/GameManager.hpp>
#include <Geode/binding/GJAccountManager.hpp>
#include <Geode/loader/Mod.hpp>
#include <arc/sync/Mutex.hpp>

using namespace pro;
using namespace pro::server;

template <class K, class V>
    requires std::equality_comparable<K> && std::copy_constructible<K>
class CacheMap final {
private:
    // I know this looks like a goofy choice over just
    // `std::unordered_map`, but hear me out:
    //
    // This needs preserved insertion order (so shrinking the cache
    // to match size limits doesn't just have to erase random
    // elements)
    //
    // If this used a map for values and another vector for storing
    // insertion order, it would have a pretty big memory footprint
    // (two copies of Query, one for order, one for map + two heap
    // allocations on top of that)
    //
    // In addition, it would be a bad idea to have a cache of 1000s
    // of items in any case (since that would likely take up a ton
    // of memory, which we want to avoid since it's likely many
    // crashes with the old index were due to too much memory
    // usage)
    //
    // Linear searching a vector of at most a couple dozen items is
    // lightning-fast (🚀), and besides the main performance benefit
    // comes from the lack of a web request - not how many extra
    // milliseconds we can squeeze out of a map access
    std::vector<std::pair<K, V>> m_values;
    size_t m_sizeLimit = 20;

public:
    std::optional<V> get(K const& key) {
        auto it = std::find_if(m_values.begin(), m_values.end(), [key](auto const& q) {
            return q.first == key;
        });
        if (it != m_values.end()) {
            return it->second;
        }
        return std::nullopt;
    }
    void add(K&& key, V&& value) {
        auto pair = std::make_pair(std::move(key), std::move(value));

        // Shift and replace last element if we're at cache size limit
        if (m_values.size() >= m_sizeLimit) {
            std::shift_left(m_values.begin(), m_values.end(), 1);
            m_values.back() = std::move(pair);
        }
        // Otherwise append at end
        else {
            m_values.emplace_back(std::move(pair));
        }
    }
    void remove(K const& key) {
        ranges::remove(m_values, [&key](auto const& q) { return q.first == key; });
    }
    void clear() {
        m_values.clear();
    }
    void limit(size_t size) {
        m_sizeLimit = size;
        m_values.clear();
    }
    size_t size() const {
        return m_values.size();
    }
    size_t limit() const {
        return m_sizeLimit;
    }
};

template <class F>
struct ExtractFun;

template <class V, class... Args>
struct ExtractFun<ServerRequest<V>(*)(Args...)> {
    using CacheKey = std::tuple<std::remove_cvref_t<Args>...>;
    using Value = V;

    template <class... CArgs>
    static CacheKey key(CArgs const&... args) {
        return std::make_tuple(args..., false);
    }
    template <class... CArgs>
    static ServerRequest<V> invoke(auto&& func, CArgs const&... args) {
        return func(args..., false);
    }
};

template <auto F>
class FunCache final {
public:
    using Extract  = ExtractFun<decltype(F)>;
    using CacheKey = typename Extract::CacheKey;
    using Value    = typename Extract::Value;

private:
    arc::Mutex<CacheMap<CacheKey, Value>> m_cache;

public:
    FunCache() = default;
    FunCache(FunCache const&) = delete;
    FunCache(FunCache&&) = delete;

    template <class... Args>
    arc::Future<Result<Value>> get(Args&&... args) {
        ARC_FRAME();
        auto key = Extract::key(args...);

        auto cache = co_await m_cache.lock();
        if (auto v = cache->get(key)) {
            co_return Ok(*v);
        }
        auto f = ARC_CO_UNWRAP(co_await Extract::invoke(F, std::forward<Args>(args)...));
        cache->add(std::move(key), Value{f});
        co_return Ok(f);
    }

    template <class... Args>
    arc::Future<> remove(Args const&... args) {
        auto cache = co_await m_cache.lock();
        cache->remove(Extract::key(args...));
    }

    arc::Future<size_t> size() {
        auto cache = co_await m_cache.lock();
        co_return cache->size();
    }
    arc::Future<> limit(size_t size) {
        auto cache = co_await m_cache.lock();
        cache->limit(size);
    }
    arc::Future<> clear() {
        auto cache = co_await m_cache.lock();
        cache->clear();
    }
};

template <auto F>
FunCache<F>& getCache() {
    static auto inst = FunCache<F>();
    return inst;
}

Result<CachedGDInfo> CachedGDInfo::parse(matjson::Value const& raw) {
    auto root = checkJson(raw, "CachedGDInfo");
    auto res = CachedGDInfo();

    root.needs("gd_account_id").into(res.gdAccountID);
    root.needs("username").into(res.username);
    root.needs("cube_id").into(res.cubeID);
    root.needs("player_color_1").into(res.playerColor1);
    root.needs("player_color_2").into(res.playerColor2);
    if (auto glow = root.needs("glow_color")) {
        if (!glow.json().isNull()) {
            glow.into(res.glowColor);
        }
    }

    return root.ok(res);
}
Result<Supporter> Supporter::parse(matjson::Value const& raw) {
    auto root = checkJson(raw, "Supporter");
    auto res = Supporter();

    GEODE_UNWRAP_INTO(res.info, CachedGDInfo::parse(root.needs("info").json()));
    root.needs("supported_amount").into(res.supportedAmount);

    return root.ok(res);
}
Result<Supporters> Supporters::parse(matjson::Value const& raw) {
    auto root = checkJson(raw, "Supporters");
    auto res = Supporters();

    for (auto&& item : root.needs("supporters").items()) {
        auto guy = Supporter::parse(item.json());
        if (guy) {
            res.supporters.push_back(guy.unwrap());
        }
        else {
            return Err(guy.unwrapErr());
        }
    }
    root.needs("total_supporter_count").into(res.totalSupporterCount);
    root.needs("total_public_supporter_count").into(res.totalPublicSupporterCount);

    return root.ok(res);
}
Result<ActivatedLicense> ActivatedLicense::parse(matjson::Value const& raw) {
    auto root = checkJson(raw, "ActivatedLicense");
    auto res = ActivatedLicense();

    root.needs("token").into(res.token);

    return root.ok(res);
}
Result<MyDevice> MyDevice::parse(matjson::Value const& raw) {
    auto root = checkJson(raw, "MyDevice");
    auto res = MyDevice();

    root.needs("device_name").into(res.deviceName);
    root.needs("device_id").into(res.deviceID);

    std::string platformID;
    root.needs("device_platform").into(platformID);
    res.devicePlatform = PlatformID::from(platformID);

    size_t supportedTimestamp;
    root.needs("activated_on").into(supportedTimestamp);
    res.activatedOn = std::chrono::time_point<std::chrono::system_clock>(
        std::chrono::seconds(supportedTimestamp)
    );

    return root.ok(res);
}
Result<MySupport> MySupport::parse(matjson::Value const& raw) {
    auto root = checkJson(raw, "MySupport");
    auto res = MySupport();

    GEODE_UNWRAP_INTO(res.gdInfo, CachedGDInfo::parse(root.needs("gd_info").json()));
    root.needs("showing_publicly").into(res.showingPublicly);
    size_t supportedTimestamp;
    root.needs("supported_on").into(supportedTimestamp);
    res.supportedOn = std::chrono::time_point<std::chrono::system_clock>(
        std::chrono::seconds(supportedTimestamp)
    );
    root.needs("supported_amount").into(res.supportedAmount);
    for (auto&& item : root.needs("devices").items()) {
        auto device = MyDevice::parse(item.json());
        if (device) {
            res.devices.push_back(device.unwrap());
        }
        else {
            return Err(device.unwrapErr());
        }
    }

    return root.ok(res);
}
Result<UpdatedDeviceInfo> UpdatedDeviceInfo::parse(matjson::Value const& raw) {
    auto root = checkJson(raw, "UpdatedDeviceInfo");
    auto res = UpdatedDeviceInfo();
    root.needs("device_name").into(res.deviceName);
    return root.ok(res);
}
Result<CreatedProductKey> CreatedProductKey::parse(matjson::Value const& raw) {
    auto root = checkJson(raw, "CreatedProductKey");
    auto res = CreatedProductKey();
    root.needs("created_license_key").into(res.key);
    return root.ok(res);
}

void CachedGDInfo::update(SimplePlayer* player) {
    player->updatePlayerFrame(cubeID, IconType::Cube);
    player->setColor(GameManager::get()->colorForIdx(playerColor1));
    player->setSecondColor(GameManager::get()->colorForIdx(playerColor2));
    if (glowColor) {
        player->setGlowOutline(GameManager::get()->colorForIdx(*glowColor));
    }
    else {
        player->disableGlowOutline();
    }
}

std::string server::getUserAgent() {
    return fmt::format(
        "BetterEdit (ver={};platform={})",
        Mod::get()->getVersion(),
        GEODE_PLATFORM_SHORT_IDENTIFIER
    );
}

template <class T>
ServerRequest<T> serverRequest(std::string method, web::WebRequest req, std::string subUrl) {
    web::WebResponse response = co_await req.send(method, BASE_URL + subUrl);
    if (response.ok()) {
        if constexpr (std::is_same_v<T, std::monostate>) {
            co_return Ok(std::monostate());
        }
        else if constexpr (std::is_same_v<T, std::string>) {
            auto json = response.json();
            if (!json) co_return Err("Invalid JSON response - this is a bug in BetterEdit!");
            auto value = json.unwrap();
            if (!value.isString()) co_return Err("Invalid response schema - this is a bug in BetterEdit!");
            co_return Ok(value.asString().unwrap());
        }
        else {
            auto json = response.json();
            if (!json) co_return Err("Invalid JSON response - this is a bug in BetterEdit!");
            auto res = T::parse(*json);
            if (!res) co_return Err("Invalid response schema - this is a bug in BetterEdit! (Error: {})", res.unwrapErr());
            co_return Ok(*res);
        }
    }
    else {
        if (auto json = response.json()) {
            auto value = json.unwrap();
            if (value.isString()) {
                co_return Err(value.asString().unwrap());
            }
        }
        co_return Err("Unknown error (code {})", response.code());
    }
}

ServerRequest<Supporters> server::getSupporters(size_t page, bool useCache) {
    if (useCache) {
        return getCache<getSupporters>().get(page);
    }
    auto req = web::WebRequest();
    req.userAgent(getUserAgent());
    req.param("page", page);
    req.param("per_page", SUPPORTERS_PER_PAGE);
    return serverRequest<Supporters>("GET", std::move(req), "/supporters");
}

ServerRequest<MySupport> server::getMySupport(std::string token, bool useCache) {
    if (useCache) {
        return getCache<getMySupport>().get(token);
    }
    auto req = web::WebRequest();
    req.userAgent(getUserAgent());
    req.param("token", token);
    return serverRequest<MySupport>("GET", std::move(req), "/my-support");
}

// No caching for POST requests

ServerRequest<ActivatedLicense> server::activateLicense(std::string key) {
    auto req = web::WebRequest();
    req.userAgent(getUserAgent());
    req.bodyJSON(matjson::makeObject({
        { "activation_key", key },
        { "for_gd_account_id", GJAccountManager::get()->m_accountID },
        { "device_platform", GEODE_PLATFORM_SHORT_IDENTIFIER },
    }));
    return serverRequest<ActivatedLicense>("POST", std::move(req), "/activate");
}

ServerRequest<std::monostate> server::deactivateLicense(std::string token, std::string deviceID) {
    auto req = web::WebRequest();
    req.userAgent(getUserAgent());
    req.bodyJSON(matjson::makeObject({
        { "token", token },
        { "device_id", deviceID },
    }));
    return serverRequest<std::monostate>("POST", std::move(req), "/deactivate");
}

ServerRequest<std::string> server::checkLicense(std::string token) {
    auto req = web::WebRequest();
    req.userAgent(getUserAgent());
    req.bodyJSON(matjson::makeObject({
        { "token", token },
    }));
    return serverRequest<std::string>("POST", std::move(req), "/check");
}

ServerRequest<CreatedProductKey> server::createNewLicense(std::string token) {
    // Invalidate caches
    co_await clearCaches();
    
    auto req = web::WebRequest();
    req.userAgent(getUserAgent());
    req.bodyJSON(matjson::makeObject({
        { "token", token },
        { "supported_amount", 0 },
    }));
    co_return co_await serverRequest<CreatedProductKey>("POST", std::move(req), "/new-license");
}

ServerRequest<std::monostate> server::updateCachedInfo(std::string token, CachedGDInfo info) {
    // Invalidate caches
    co_await clearCaches();

    auto req = web::WebRequest();
    req.userAgent(getUserAgent());
    req.bodyJSON(matjson::makeObject({
        { "token", token },
        { "info", matjson::makeObject({
            { "gd_account_id", info.gdAccountID },
            { "username", info.username },
            { "cube_id", info.cubeID },
            { "player_color_1", info.playerColor1 },
            { "player_color_2", info.playerColor2 },
            { "glow_color", info.glowColor },
        }) }
    }));
    co_return co_await serverRequest<std::monostate>("POST", std::move(req), "/gd-account-info");
}

ServerRequest<std::monostate> server::updateSupporter(std::string token, UpdateSupporter info) {
    // Invalidate caches
    co_await clearCaches();

    auto req = web::WebRequest();
    req.userAgent(getUserAgent());
    req.bodyJSON(matjson::makeObject({
        { "token", token },
        { "show_publicly", info.showPublicly },
    }));
    co_return co_await serverRequest<std::monostate>("POST", std::move(req), "/my-support");
}

ServerRequest<UpdatedDeviceInfo> server::updateDeviceInfo(std::string token, std::string deviceID, UpdateDeviceInfo info) {
    // Invalidate caches
    co_await clearCaches();

    auto req = web::WebRequest();
    req.userAgent(getUserAgent());
    req.bodyJSON(matjson::makeObject({
        { "token", token },
        { "device_id", deviceID },
        { "device_name", info.deviceName },
    }));
    co_return co_await serverRequest<UpdatedDeviceInfo>("POST", std::move(req), "/update-device");
}

arc::Future<void> server::clearCaches() {
    co_await getCache<&getSupporters>().clear();
    co_await getCache<&getMySupport>().clear();
}
