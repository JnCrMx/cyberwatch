#include <LilyGoLib.h>
#include <LV_Helper.h>

LV_FONT_DECLARE(lv_font_play_40);
LV_FONT_DECLARE(lv_font_play_70);

lv_obj_t* ui_main;

lv_obj_t* ui_time_label;
lv_obj_t* ui_battery_label;
lv_obj_t* ui_battery_bar;

template<typename Callback>
lv_event_dsc_t* lv_obj_add_event_handler(lv_obj_t* obj, lv_event_code_t filter, Callback&& event_cb)
{
    lv_event_cb_t cb = [](lv_event_t* e) {
        Callback* callback = static_cast<Callback*>(e->user_data);
        (*callback)(e);
    };
    return lv_obj_add_event_cb(obj, cb, filter, new Callback(std::forward<Callback>(event_cb)));
}

struct ClockState {
    bool expanded = false;
};

void enter_sleep_mode() {
    instance.lightSleep();

    // after waking up, force a refresh and activity trigger (so we won't sleep again immediately)
    lv_refr_now(nullptr);
    lv_disp_trig_activity(nullptr);
}

void setup()
{
    Serial.begin(115200);
    instance.begin();
    beginLvglHelper(instance);

    static lv_style_t cyberpunk_style;
    lv_style_init(&cyberpunk_style);
    lv_style_set_bg_color(&cyberpunk_style, lv_color_hex(0x0b0c16));
    lv_style_set_text_color(&cyberpunk_style, lv_color_hex(0xffffff));

    lv_theme_t * th_act = lv_display_get_theme(nullptr);
    static lv_theme_t cyberpunk_theme = *th_act;
    static auto theme_callback = [](lv_theme_t* th, lv_obj_t* obj){
        lv_obj_add_style(obj, &cyberpunk_style, 0);
    };
    lv_theme_set_parent(&cyberpunk_theme, th_act);
    lv_theme_set_apply_cb(&cyberpunk_theme, theme_callback);

    lv_disp_set_theme(lv_display_get_default(), &cyberpunk_theme);

    lv_obj_add_style(lv_screen_active(), &cyberpunk_style, 0);
    ui_main = lv_tileview_create(lv_screen_active());

    lv_obj_t* clock_tile = lv_tileview_add_tile(ui_main, 0, 0, LV_DIR_VER);

    ui_time_label = lv_label_create(clock_tile);
    lv_label_set_text(ui_time_label, "00:00");
    lv_obj_set_style_text_font(ui_time_label, &lv_font_play_70, 0);
    lv_obj_set_style_text_color(ui_time_label, lv_color_hex(0xe4381c), 0);
    lv_obj_set_user_data(ui_time_label, new ClockState());
    lv_obj_add_event_cb(ui_time_label, [](lv_event_t* e){
        lv_obj_t* obj = static_cast<lv_obj_t*>(e->current_target);
        ClockState* state = static_cast<ClockState*>(lv_obj_get_user_data(obj));
        if(!state) {
            return;
        }
        auto datetime = instance.rtc.getDateTime();
        if (state->expanded) {
            state->expanded = false;
            lv_obj_set_style_text_font(obj, &lv_font_play_70, 0);
            lv_label_set_text_fmt(obj, "%02d:%02d", datetime.getHour(), datetime.getMinute());
        } else {
            state->expanded = true;
            lv_obj_set_style_text_font(obj, &lv_font_play_40, 0);
            lv_label_set_text_fmt(obj, "%02d:%02d:%02d", datetime.getHour(), datetime.getMinute(), datetime.getSecond());
        }
    }, LV_EVENT_CLICKED, nullptr);
    lv_obj_add_flag(ui_time_label, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_center(ui_time_label);

    ui_battery_label = lv_label_create(clock_tile);
    lv_label_set_text(ui_battery_label, "Battery: ??%");
    lv_obj_align_to(ui_battery_label, ui_time_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    ui_battery_bar = lv_bar_create(clock_tile);
    lv_obj_set_size(ui_battery_bar, 100, 20);
    lv_obj_align_to(ui_battery_bar, ui_battery_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
    lv_obj_set_style_bg_opa(ui_battery_bar, LV_OPA_COVER, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(ui_battery_bar, lv_color_hex(0xFF0000), LV_PART_INDICATOR);
    lv_obj_set_style_bg_grad_color(ui_battery_bar, lv_color_hex(0x00FF00), LV_PART_INDICATOR);
    lv_obj_set_style_bg_grad_dir(ui_battery_bar, LV_GRAD_DIR_HOR, LV_PART_INDICATOR);
    lv_bar_set_range(ui_battery_bar, 0, 100);
    lv_bar_set_value(ui_battery_bar, 100, LV_ANIM_OFF);

    lv_timer_create([](lv_timer_t* timer){
        ClockState* state = static_cast<ClockState*>(lv_obj_get_user_data(ui_time_label));
        if(state) {
            auto datetime = instance.rtc.getDateTime();
            if(state->expanded) {
                lv_label_set_text_fmt(ui_time_label, "%02d:%02d:%02d", datetime.getHour(), datetime.getMinute(), datetime.getSecond());
            } else {
                lv_label_set_text_fmt(ui_time_label, "%02d:%02d", datetime.getHour(), datetime.getMinute());
            }
        }

        int battery_percent = instance.pmu.getBatteryPercent();
        bool is_charging = instance.pmu.isCharging();
        lv_label_set_text_fmt(ui_battery_label, "Battery: %d%%", battery_percent);
        if (is_charging) {
            lv_obj_set_style_text_color(ui_battery_label, lv_color_hex(0x00FF00), 0);
        } else {
            lv_obj_set_style_text_color(ui_battery_label, lv_color_hex(0xFFFFFF), 0);
        }
        lv_bar_set_value(ui_battery_bar, battery_percent, LV_ANIM_OFF);
    }, 1000, nullptr);
    lv_timer_create([](lv_timer_t* timer){
        uint32_t inactive_time = lv_display_get_inactive_time(nullptr);
        if(inactive_time > 30000) {
            enter_sleep_mode();
        } else if(inactive_time > 10000) {
            instance.setBrightness(1);
        } else {
            instance.setBrightness(DEVICE_MAX_BRIGHTNESS_LEVEL);
        }
    }, 1000, nullptr);

    instance.setBrightness(DEVICE_MAX_BRIGHTNESS_LEVEL);
    instance.enableCharge(1000);
    instance.onEvent([](DeviceEvent_t event, void* user_data) {
        if (event == PMU_EVENT_KEY_CLICKED) {
            enter_sleep_mode();
        }
    }, nullptr);
}

void loop()
{
    instance.loop();
    lv_timer_handler();
    delay(2);
}
