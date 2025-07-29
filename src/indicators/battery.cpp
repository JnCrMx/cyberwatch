#include "battery.hpp"

#include <LilyGoLib.h>
#include "cyberpunk_ui.hpp"

LV_FONT_DECLARE(lv_font_play_14);
LV_IMAGE_DECLARE(lv_img_battery);

namespace indicators {

constexpr const char* PREFS_KEY_SHOW_PERCENTAGE = "i_bat/percent";

battery::battery(Preferences& prefs, lv_obj_t* parent) : prefs(prefs) {
    container = std::unique_ptr<lv_obj_t, lv_obj_deleter>(lv_obj_create(parent));
    lv_obj_remove_style_all(container.get());

    bar = std::unique_ptr<lv_obj_t, lv_obj_deleter>(lv_bar_create(container.get()));
    lv_obj_set_size(bar.get(), 50, 25);
    lv_obj_set_style_pad_all(bar.get(), 5, LV_PART_MAIN);
    lv_obj_set_style_pad_right(bar.get(), 9, LV_PART_MAIN);

    lv_obj_set_style_bg_opa(bar.get(), LV_OPA_COVER, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(bar.get(), colors::PRIMARY, LV_PART_INDICATOR);
    lv_obj_set_style_radius(bar.get(), 0, LV_PART_INDICATOR);

    lv_obj_set_style_bg_opa(bar.get(), LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_bg_image_src(bar.get(), &lv_img_battery, LV_PART_MAIN);

    lv_bar_set_range(bar.get(), 0, 100);
    lv_bar_set_value(bar.get(), 100, LV_ANIM_OFF);

    percentage_label = std::unique_ptr<lv_obj_t, lv_obj_deleter>(lv_label_create(container.get()));
    lv_label_set_text(percentage_label.get(), "100%");
    lv_obj_set_style_text_color(percentage_label.get(), colors::PRIMARY, 0);
    lv_obj_set_style_text_align(percentage_label.get(), LV_TEXT_ALIGN_RIGHT, 0);

    lv_obj_align_to(percentage_label.get(), bar.get(), LV_ALIGN_LEFT_MID, -5, 0);
    lv_obj_align(bar.get(), LV_ALIGN_RIGHT_MID, 0, 0);

    if(!prefs.getBool(PREFS_KEY_SHOW_PERCENTAGE, true)) {
        lv_obj_add_flag(percentage_label.get(), LV_OBJ_FLAG_HIDDEN);
    }
    update();
}

void battery::update() {
    int battery_percent = instance.pmu.getBatteryPercent();
    bool is_charging = instance.pmu.isCharging();
    lv_bar_set_value(bar.get(), battery_percent, LV_ANIM_OFF);
    lv_label_set_text_fmt(percentage_label.get(), "%d%%", battery_percent);

    int size = lv_obj_get_width(bar.get());
    if(lv_obj_is_visible(percentage_label.get())) {
        size += lv_obj_get_width(percentage_label.get()) + 5;
    }
    lv_obj_set_size(container.get(), size, 25);
}

}
