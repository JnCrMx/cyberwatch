#include "clock.hpp"

#include <LilyGoLib.h>
#include "cyberpunk_ui.hpp"
#include "hardware.hpp"
#include "prefs.hpp"

LV_FONT_DECLARE(lv_font_play_40);
LV_FONT_DECLARE(lv_font_play_70);

namespace apps {

clock::clock(Preferences& prefs, lv_obj_t* parent) : prefs(prefs) {
    label = std::unique_ptr<lv_obj_t, lv_obj_deleter>(lv_label_create(parent));
    lv_label_set_text(label.get(), "00:00");
    lv_obj_set_style_text_font(label.get(), &lv_font_play_70, 0);
    lv_obj_set_style_text_color(label.get(), colors::PRIMARY, 0);
    lv_obj_set_style_border_color(label.get(), colors::PRIMARY, 0);
    lv_obj_set_style_pad_all(label.get(), 15, 0);
    lv_obj_set_user_data(label.get(), this);
    lv_obj_add_event_cb(label.get(), [](lv_event_t* e){
        haptic_feedback();
        lv_obj_t* obj = static_cast<lv_obj_t*>(e->current_target);
        clock* state = static_cast<clock*>(lv_obj_get_user_data(obj));
        if(!state) {
            return;
        }
        state->expanded = !state->expanded;
        lv_obj_set_style_text_font(obj, state->expanded ? &lv_font_play_40 : &lv_font_play_70, 0);
        state->update();
        state->prefs.putBool(preferences::apps::clock::EXPANDED, state->expanded);
    }, LV_EVENT_CLICKED, nullptr);
    lv_obj_add_flag(label.get(), LV_OBJ_FLAG_CLICKABLE);
    cyberpunk_decoration_apply(label.get());
    lv_obj_center(label.get());

    expanded = prefs.getBool(preferences::apps::clock::EXPANDED, false);
    lv_obj_set_style_text_font(label.get(), expanded ? &lv_font_play_40 : &lv_font_play_70, 0);
    update();
}

void clock::update() {
    auto datetime = instance.rtc.getDateTime();
    if(expanded) {
        lv_label_set_text_fmt(label.get(), "%02d:%02d:%02d", datetime.getHour(), datetime.getMinute(), datetime.getSecond());
    } else {
        lv_label_set_text_fmt(label.get(), "%02d:%02d", datetime.getHour(), datetime.getMinute());
    }
}

}
