#pragma once

#include "lvgl.hpp"
#include "cyberpunk_ui.hpp"
#include <memory>
#include <LilyGoLib.h>

LV_FONT_DECLARE(lv_font_play_40);
LV_FONT_DECLARE(lv_font_play_70);

class ui_clock {
    public:
        ui_clock(lv_obj_t* parent) {
            label = std::unique_ptr<lv_obj_t, lv_obj_deleter>(lv_label_create(parent));
            lv_label_set_text(label.get(), "00:00");
            lv_obj_set_style_text_font(label.get(), &lv_font_play_70, 0);
            lv_obj_set_style_text_color(label.get(), lv_color_hex(0xe4381c), 0);
            lv_obj_set_style_border_color(label.get(), lv_color_hex(0xe4381c), 0);
            lv_obj_set_style_pad_all(label.get(), 15, 0);
            lv_obj_set_user_data(label.get(), this);
            lv_obj_add_event_cb(label.get(), [](lv_event_t* e){
                lv_obj_t* obj = static_cast<lv_obj_t*>(e->current_target);
                ui_clock* state = static_cast<ui_clock*>(lv_obj_get_user_data(obj));
                if(!state) {
                    return;
                }
                state->expanded = !state->expanded;
                lv_obj_set_style_text_font(obj, state->expanded ? &lv_font_play_40 : &lv_font_play_70, 0);
                state->update();
            }, LV_EVENT_CLICKED, nullptr);
            lv_obj_add_flag(label.get(), LV_OBJ_FLAG_CLICKABLE);
            lv_obj_add_event_cb(label.get(), cyberpunk_decoration_cb, LV_EVENT_DRAW_TASK_ADDED, nullptr);
            lv_obj_add_flag(label.get(), LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS);
        }
        void update() {
            auto datetime = instance.rtc.getDateTime();
            if(expanded) {
                lv_label_set_text_fmt(label.get(), "%02d:%02d:%02d", datetime.getHour(), datetime.getMinute(), datetime.getSecond());
            } else {
                lv_label_set_text_fmt(label.get(), "%02d:%02d", datetime.getHour(), datetime.getMinute());
            }
        }
        lv_obj_t* obj() {
            return label.get();
        }
    private:
        std::unique_ptr<lv_obj_t, lv_obj_deleter> label;
        bool expanded = false;
};
