#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include <stdio.h>
#include "core/vec2i.h"

#ifdef GAME_HAS_VIDEO_RECORDING

struct ISVCEncoder;

class MovieWriter {
    const uint16_t width;
    const uint16_t height;
    const uint16_t src_stride; // pixels per row in input BGRA buffer (may be odd)
    uint32_t iframe = 0;
    uint32_t skipped_frames = 0;
    uint16_t frameRate = 25;

    ISVCEncoder *encoder = nullptr;
    void *mux = nullptr;   // MP4E_mux_t*
    void *mp4wr = nullptr; // mp4_h26x_writer_t*
    FILE *output_file = nullptr;

    std::string filename;
    std::vector<uint8_t> yuv_buffer;
    bool initialized = false;

    void convertBGRAtoYUV420(const uint8_t *bgra, uint8_t *yuv);

public:
    MovieWriter(const std::string &filename, unsigned int width, unsigned int height, int frameRate = 25);
    void addFrame(const uint8_t *pixels);
    bool ok() const {
        return initialized;
    }
    vec2i frameSize() const {
        return {(int)width, (int)height};
    }
    ~MovieWriter();
};

#else

class MovieWriter {
public:
    MovieWriter(const std::string &, unsigned int, unsigned int, int) {}
    void addFrame(const uint8_t *) {}
    bool ok() const {
        return true;
    }
    vec2i frameSize() const {
        return {0, 0};
    }
    ~MovieWriter() = default;
};

#endif // GAME_HAS_VIDEO_RECORDING
