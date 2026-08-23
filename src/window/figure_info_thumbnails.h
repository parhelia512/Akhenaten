#pragma once

#include "core/svector.h"
#include "figure/figure_type.h"

class figure_info_thumbnails {
public:
    void clear();
    void prepare_thumbnail(int index, figure_id id);
    void finish();
    int texture(int index) const;

private:
    struct figure_small_image {
        int image_id = 0;

        figure_small_image(figure_small_image &o);
        explicit figure_small_image(figure_id figure_id);
        ~figure_small_image();
    };

    svector<figure_small_image, 7> figure_images_;
};

extern figure_info_thumbnails g_figure_info_thumbnails;
