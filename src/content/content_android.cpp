#include "content.h"
#include "platform/platform.h"

#if defined(GAME_PLATFORM_ANDROID)

#include "platform/android/android.h"

#include <string>

FILE *vfs::platform_file_manager_open_file(std::string_view filename, pcstr mode) {
    const std::string path(filename);
    int fd = android_get_file_descriptor(path.c_str(), mode);
    if (!fd) {
        return NULL;
    }
    return fdopen(fd, mode);
}

bool vfs::platform_file_manager_remove_file(const char *filename) {
    return android_remove_file(filename);
}

#endif
