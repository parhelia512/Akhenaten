#include "movie_writer.h"

#include "core/log.h"

#ifdef GAME_HAS_VIDEO_RECORDING

#define MINIMP4_IMPLEMENTATION
#include "minimp4.h"

#include <wels/codec_api.h>
#include <wels/codec_app_def.h>
#include <wels/codec_def.h>

#include <algorithm>
#include <cstring>

namespace {

int mp4_write_callback(int64_t offset, const void *buffer, size_t size, void *token) {
    FILE *f = (FILE *)token;
    if (fseek(f, (long)offset, SEEK_SET) != 0) {
        return 1;
    }
    return fwrite(buffer, 1, size, f) != size;
}

} // namespace

void MovieWriter::convertBGRAtoYUV420(const uint8_t *bgra, uint8_t *yuv) {
    uint8_t *y_plane = yuv;
    uint8_t *u_plane = yuv + width * height;
    uint8_t *v_plane = yuv + width * height * 5 / 4;

    for (unsigned int y = 0; y < height; y++) {
        for (unsigned int x = 0; x < width; x++) {
            const uint8_t *px = &bgra[(y * src_stride + x) * 4];
            int b = px[0];
            int g = px[1];
            int r = px[2];

            int y_val = (int)(0.299 * r + 0.587 * g + 0.114 * b);
            int u_val = (int)(-0.169 * r - 0.331 * g + 0.5 * b + 128);
            int v_val = (int)(0.5 * r - 0.419 * g - 0.081 * b + 128);

            y_plane[y * width + x] = (uint8_t)std::clamp(y_val, 0, 255);

            if ((x % 2) == 0 && (y % 2) == 0) {
                u_plane[(y / 2) * (width / 2) + (x / 2)] = (uint8_t)std::clamp(u_val, 0, 255);
                v_plane[(y / 2) * (width / 2) + (x / 2)] = (uint8_t)std::clamp(v_val, 0, 255);
            }
        }
    }
}

MovieWriter::MovieWriter(const std::string &filename_, unsigned int width_, unsigned int height_, int frameRate_)
    : width((uint16_t)(width_ & ~1u)), height((uint16_t)(height_ & ~1u)), src_stride((uint16_t)width_),
      frameRate((uint16_t)std::max(1, frameRate_)), filename(filename_) {
    if (width < 2 || height < 2) {
        logs::info("MovieWriter: invalid size %ux%u", width_, height_);
        return;
    }

    yuv_buffer.resize((size_t)width * height * 3 / 2);

    std::string output_filename = filename;
    if (output_filename.size() < 4 || output_filename.substr(output_filename.size() - 4) != ".mp4") {
        output_filename = filename + ".mp4";
    }
    filename = output_filename;

    output_file = fopen(output_filename.c_str(), "wb");
    if (!output_file) {
        logs::info("MovieWriter: Failed to open output file: %s", output_filename.c_str());
        return;
    }

    if (WelsCreateSVCEncoder(&encoder) != 0 || !encoder) {
        logs::info("MovieWriter: WelsCreateSVCEncoder failed");
        fclose(output_file);
        output_file = nullptr;
        return;
    }

    SEncParamExt param;
    memset(&param, 0, sizeof(param));
    encoder->GetDefaultParams(&param);
    param.iUsageType = SCREEN_CONTENT_REAL_TIME;
    param.fMaxFrameRate = (float)frameRate;
    param.iPicWidth = width;
    param.iPicHeight = height;
    // Screen capture needs more bits than camera video; keep a high floor so
    // low-fps city recordings are not almost entirely frame-skipped.
    param.iTargetBitrate = std::max(2500000, (int)width * (int)height * (int)frameRate / 4);
    param.iMaxBitrate = param.iTargetBitrate * 2;
    param.iRCMode = RC_BITRATE_MODE;
    param.bEnableFrameSkip = false;
    param.uiIntraPeriod = (unsigned int)frameRate * 2;
    param.eSpsPpsIdStrategy = CONSTANT_ID;
    param.bPrefixNalAddingCtrl = false;
    param.iSpatialLayerNum = 1;
    param.sSpatialLayers[0].iVideoWidth = width;
    param.sSpatialLayers[0].iVideoHeight = height;
    param.sSpatialLayers[0].fFrameRate = (float)frameRate;
    param.sSpatialLayers[0].iSpatialBitrate = param.iTargetBitrate;
    param.sSpatialLayers[0].iMaxSpatialBitrate = param.iMaxBitrate;

    if (encoder->InitializeExt(&param) != 0) {
        logs::info("MovieWriter: InitializeExt failed");
        WelsDestroySVCEncoder(encoder);
        encoder = nullptr;
        fclose(output_file);
        output_file = nullptr;
        return;
    }

    int video_format = videoFormatI420;
    encoder->SetOption(ENCODER_OPTION_DATAFORMAT, &video_format);

    mux = MP4E_open(0, 0, output_file, mp4_write_callback);
    if (!mux) {
        logs::info("MovieWriter: MP4E_open failed");
        encoder->Uninitialize();
        WelsDestroySVCEncoder(encoder);
        encoder = nullptr;
        fclose(output_file);
        output_file = nullptr;
        return;
    }

    auto *writer = new mp4_h26x_writer_t();
    memset(writer, 0, sizeof(*writer));
    if (mp4_h26x_write_init(writer, (MP4E_mux_t *)mux, width, height, 0) != MP4E_STATUS_OK) {
        logs::info("MovieWriter: mp4_h26x_write_init failed");
        delete writer;
        MP4E_close((MP4E_mux_t *)mux);
        mux = nullptr;
        encoder->Uninitialize();
        WelsDestroySVCEncoder(encoder);
        encoder = nullptr;
        fclose(output_file);
        output_file = nullptr;
        return;
    }
    mp4wr = writer;

    initialized = true;
    logs::info("MovieWriter: Initialized OpenH264 for %dx%d @ %d fps -> %s", width, height, frameRate,
               filename.c_str());
}

