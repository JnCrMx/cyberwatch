#include "flashlight.hpp"

#include "cyberpunk_ui.hpp"
#include "hardware.hpp"

LV_FONT_DECLARE(lv_font_play_25);
LV_IMAGE_DECLARE(lv_img_flashlight);

namespace apps {

flashlight::flashlight(Preferences& prefs, lv_obj_t* parent) {
    lv_obj_t* button = lv_button_create(parent);
    lv_obj_set_size(button, 150, 150);
    lv_obj_set_style_bg_image_src(button, &lv_img_flashlight, LV_PART_MAIN);
    lv_obj_set_style_bg_image_recolor(button, colors::PRIMARY, LV_PART_MAIN);
    lv_obj_set_style_bg_img_recolor_opa(button, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(button, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_color(button, colors::PRIMARY, LV_PART_MAIN);
    lv_obj_set_style_outline_width(button, 0, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(button, 0, LV_PART_MAIN);
    cyberpunk_decoration_apply(button);
    lv_obj_center(button);

    lv_obj_t* white = lv_obj_create(parent);
    lv_obj_remove_style_all(white);
    lv_obj_set_style_bg_color(white, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(white, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_size(white, LV_PCT(100), LV_PCT(100));
    lv_obj_add_flag(white, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(white, LV_OBJ_FLAG_FLOATING);
    lv_obj_add_flag(white, LV_OBJ_FLAG_HIDDEN);

    lv_obj_add_event_cb(button, [](lv_event_t* e) {
        haptic_feedback();
        lv_obj_t* white = static_cast<lv_obj_t*>(lv_event_get_user_data(e));
        lv_obj_clear_flag(white, LV_OBJ_FLAG_HIDDEN);
        block_dim();
        block_sleep_mode();
        lock_tiles();
    }, LV_EVENT_CLICKED, white);
    lv_obj_add_event_cb(white, [](lv_event_t* e) {
        haptic_feedback();
        lv_obj_t* white = static_cast<lv_obj_t*>(lv_event_get_user_data(e));
        lv_obj_add_flag(white, LV_OBJ_FLAG_HIDDEN);
        unblock_dim();
        unblock_sleep_mode();
        unlock_tiles();
    }, LV_EVENT_CLICKED, white);
}

}
