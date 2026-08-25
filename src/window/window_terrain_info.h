#pragma once

#include "window_info.h"
#include "city/object_info.h"
#include "core/typename.h"

struct terrain_info_window : public common_info_window {
    svector<terrain_info_type, 8> related_terrain;
    xstring help_id;

    terrain_info_window();

    using widget::archive_load;
    virtual void archive_load(archive arch) override;

    virtual int window_info_handle_mouse(const mouse *m, object_info &c) override {
        return 0;
    }

    virtual void window_info_background(object_info &c) override;

    virtual void window_info_foreground(object_info &c) override {
        draw();
    }

    virtual void update(object_info &c) override;

    virtual void init(object_info &c) override;
    virtual bool check(object_info &c) override;
};

template<typename T>
struct terrain_info_window_t : public terrain_info_window {
    virtual xstring section() const override {
        static type_name_holder<T> _impl;
        static xstring _section = type_simplified_name(_impl.value.data());
        return _section;
    }
};
