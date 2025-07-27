#pragma once

#include <lvgl.hpp>
#include <memory>

LV_IMAGE_DECLARE(lv_img_battery);

class ui_battery {
    public:
        ui_battery(lv_obj_t* parent) {
            bar = std::unique_ptr<lv_obj_t, lv_obj_deleter>(lv_bar_create(parent));
            lv_obj_set_size(bar.get(), 50, 25);
            lv_obj_set_style_pad_all(bar.get(), 5, LV_PART_MAIN);
            lv_obj_set_style_pad_right(bar.get(), 9, LV_PART_MAIN);

            lv_obj_set_style_bg_opa(bar.get(), LV_OPA_COVER, LV_PART_INDICATOR);
            lv_obj_set_style_bg_color(bar.get(), lv_color_hex(0xe4381c), LV_PART_INDICATOR);
            lv_obj_set_style_radius(bar.get(), 0, LV_PART_INDICATOR);

            lv_obj_set_style_bg_opa(bar.get(), LV_OPA_COVER, LV_PART_MAIN);
            lv_obj_set_style_bg_image_src(bar.get(), &lv_img_battery, LV_PART_MAIN);

            lv_bar_set_range(bar.get(), 0, 100);
            lv_bar_set_value(bar.get(), 100, LV_ANIM_OFF);
        }
        void update(int battery_percent, bool is_charging) {
            lv_bar_set_value(bar.get(), battery_percent, LV_ANIM_OFF);
        }
        lv_obj_t* obj() {
            return bar.get();
        }
    private:
        std::unique_ptr<lv_obj_t, lv_obj_deleter> bar;
};
