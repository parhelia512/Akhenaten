#include "core/app.h"
#include "core/profiler.h"
#include "core/xvalue.h"
#include "game/game_events.h"
#include "js/js_events.h"
#include "js/js_game.h"
#include "net/http_client.h"
#include "core/log.h"
#include "game/game.h"

#include <regex>
#include <string>

struct event_github_changelog_loaded { xstring change_log; };
struct event_github_totals_commits_loaded { int current_commit; };

static constexpr pcstr GITHUB_CHANGELOG_URL =
    "https://raw.githubusercontent.com/dalerank/Akhenaten/master/changelog.txt";

// Main menu init can re-fire; only hit GitHub once per process.
struct github_cache_t {
    bool changelog_attempted = false;
    xstring changelog;
    bool commits_attempted = false;
    int commits = -1;
};

bool github_is_active() {
#ifdef GAME_HAVE_CURL
    return true;
#else
    return false;
#endif
}

xstring github_download_changelog() {
    auto &cache = xvalue<github_cache_t>::get();
    if (cache.changelog_attempted) {
        return cache.changelog;
    }
    cache.changelog_attempted = true;

    const http_get_result response = http_get(GITHUB_CHANGELOG_URL, 10, false);
    if (response.ok && !response.body.empty()) {
        logs::info("Changelog downloaded successfully (%zu bytes)", response.body.size());
        cache.changelog = xstring(response.body.c_str());
        return cache.changelog;
    }

    if (response.http_code != 0) {
        logs::error("Failed to download changelog (HTTP code: %ld)", response.http_code);
    }

#ifndef GAME_HAVE_CURL
    cache.changelog = xstring("Changelog download not supported on this platform (libcurl is not available).");
#else
    cache.changelog = xstring("Failed to download changelog from GitHub.");
#endif
    return cache.changelog;
}

int github_get_total_commits(pcstr owner, pcstr repo) {
    if (!owner || !*owner || !repo || !*repo) {
        return -1;
    }

    auto &cache = xvalue<github_cache_t>::get();
    if (cache.commits_attempted) {
        return cache.commits;
    }
    cache.commits_attempted = true;

    std::string url = "https://api.github.com/repos/";
    url += owner;
    url += "/";
    url += repo;
    url += "/commits?per_page=1";

    const http_get_result response = http_get(url.c_str(), 10, true);

#ifndef GAME_HAVE_CURL
    (void)response;
    cache.commits = -1;
    return -1;
#else
    if (!response.ok) {
        logs::error("Unable to fetch commits (HTTP code: %ld)", response.http_code);
        cache.commits = -1;
        return -1;
    }

    const std::regex link_regex(R"(<([^>]+)>; rel="last")");
    std::smatch match;

    int total = 1;
    if (std::regex_search(response.headers, match, link_regex)) {
        const std::string last_page_url = match[1].str();
        const std::regex page_regex(R"(&page=(\d+))");
        if (std::regex_search(last_page_url, match, page_regex)) {
            total = std::stoi(match[1].str());
        }
    }

    cache.commits = total;
    return total;
#endif
}

void github_download_changelog_async() {
    game.mt.detach_task([]() {
        const xstring changelog = github_download_changelog();
        game.add_frame_end_event([changelog]() {
            events::emit(event_github_changelog_loaded{ changelog });
        });
    });
}

void github_get_total_commits_async(pcstr owner, pcstr repo) {
    const xstring owner_str(owner ? owner : "");
    const xstring repo_str(repo ? repo : "");

    game.mt.detach_task([owner_str, repo_str]() {
        const int current_commit = github_get_total_commits(owner_str.c_str(), repo_str.c_str());
        game.add_frame_end_event([current_commit]() {
            events::emit(event_github_totals_commits_loaded{ current_commit });
        });
    });
}

void ANK_REGISTER_APPLICATION_MODULE(register_github_module) {
}

ANK_FUNCTION(github_is_active)
ANK_FUNCTION(github_download_changelog_async)
ANK_FUNCTION_2(github_get_total_commits_async)

ANK_SCRIPT_EVENT(event_github_changelog_loaded, change_log)
ANK_SCRIPT_EVENT(event_github_totals_commits_loaded, current_commit)
