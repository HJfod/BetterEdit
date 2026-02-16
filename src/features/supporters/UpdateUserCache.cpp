#include <features/supporters/Pro.hpp>
#include <server/Server.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/binding/GameManager.hpp>
#include <fmt/chrono.h>

using namespace pro;
using namespace pro::server;

class $modify(MenuLayer) {
    bool init() {
        if (!MenuLayer::init())
            return false;
        
        if (!HAS_PRO()) {
            return true;
        }

        // This is a stupid place to put this but oh well
        async::spawn(server::clearCaches());

        static bool CHECKED_TOKEN = false;
        if (!CHECKED_TOKEN) {
            CHECKED_TOKEN = true;
            if (auto token = pro::getProKey()) {
                async::spawn(
                    server::checkLicense(*token),
                    [token = *token](Result<std::string> result) {
                        if (result.isOk() && result.unwrap() != token.substr(token.find_last_of(':') + 1)) {
                            // Override key if the token has been banned
                            (void)saveProKey("");
                        }
                    }
                );
            }
        }

        using namespace std::chrono;

        auto lastUpdated = time_point<system_clock>(seconds(
            Mod::get()->template getSavedValue<size_t>("last-updated-gd-cache")
        ));
        if (system_clock::now() - lastUpdated < hours(4)) {
            return true;
        }
        Mod::get()->setSavedValue(
            "last-updated-gd-cache",
            static_cast<size_t>(duration_cast<seconds>(system_clock::now().time_since_epoch()).count())
        );
        
        if (auto token = getProKey()) {
            auto gjam = GJAccountManager::get();
            auto gm = GameManager::get();
            async::spawn(
                server::updateCachedInfo(*token, CachedGDInfo {
                    .gdAccountID = gjam->m_accountID,
                    .username = gjam->m_username,
                    .cubeID = gm->m_playerFrame,
                    .playerColor1 = gm->m_playerColor,
                    .playerColor2 = gm->m_playerColor2,
                    .glowColor = gm->m_playerGlow ? std::optional(gm->m_playerGlowColor.value()) : std::nullopt, 
                }),
                [](Result<std::monostate> result) {
                    if (result.isErr()) {
                        log::info("Unable to update GD account info cache: {}", result.unwrapErr());
                    }
                }
            );
        }

        return true;
    }
};
