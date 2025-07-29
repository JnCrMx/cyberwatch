#pragma once

#include <lvgl.h>

#define lv_color_hex(c) lv_color_t{.blue = ((c) >> 0) & 0xff, .green = ((c) >> 8) & 0xff, .red = ((c) >> 16) & 0xff}
namespace colors {
    constexpr auto PRIMARY = lv_color_hex(0xe4381c);
    constexpr auto BACKGROUND = lv_color_hex(0x0b0c16);
}
#undef lv_color_hex

void cyberpunk_decoration_cb(lv_event_t * e);

extern lv_style_t cyberpunk_style;
extern lv_theme_t cyberpunk_theme;

void init_cyberpunk_theme();
