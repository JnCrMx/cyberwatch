#include <LilyGoLib.h>
#include <LV_Helper.h>

#include "cyberpunk_ui.hpp"
#include "ui_battery_indicator.hpp"
#include "ui_clock.hpp"

static lv_obj_t* ui_main;

static std::unique_ptr<class ui_clock> ui_clock;
static std::unique_ptr<class ui_battery_indicator> ui_battery_indicator;

static bool is_dimmed = false;

template<typename Callback>
lv_event_dsc_t* lv_obj_add_event_handler(lv_obj_t* obj, lv_event_code_t filter, Callback&& event_cb)
{
    lv_event_cb_t cb = [](lv_event_t* e) {
        Callback* callback = static_cast<Callback*>(e->user_data);
        (*callback)(e);
    };
    return lv_obj_add_event_cb(obj, cb, filter, new Callback(std::forward<Callback>(event_cb)));
}

void enter_sleep_mode() {
    instance.lightSleep();

    // after waking up, force a refresh and activity trigger (so we won't sleep again immediately)
    lv_refr_now(nullptr);
    lv_display_trigger_activity(nullptr);

    instance.setBrightness(DEVICE_MAX_BRIGHTNESS_LEVEL);
    is_dimmed = false;

    ui_clock->update();
    ui_battery_indicator->update();
}

void setup()
{
    Serial.begin(115200);
    instance.begin();
    beginLvglHelper(instance);

    init_cyberpunk_theme();
    lv_disp_set_theme(lv_display_get_default(), &cyberpunk_theme);
    lv_obj_add_style(lv_screen_active(), &cyberpunk_style, 0);

    ui_main = lv_tileview_create(lv_screen_active());
    lv_obj_t* clock_tile = lv_tileview_add_tile(ui_main, 0, 0, LV_DIR_VER);

    ui_clock = std::make_unique<class ui_clock>(clock_tile);
    lv_obj_center(ui_clock->obj());

    ui_battery_indicator = std::make_unique<class ui_battery_indicator>(lv_screen_active());
    lv_obj_align(ui_battery_indicator->obj(), LV_ALIGN_TOP_RIGHT, -4, 4);
    lv_obj_add_flag(ui_battery_indicator->obj(), LV_OBJ_FLAG_FLOATING);

    lv_timer_create([](lv_timer_t* timer){
        ui_clock->update();
        ui_battery_indicator->update();
    }, 1000, nullptr);

    lv_timer_create([](lv_timer_t* timer){
        uint32_t inactive_time = lv_display_get_inactive_time(nullptr);
        if(inactive_time > 30000) {
            enter_sleep_mode();
        } else if(inactive_time > 10000) {
            instance.setBrightness(1);
            is_dimmed = true;
        } else if(is_dimmed) {
            instance.setBrightness(DEVICE_MAX_BRIGHTNESS_LEVEL);
            is_dimmed = false;
        }
    }, 100, nullptr);

    instance.setBrightness(DEVICE_MAX_BRIGHTNESS_LEVEL);
    instance.enableCharge(1000);
    instance.onEvent([](DeviceEvent_t event, void* user_data) {
        if (event == PMU_EVENT_KEY_CLICKED) {
            if(is_dimmed) {
                lv_display_trigger_activity(nullptr); // trigger activity to prevent immediate dimming
                instance.setBrightness(DEVICE_MAX_BRIGHTNESS_LEVEL);
                is_dimmed = false;
            } else {
                enter_sleep_mode();
            }
        }
    }, nullptr);
}

void loop()
{
    instance.loop();
    lv_timer_handler();
    delay(2);
}
