#include "settings_general.hpp"

#include <LilyGoLib.h>
#include "cyberpunk_ui.hpp"
#include "hardware.hpp"
#include "prefs.hpp"

LV_FONT_DECLARE(lv_font_play_18);
LV_FONT_DECLARE(lv_font_play_25);

namespace apps {

static lv_style_t switch_main_style{};
static lv_style_t switch_indicator_style{};
static lv_style_t switch_knob_style{};

lv_obj_t* create_switch(lv_obj_t* parent, const char* label_text, lv_event_cb_t event_cb, void* user_data, bool initial_state = false) {
    lv_obj_t* switch_container = lv_obj_create(parent);
    lv_obj_remove_style_all(switch_container);
    lv_obj_set_size(switch_container, LV_PCT(100), 30);
    lv_obj_set_style_bg_opa(switch_container, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_bg_color(switch_container, colors::BACKGROUND, LV_PART_MAIN);
    lv_obj_set_style_border_color(switch_container, colors::PRIMARY, LV_PART_MAIN);
    cyberpunk_decoration_apply(switch_container);

    lv_obj_t* switch_label = lv_label_create(switch_container);
    lv_label_set_text(switch_label, label_text);
    lv_obj_set_style_text_font(switch_label, &lv_font_play_18, 0);
    lv_obj_align(switch_label, LV_ALIGN_LEFT_MID, 5, 0);

    lv_obj_t* switch_obj = lv_switch_create(switch_container);
    lv_obj_align(switch_obj, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_add_style(switch_obj, &switch_main_style, LV_PART_MAIN);
    lv_obj_add_style(switch_obj, &switch_indicator_style, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_add_style(switch_obj, &switch_indicator_style, LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_add_style(switch_obj, &switch_knob_style, LV_PART_KNOB);
    lv_obj_add_event_cb(switch_obj, event_cb, LV_EVENT_VALUE_CHANGED, user_data);
    cyberpunk_decoration_apply(switch_obj);
    lv_obj_set_state(switch_obj, LV_STATE_CHECKED, initial_state);

    return switch_container;
}

settings_general::settings_general(Preferences& prefs, lv_obj_t* parent) : prefs(prefs) {
    title = std::unique_ptr<lv_obj_t, lv_obj_deleter>(lv_label_create(parent));
    lv_obj_set_style_text_font(title.get(), &lv_font_play_25, 0);
    lv_label_set_text(title.get(), "General");
    lv_obj_set_width(title.get(), LV_PCT(90));
    lv_obj_align(title.get(), LV_ALIGN_TOP_MID, 0, 35);

    lv_style_init(&switch_main_style);
    lv_style_set_pad_all(&switch_main_style, 10);
    lv_style_set_border_color(&switch_main_style, colors::PRIMARY);

    lv_style_init(&switch_indicator_style);
    lv_style_set_bg_opa(&switch_indicator_style, LV_OPA_TRANSP);

    lv_style_init(&switch_knob_style);
    lv_style_set_radius(&switch_knob_style, 0);
    lv_style_set_bg_color(&switch_knob_style, colors::PRIMARY);
    lv_style_set_pad_all(&switch_knob_style, -3);

    lv_obj_t* scroll_view = lv_obj_create(parent);
    lv_obj_set_size(scroll_view, LV_PCT(85), lv_obj_get_height(parent) - 35 - lv_obj_get_height(title.get()) - 10 - 35);
    lv_obj_set_style_bg_opa(scroll_view, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_opa(scroll_view, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_pad_all(scroll_view, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_right(scroll_view, 5, LV_PART_MAIN);
    lv_obj_align_to(scroll_view, title.get(), LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);
    lv_obj_set_scroll_dir(scroll_view, LV_DIR_VER);

    parent = scroll_view;
    lv_obj_t* last_obj = nullptr;

    lv_obj_t* boot_animation = create_switch(parent, "Boot Animation", [](lv_event_t* e) {
        settings_general* self = static_cast<settings_general*>(lv_event_get_user_data(e));
        lv_obj_t* obj = lv_event_get_target_obj(e);
        self->prefs.putBool(preferences::system::BOOT_ANIMATION, lv_obj_has_state(obj, LV_STATE_CHECKED));
        haptic_feedback();
    }, this, prefs.getBool(preferences::system::BOOT_ANIMATION, true));
    lv_obj_align(boot_animation, LV_ALIGN_TOP_LEFT, 0, 0);
    last_obj = boot_animation;

    lv_obj_t* auto_sleep = create_switch(parent, "Auto Sleep", [](lv_event_t* e) {
        settings_general* self = static_cast<settings_general*>(lv_event_get_user_data(e));
        lv_obj_t* obj = lv_event_get_target_obj(e);
        self->prefs.putBool(preferences::system::AUTO_SLEEP, lv_obj_has_state(obj, LV_STATE_CHECKED));
        haptic_feedback();
    }, this, prefs.getBool(preferences::system::AUTO_SLEEP, true));
    lv_obj_align_to(auto_sleep, last_obj, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5);
    last_obj = auto_sleep;

    lv_obj_t* auto_dim = create_switch(parent, "Auto Dim", [](lv_event_t* e) {
        settings_general* self = static_cast<settings_general*>(lv_event_get_user_data(e));
        lv_obj_t* obj = lv_event_get_target_obj(e);
        self->prefs.putBool(preferences::system::AUTO_DIM, lv_obj_has_state(obj, LV_STATE_CHECKED));
        haptic_feedback();
    }, this, prefs.getBool(preferences::system::AUTO_DIM, true));
    lv_obj_align_to(auto_dim, last_obj, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5);
    last_obj = auto_dim;

    lv_obj_t* haptic_enabled = create_switch(parent, "Haptic Feedback", [](lv_event_t* e) {
        settings_general* self = static_cast<settings_general*>(lv_event_get_user_data(e));
        lv_obj_t* obj = lv_event_get_target_obj(e);
        self->prefs.putBool(preferences::system::HAPTIC_ENABLED, lv_obj_has_state(obj, LV_STATE_CHECKED));
        haptic_feedback();
    }, this, prefs.getBool(preferences::system::HAPTIC_ENABLED, true));
    lv_obj_align_to(haptic_enabled, last_obj, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5);
    last_obj = haptic_enabled;

    lv_obj_t* wakeup_double_tap = create_switch(parent, "2Tap Wake", [](lv_event_t* e) {
        settings_general* self = static_cast<settings_general*>(lv_event_get_user_data(e));
        lv_obj_t* obj = lv_event_get_target_obj(e);
        self->prefs.putBool(preferences::system::WAKEUP_DOUBLE_TAP, lv_obj_has_state(obj, LV_STATE_CHECKED));
        haptic_feedback();
    }, this, prefs.getBool(preferences::system::WAKEUP_DOUBLE_TAP, false));
    lv_obj_align_to(wakeup_double_tap, last_obj, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5);
    last_obj = wakeup_double_tap;

    lv_obj_t* wakeup_tilt = create_switch(parent, "Tilt Wake", [](lv_event_t* e) {
        settings_general* self = static_cast<settings_general*>(lv_event_get_user_data(e));
        lv_obj_t* obj = lv_event_get_target_obj(e);
        self->prefs.putBool(preferences::system::WAKEUP_TILT, lv_obj_has_state(obj, LV_STATE_CHECKED));
        haptic_feedback();
    }, this, prefs.getBool(preferences::system::WAKEUP_TILT, false));
    lv_obj_align_to(wakeup_tilt, last_obj, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5);
    last_obj = wakeup_tilt;
}

}
