#include "sound.h"

#include "core/bstring.h"
#include "core/log.h"
#include "game/game_config.h"
#include "content/dir.h"
#include "content/mods.h"
#include "content/vfs.h"
#include "sound/channel.h"

#include <cstring>

vfs::path sound_manager_t::speech_filename(xstring filename) {
    pcstr filename_str = filename.c_str();
    vfs::path fs_path = filename_str;
    if (strncmp(filename_str, vfs::content_audio, strlen(vfs::content_audio)) != 0) {
        fs_path = vfs::path(vfs::content_audio, filename_str);
    }

    return fs_path.resolve();
}

bool sound_manager_t::speech_try_rel(pcstr rel, vfs::path &fs_path) {
    if (!rel || !*rel) {
        return false;
    }

    vfs::path with_audio = rel;
    if (strncmp(rel, vfs::content_audio, strlen(vfs::content_audio)) != 0) {
        with_audio = vfs::path(vfs::content_audio, rel);
    }

    vfs::path from_mods = mods_exist_audio(with_audio);
    if (!from_mods.empty()) {
        fs_path = from_mods;
        return true;
    }

    fs_path = vfs::path::resolve(with_audio);
    return !fs_path.empty() && vfs::file_exists(fs_path);
}

bool sound_manager_t::speech_file_exist(xstring filename, vfs::path &fs_path) {
    if (!game_features::gameopt_sound_speech_enabled) {
        return false;
    }

    pcstr filename_str = filename.empty() ? "" : filename.c_str();
    if (!filename_str || !*filename_str) {
        return false;
    }

    const vfs::path base = vfs::path(filename_str).basename();
    if (base.empty()) {
        return false;
    }

    bstring256 rel("voice/walker/", base.c_str());
    return speech_try_rel(rel.c_str(), fs_path);
}

bool sound_manager_t::speech_play_file(xstring filename, int volume) {
    vfs::path fs_path;
    if (!speech_file_exist(filename, fs_path)) {
        return false;
    }

    stop_channel(SOUND_CHANNEL_SPEECH);
    return play_file_on_channel(fs_path, SOUND_CHANNEL_SPEECH, game_features::gameopt_sound_speech_volume.to_int());
}

void sound_manager_t::speech_stop() {
    stop_channel(SOUND_CHANNEL_SPEECH);
}
