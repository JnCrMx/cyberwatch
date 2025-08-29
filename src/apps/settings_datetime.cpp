#include "settings_datetime.hpp"

#include <LilyGoLib.h>
#include "cyberpunk_ui.hpp"
#include "hardware.hpp"
#include "prefs.hpp"

LV_FONT_DECLARE(lv_font_play_18);
LV_FONT_DECLARE(lv_font_play_25);

namespace apps {

constexpr const char* numbers_00_to_23 =
    "00" "\n" "01" "\n" "02" "\n" "03" "\n" "04" "\n"
    "05" "\n" "06" "\n" "07" "\n" "08" "\n" "09" "\n"
    "10" "\n" "11" "\n" "12" "\n" "13" "\n" "14" "\n"
    "15" "\n" "16" "\n" "17" "\n" "18" "\n" "19" "\n"
    "20" "\n" "21" "\n" "22" "\n" "23";
constexpr const char* numbers_00_to_59 =
    "00" "\n" "01" "\n" "02" "\n" "03" "\n" "04" "\n"
    "05" "\n" "06" "\n" "07" "\n" "08" "\n" "09" "\n"
    "10" "\n" "11" "\n" "12" "\n" "13" "\n" "14" "\n"
    "15" "\n" "16" "\n" "17" "\n" "18" "\n" "19" "\n"
    "20" "\n" "21" "\n" "22" "\n" "23" "\n" "24" "\n"
    "25" "\n" "26" "\n" "27" "\n" "28" "\n" "29" "\n"
    "30" "\n" "31" "\n" "32" "\n" "33" "\n" "34" "\n"
    "35" "\n" "36" "\n" "37" "\n" "38" "\n" "39" "\n"
    "40" "\n" "41" "\n" "42" "\n" "43" "\n" "44" "\n"
    "45" "\n" "46" "\n" "47" "\n" "48" "\n" "49" "\n"
    "50" "\n" "51" "\n" "52" "\n" "53" "\n" "54" "\n"
    "55" "\n" "56" "\n" "57" "\n" "58" "\n" "59";

settings_datetime::settings_datetime(Preferences& prefs, lv_obj_t* parent) : prefs(prefs), parent_tile(parent) {
    title = std::unique_ptr<lv_obj_t, lv_obj_deleter>(lv_label_create(parent));
    lv_obj_set_style_text_font(title.get(), &lv_font_play_25, 0);
    lv_label_set_text(title.get(), "Date & Time");
    lv_obj_set_width(title.get(), LV_PCT(90));
    lv_obj_align(title.get(), LV_ALIGN_TOP_MID, 0, 35);

    hour = lv_roller_create(parent);
    lv_roller_set_options(hour, numbers_00_to_23, LV_ROLLER_MODE_INFINITE);
    lv_roller_set_visible_row_count(hour, 3);
    lv_obj_set_style_text_font(hour, &lv_font_play_18, LV_PART_MAIN);
    lv_obj_set_style_text_font(hour, &lv_font_play_25, LV_PART_SELECTED);
    lv_obj_set_style_bg_color(hour, colors::PRIMARY, LV_PART_SELECTED);
    lv_obj_set_style_border_color(hour, colors::PRIMARY, LV_PART_MAIN);
    lv_obj_align_to(hour, title.get(), LV_ALIGN_OUT_BOTTOM_LEFT, 0, 25);
    cyberpunk_decoration_apply(hour);

    minute = lv_roller_create(parent);
    lv_roller_set_options(minute, numbers_00_to_59, LV_ROLLER_MODE_INFINITE);
    lv_roller_set_visible_row_count(minute, 3);
    lv_obj_set_style_text_font(minute, &lv_font_play_18, LV_PART_MAIN);
    lv_obj_set_style_text_font(minute, &lv_font_play_25, LV_PART_SELECTED);
    lv_obj_set_style_bg_color(minute, colors::PRIMARY, LV_PART_SELECTED);
    lv_obj_set_style_border_color(minute, colors::PRIMARY, LV_PART_MAIN);
    lv_obj_align_to(minute, hour, LV_ALIGN_OUT_RIGHT_MID, 12, 0);
    cyberpunk_decoration_apply(minute);

    second = lv_roller_create(parent);
    lv_roller_set_options(second, numbers_00_to_59, LV_ROLLER_MODE_INFINITE);
    lv_roller_set_visible_row_count(second, 3);
    lv_obj_set_style_text_font(second, &lv_font_play_18, LV_PART_MAIN);
    lv_obj_set_style_text_font(second, &lv_font_play_25, LV_PART_SELECTED);
    lv_obj_set_style_bg_color(second, colors::PRIMARY, LV_PART_SELECTED);
    lv_obj_set_style_border_color(second, colors::PRIMARY, LV_PART_MAIN);
    lv_obj_align_to(second, minute, LV_ALIGN_OUT_RIGHT_MID, 12, 0);
    cyberpunk_decoration_apply(second);

    auto datetime = instance.rtc.getDateTime();
    lv_roller_set_selected(hour, datetime.getHour(), LV_ANIM_OFF);
    lv_roller_set_selected(minute, datetime.getMinute(), LV_ANIM_OFF);
    lv_roller_set_selected(second, datetime.getSecond(), LV_ANIM_OFF);

    auto* tileview = lv_obj_get_parent(parent);
    lv_obj_add_event_cb(tileview, [](lv_event_t* event){
        settings_datetime* self = static_cast<settings_datetime*>(lv_event_get_user_data(event));
        auto* tileview = lv_event_get_current_target_obj(event);
        auto* tile = lv_tileview_get_tile_active(tileview);

        if(!self->open && tile == self->parent_tile) {
            self->open = true;

            auto datetime = instance.rtc.getDateTime();
            lv_roller_set_selected(self->hour, datetime.getHour(), LV_ANIM_OFF);
            lv_roller_set_selected(self->minute, datetime.getMinute(), LV_ANIM_OFF);
            lv_roller_set_selected(self->second, datetime.getSecond(), LV_ANIM_OFF);
            self->dirty = false;
        } else if(self->open && tile != self->parent_tile) {
            self->open = false;
            if(self->dirty) {
                auto datetime = instance.rtc.getDateTime();
                int hour = lv_roller_get_selected(self->hour);
                int minute = lv_roller_get_selected(self->minute);
                int second = lv_roller_get_selected(self->second);
                instance.rtc.setDateTime(datetime.getYear(), datetime.getMonth(), datetime.getDay(), hour, minute, second);
                haptic_feedback();
            }
        }
    }, LV_EVENT_VALUE_CHANGED, this);

    auto roller_cb = [](lv_event_t* event){
        settings_datetime* self = static_cast<settings_datetime*>(lv_event_get_user_data(event));
        self->dirty = true;
    };
    lv_obj_add_event_cb(hour, roller_cb, LV_EVENT_VALUE_CHANGED, this);
    lv_obj_add_event_cb(minute, roller_cb, LV_EVENT_VALUE_CHANGED, this);
    lv_obj_add_event_cb(second, roller_cb, LV_EVENT_VALUE_CHANGED, this);
}

}
