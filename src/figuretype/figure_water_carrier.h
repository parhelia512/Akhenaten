#pragma once

#include "figure/figure.h"

class figure_water_carrier : public figure_impl {
public:
    figure_water_carrier(figure *f) : figure_impl(f) {}

    virtual void on_create() override {}
    virtual void figure_action() override;
    virtual e_figure_sound phrase() const override { return {FIGURE_WATER_CARRIER, "water"}; }
    virtual e_overlay get_overlay() const override { return OVERLAY_FIRE; }
    virtual sound_key phrase_key() const override;
    virtual int provide_service() override;
};