void MovieWriter::addFrame(const uint8_t *pixels) {
    if (!initialized || !encoder || !mp4wr || !pixels) {
        return;
    }

    convertBGRAtoYUV420(pixels, yuv_buffer.data());

    SSourcePicture pic;
    memset(&pic, 0, sizeof(pic));
    pic.iPicWidth = width;
    pic.iPicHeight = height;
    pic.iColorFormat = videoFormatI420;
    pic.iStride[0] = width;
    pic.iStride[1] = width / 2;
    pic.iStride[2] = width / 2;
    pic.pData[0] = yuv_buffer.data();
    pic.pData[1] = yuv_buffer.data() + width * height;
    pic.pData[2] = yuv_buffer.data() + width * height * 5 / 4;

    SFrameBSInfo info;
    memset(&info, 0, sizeof(info));
    const int enc_ret = encoder->EncodeFrame(&pic, &info);
    if (enc_ret != 0) {
        logs::info("MovieWriter: EncodeFrame failed (%d)", enc_ret);
        return;
    }
    if (info.eFrameType == videoFrameTypeSkip || info.iLayerNum <= 0) {
        skipped_frames++;
        return;
    }

    // OpenH264 emits contiguous Annex-B for this picture starting at layer 0.
    int total_size = 0;
    for (int layer = 0; layer < info.iLayerNum; layer++) {
        const SLayerBSInfo &li = info.sLayerInfo[layer];
        for (int nal = 0; nal < li.iNalCount; nal++) {
            total_size += li.pNalLengthInByte[nal];
        }
    }
    if (total_size <= 0 || !info.sLayerInfo[0].pBsBuf) {
        skipped_frames++;
        return;
    }

    const unsigned duration_90k = 90000u / (unsigned)frameRate;
    auto *writer = (mp4_h26x_writer_t *)mp4wr;
    if (mp4_h26x_write_nal(writer, info.sLayerInfo[0].pBsBuf, total_size, duration_90k) != MP4E_STATUS_OK) {
        logs::info("MovieWriter: mp4_h26x_write_nal failed on frame %u", iframe);
        return;
    }

    iframe++;
    if ((iframe % (unsigned)frameRate) == 0) {
        logs::info("MovieWriter: encoded %u frames (%u skipped)", iframe, skipped_frames);
    }
}

MovieWriter::~MovieWriter() {
    if (mp4wr) {
        auto *writer = (mp4_h26x_writer_t *)mp4wr;
        if (initialized) {
            mp4_h26x_write_close(writer);
        }
        delete writer;
        mp4wr = nullptr;
    }
    if (mux) {
        MP4E_close((MP4E_mux_t *)mux);
        mux = nullptr;
    }
    if (encoder) {
        encoder->Uninitialize();
        WelsDestroySVCEncoder(encoder);
        encoder = nullptr;
    }
    if (output_file) {
        fclose(output_file);
        output_file = nullptr;
    }

    if (initialized) {
        logs::info("MovieWriter: Finished writing %u frames (%u skipped) to %s", iframe, skipped_frames,
                   filename.c_str());
    }
}

#endif // GAME_HAS_VIDEO_RECORDING
