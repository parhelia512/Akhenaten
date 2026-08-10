#pragma once

#include "core/xstring.h"

#include <cstdint>
#include <vector>

enum e_memory_bucket : uint8_t {
    MEMORY_PACK_TEXTURES = 0,
    MEMORY_BUCKET_COUNT
};

struct memory_pack_alloc_t {
    xstring name;
    int width = 0;
    int height = 0;
    int64_t bytes = 0;
};

struct memory_manager_t {
    void alloc(e_memory_bucket bucket, int64_t bytes);
    void free(e_memory_bucket bucket, int64_t bytes);
    void set(e_memory_bucket bucket, int64_t bytes);

    int64_t used(e_memory_bucket bucket) const;
    int64_t total() const;
    static const char *bucket_name(e_memory_bucket bucket);

    void track_pack_texture(pcstr name, int width, int height, bool allocate);
    const std::vector<memory_pack_alloc_t> &pack_texture_allocs() const { return _pack_allocs; }
    void log() const;

private:
    int64_t _used[MEMORY_BUCKET_COUNT] = {};
    std::vector<memory_pack_alloc_t> _pack_allocs;
};

extern memory_manager_t g_memory;
