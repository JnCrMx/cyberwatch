#include "calculator.hpp"

#include <string_view>
#include <variant>
#include <utility>
#include <LilyGoLib.h>
#include "cyberpunk_ui.hpp"
#include "hardware.hpp"
#include "tinyexpr.h"

LV_FONT_DECLARE(lv_font_play_40);

namespace apps {

calculator::calculator(Preferences& prefs, lv_obj_t* parent) : prefs(prefs) {
    textarea = std::unique_ptr<lv_obj_t, lv_obj_deleter>(lv_textarea_create(parent));
    lv_textarea_set_one_line(textarea.get(), true);
    lv_obj_set_style_text_font(textarea.get(), &lv_font_play_40, LV_PART_MAIN);
    lv_obj_set_style_text_color(textarea.get(), colors::PRIMARY, LV_PART_MAIN);
    lv_obj_set_style_border_color(textarea.get(), colors::PRIMARY, LV_PART_CURSOR | LV_STATE_FOCUSED);
    lv_obj_set_width(textarea.get(), LV_PCT(70));
    lv_obj_align(textarea.get(), LV_ALIGN_TOP_LEFT, 5, 35);
    lv_obj_add_state(textarea.get(), LV_STATE_FOCUSED);
    lv_obj_add_flag(textarea.get(), LV_OBJ_FLAG_FLOATING);
    lv_obj_set_style_border_color(textarea.get(), colors::PRIMARY, LV_PART_MAIN);
    cyberpunk_decoration_apply(textarea.get());

    lv_obj_add_event_cb(textarea.get(), [](lv_event_t* e){
        lv_obj_t * obj = lv_event_get_target_obj(e);
        calculator* self = static_cast<calculator*>(lv_event_get_user_data(e));

        const char* text = lv_textarea_get_text(obj);
        double result = te_interp(text, nullptr);
        lv_textarea_set_text(obj, std::to_string(result).c_str());
        lv_obj_scroll_to_x(obj, 0, LV_ANIM_OFF);
    }, LV_EVENT_READY, this);

    button = std::unique_ptr<lv_obj_t, lv_obj_deleter>(lv_button_create(parent));
    lv_obj_set_style_text_color(button.get(), colors::PRIMARY, LV_PART_MAIN);
    lv_obj_set_style_text_font(button.get(), &lv_font_play_40, LV_PART_MAIN);
    lv_obj_set_width(button.get(), LV_PCT(20));
    lv_obj_align_to(button.get(), textarea.get(), LV_ALIGN_OUT_RIGHT_TOP, 7, 0);
    lv_obj_set_height(button.get(), lv_obj_get_height(textarea.get()));
    lv_obj_add_flag(button.get(), LV_OBJ_FLAG_FLOATING);
    lv_obj_set_style_outline_width(button.get(), 0, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(button.get(), 0, LV_PART_MAIN);
    lv_obj_add_event_cb(button.get(), [](lv_event_t* e){
        haptic_feedback();
        calculator* self = static_cast<calculator*>(lv_event_get_user_data(e));
        lv_obj_send_event(self->textarea.get(), LV_EVENT_READY, NULL);
    }, LV_EVENT_CLICKED, this);
    lv_obj_set_style_border_color(button.get(), colors::PRIMARY, LV_PART_MAIN);
    cyberpunk_decoration_apply(button.get());

    button_label = std::unique_ptr<lv_obj_t, lv_obj_deleter>(lv_label_create(button.get()));
    lv_obj_set_style_text_color(button_label.get(), colors::PRIMARY, LV_PART_MAIN);
    lv_obj_set_style_text_font(button_label.get(), &lv_font_play_40, LV_PART_MAIN);
    lv_label_set_text(button_label.get(), "=");
    lv_obj_center(button_label.get());

    static const char * btnm_map[] = {
        "(", ")", LV_SYMBOL_BACKSPACE, LV_SYMBOL_TRASH, "\n",
        "7", "8", "9", "/", "\n",
        "4", "5", "6", "*", "\n",
        "1", "2", "3", "-", "\n",
        "0", ".", "=", "+", ""
    };

    keypad = std::unique_ptr<lv_obj_t, lv_obj_deleter>(lv_buttonmatrix_create(parent));
    lv_obj_remove_flag(keypad.get(), LV_OBJ_FLAG_CLICK_FOCUSABLE);
    lv_buttonmatrix_set_map(keypad.get(), btnm_map);
    lv_obj_set_style_border_width(keypad.get(), 0, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(keypad.get(), LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_bg_color(keypad.get(), colors::BACKGROUND, LV_PART_MAIN);
    lv_obj_set_style_text_color(keypad.get(), colors::PRIMARY, LV_PART_ITEMS);
    lv_obj_set_style_border_color(keypad.get(), colors::PRIMARY, LV_PART_ITEMS);
    lv_obj_set_style_border_width(keypad.get(), 2, LV_PART_ITEMS);
    lv_obj_set_style_radius(keypad.get(), 0, LV_PART_ITEMS);
    lv_obj_set_style_bg_color(keypad.get(), colors::BACKGROUND, LV_PART_ITEMS);
    lv_obj_set_size(keypad.get(), LV_PCT(100), LV_PCT(100));
    lv_obj_align_to(keypad.get(), textarea.get(), LV_ALIGN_OUT_BOTTOM_LEFT, -9, 10);
    lv_obj_remove_flag(keypad.get(), LV_OBJ_FLAG_PRESS_LOCK);
    lv_buttonmatrix_set_button_ctrl_all(keypad.get(), LV_BUTTONMATRIX_CTRL_CLICK_TRIG);

    lv_obj_add_event_cb(keypad.get(), [](lv_event_t* e){
        haptic_feedback();
        lv_obj_t * obj = lv_event_get_target_obj(e);
        calculator* self = static_cast<calculator*>(lv_event_get_user_data(e));

        const char* txt = lv_buttonmatrix_get_button_text(obj, lv_buttonmatrix_get_selected_button(obj));
        if(lv_strcmp(txt, LV_SYMBOL_BACKSPACE) == 0) {
            lv_textarea_delete_char(self->textarea.get());
        } else if(lv_strcmp(txt, LV_SYMBOL_TRASH) == 0) {
            lv_textarea_set_text(self->textarea.get(), "");
        } else if(lv_strcmp(txt, "=") == 0) {
            lv_obj_send_event(self->textarea.get(), LV_EVENT_READY, NULL);
        } else {
            lv_textarea_add_text(self->textarea.get(), txt);
        }
    }, LV_EVENT_VALUE_CHANGED, this);
}

}
