// Minimal helper used by Akhenaten after an in-app update download.
// The game process cannot overwrite its own executable while running, so it
// launches this tool, then exits. Flow:
//   wait for --pid → copy --staging into --install → start --restart → cleanup
//
// Built as a standalone binary with no game/SDL dependencies.

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <string>
#include <thread>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

namespace fs = std::filesystem;

namespace {

void log_line(const std::string &msg) {
    std::cerr << "[akhenaten-updater] " << msg << std::endl;
}

bool arg_value(int argc, char **argv, const char *key, std::string &out) {
    const std::string prefix = std::string(key) + "=";
    for (int i = 1; i < argc; ++i) {
        const char *a = argv[i];
        if (std::strncmp(a, prefix.c_str(), prefix.size()) == 0) {
            out = a + prefix.size();
            return !out.empty();
        }
        if (std::strcmp(a, key) == 0 && i + 1 < argc) {
            out = argv[++i];
            return !out.empty();
        }
    }
    return false;
}

bool wait_for_pid(unsigned long pid, int timeout_sec) {
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(timeout_sec);
#if defined(_WIN32)
    HANDLE h = OpenProcess(SYNCHRONIZE, FALSE, (DWORD)pid);
    if (!h) {
        // Already gone or inaccessible — treat as exited.
        return true;
    }
    const DWORD wait_ms = (DWORD)((std::max)(1000, timeout_sec * 1000));
    const DWORD rc = WaitForSingleObject(h, wait_ms);
    CloseHandle(h);
    return rc == WAIT_OBJECT_0;
#else
    while (std::chrono::steady_clock::now() < deadline) {
        if (kill((pid_t)pid, 0) != 0) {
            return true;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    return kill((pid_t)pid, 0) != 0;
#endif
}

bool copy_staging(const fs::path &staging, const fs::path &install) {
    std::error_code ec;
    if (!fs::exists(staging, ec) || !fs::is_directory(staging, ec)) {
        log_line("staging missing: " + staging.string());
        return false;
    }
    fs::create_directories(install, ec);

    for (auto it = fs::recursive_directory_iterator(staging, ec);
         !ec && it != fs::recursive_directory_iterator();
         it.increment(ec)) {
        const fs::path rel = fs::relative(it->path(), staging, ec);
        if (ec) {
            log_line("relative path failed: " + ec.message());
            return false;
        }
        const fs::path dst = install / rel;
        if (it->is_directory(ec)) {
            fs::create_directories(dst, ec);
            continue;
        }
        if (it->is_regular_file(ec)) {
            fs::create_directories(dst.parent_path(), ec);
            fs::copy_file(it->path(), dst, fs::copy_options::overwrite_existing, ec);
            if (ec) {
                log_line("copy failed " + it->path().string() + " -> " + dst.string() + ": " + ec.message());
                return false;
            }
#if !defined(_WIN32)
            fs::permissions(dst,
                fs::perms::owner_read | fs::perms::owner_write | fs::perms::owner_exec |
                    fs::perms::group_read | fs::perms::group_exec |
                    fs::perms::others_read | fs::perms::others_exec,
                fs::perm_options::add,
                ec);
#endif
        }
    }
    return !ec;
}

bool start_process(const fs::path &target) {
    if (target.empty()) {
        return false;
    }

#if defined(_WIN32)
    std::wstring wpath = target.wstring();
    STARTUPINFOW si{};
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi{};
    // CreateProcess wants a mutable command line.
    std::wstring cmd = L"\"" + wpath + L"\"";
    if (!CreateProcessW(wpath.c_str(), cmd.data(), nullptr, nullptr, FALSE, 0, nullptr,
            target.parent_path().wstring().c_str(), &si, &pi)) {
        log_line("CreateProcess failed (" + std::to_string(GetLastError()) + ")");
        return false;
    }
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    return true;
#else
    const std::string path = target.string();
#if defined(__APPLE__)
    if (path.size() >= 4 && path.compare(path.size() - 4, 4, ".app") == 0) {
        const pid_t pid = fork();
        if (pid == 0) {
            execlp("open", "open", path.c_str(), (char *)nullptr);
            _exit(127);
        }
        return pid > 0;
    }
#endif
    const pid_t pid = fork();
    if (pid == 0) {
        fs::current_path(target.parent_path());
        execl(path.c_str(), path.c_str(), (char *)nullptr);
        _exit(127);
    }
    return pid > 0;
#endif
}

void print_usage() {
    std::cerr
        << "Usage: akhenaten-updater --pid <pid> --staging <dir> --install <dir> "
           "--restart <exe|app> [--cleanup <dir>] [--timeout <sec>]\n";
}

} // namespace

int main(int argc, char **argv) {
    std::string pid_s, staging_s, install_s, restart_s, cleanup_s, timeout_s;
    if (!arg_value(argc, argv, "--pid", pid_s) || !arg_value(argc, argv, "--staging", staging_s)
        || !arg_value(argc, argv, "--install", install_s) || !arg_value(argc, argv, "--restart", restart_s)) {
        print_usage();
        return 2;
    }
    arg_value(argc, argv, "--cleanup", cleanup_s);
    if (!arg_value(argc, argv, "--timeout", timeout_s)) {
        timeout_s = "120";
    }

    const unsigned long pid = std::strtoul(pid_s.c_str(), nullptr, 10);
    const int timeout_sec = std::atoi(timeout_s.c_str());
    if (pid == 0) {
        log_line("invalid --pid");
        return 2;
    }

    log_line("waiting for pid " + pid_s);
    if (!wait_for_pid(pid, timeout_sec > 0 ? timeout_sec : 120)) {
        log_line("timed out waiting for process to exit");
        return 1;
    }

    log_line("copying " + staging_s + " -> " + install_s);
    if (!copy_staging(fs::path(staging_s), fs::path(install_s))) {
        return 1;
    }

    log_line("starting " + restart_s);
    if (!start_process(fs::path(restart_s))) {
        log_line("failed to restart game");
        // Still try cleanup
    }

    if (!cleanup_s.empty()) {
        std::error_code ec;
        fs::remove_all(fs::path(cleanup_s), ec);
        if (ec) {
            log_line("cleanup warning: " + ec.message());
        }
    }

    log_line("done");
    return 0;
}
