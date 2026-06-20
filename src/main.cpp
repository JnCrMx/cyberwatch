#include <LilyGoLib.h>
#include <LV_Helper.h>
#include <Preferences.h>

#include "cyberpunk_ui.hpp"
#include "hardware.hpp"
#include "prefs.hpp"

#include "apps/clock.hpp"
#include "apps/calculator.hpp"
#include "apps/flashlight.hpp"
#include "apps/info_battery.hpp"
#include "apps/info_sensors.hpp"
#include "apps/settings_general.hpp"
#include "apps/settings_datetime.hpp"

#include "indicators/battery.hpp"

LV_IMAGE_DECLARE(lv_img_startup_0);
LV_IMAGE_DECLARE(lv_img_startup_1);
LV_IMAGE_DECLARE(lv_img_startup_2);
LV_IMAGE_DECLARE(lv_img_startup_3);

static Preferences prefs;

static lv_obj_t* ui_main;
static std::unique_ptr<apps::clock> app_clock;
static std::unique_ptr<apps::calculator> app_calculator;
static std::unique_ptr<apps::flashlight> app_flashlight;
static std::unique_ptr<apps::info_battery> app_info_battery;
static std::unique_ptr<apps::info_sensors> app_info_sensors;
static std::unique_ptr<apps::settings_general> app_settings_general;
static std::unique_ptr<apps::settings_datetime> app_settings_datetime;
static std::unique_ptr<indicators::battery> indicator_battery;

static bool sleep_mode_blocked = false;
static bool dim_blocked = false;
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
    if(sleep_mode_blocked) {
        return;
    }

    instance.lightSleep(static_cast<WakeupSource_t>(WAKEUP_SRC_POWER_KEY | WAKEUP_SRC_TOUCH_PANEL | WAKEUP_SRC_SENSOR));

    // after waking up, force a refresh and activity trigger (so we won't sleep again immediately)
    lv_refr_now(nullptr);
    lv_display_trigger_activity(nullptr);

    instance.setBrightness(DEVICE_MAX_BRIGHTNESS_LEVEL);
    is_dimmed = false;

    app_clock->update();
    indicator_battery->update();
}
void haptic_feedback() {
    if(!prefs.getBool(preferences::system::HAPTIC_ENABLED, true)) {
        return;
    }
    instance.vibrator();
}

void block_sleep_mode() {
    sleep_mode_blocked = true;
}
void unblock_sleep_mode() {
    sleep_mode_blocked = false;
}
void block_dim() {
    dim_blocked = true;
    if(is_dimmed) {
        instance.setBrightness(DEVICE_MAX_BRIGHTNESS_LEVEL);
        is_dimmed = false;
    }
}
void unblock_dim() {
    dim_blocked = false;
}

void lock_tiles() {
    lv_obj_remove_flag(ui_main, LV_OBJ_FLAG_SCROLLABLE);
}
void unlock_tiles() {
    lv_obj_add_flag(ui_main, LV_OBJ_FLAG_SCROLLABLE);
}

void setup() {
    Serial.begin(115200);
    instance.begin();
    beginLvglHelper(instance);
    prefs.begin("CyberWatch", false);

    init_cyberpunk_theme();
    lv_display_set_theme(lv_display_get_default(), &cyberpunk_theme);
    lv_obj_add_style(lv_screen_active(), &cyberpunk_style, 0);

    ui_main = lv_tileview_create(lv_screen_active());
    lv_obj_set_style_bg_opa(ui_main, LV_OPA_TRANSP, LV_PART_MAIN);

    // Clocks
    lv_obj_t* clock_tile = lv_tileview_add_tile(ui_main, 2, 0, LV_DIR_ALL);
    app_clock = std::make_unique<apps::clock>(prefs, clock_tile);

    // Tools
    lv_obj_t* flashlight_tile = lv_tileview_add_tile(ui_main, 3, 0, LV_DIR_ALL);
    app_flashlight = std::make_unique<apps::flashlight>(prefs, flashlight_tile);

    lv_obj_t* calculator_tile = lv_tileview_add_tile(ui_main, 3, 1, LV_DIR_ALL);
    app_calculator = std::make_unique<apps::calculator>(prefs, calculator_tile);

    // Settings
    lv_obj_t* settings_general_tile = lv_tileview_add_tile(ui_main, 1, 0, LV_DIR_ALL);
    app_settings_general = std::make_unique<apps::settings_general>(prefs, settings_general_tile);

    lv_obj_t* settings_datetime_tile = lv_tileview_add_tile(ui_main, 1, 1, LV_DIR_ALL);
    app_settings_datetime = std::make_unique<apps::settings_datetime>(prefs, settings_datetime_tile);

    // System Info
    lv_obj_t* info_battery_tile = lv_tileview_add_tile(ui_main, 0, 0, LV_DIR_ALL);
    app_info_battery = std::make_unique<apps::info_battery>(prefs, info_battery_tile);

    lv_obj_t* info_sensors_tile = lv_tileview_add_tile(ui_main, 0, 1, LV_DIR_ALL);
    app_info_sensors = std::make_unique<apps::info_sensors>(prefs, info_sensors_tile);

    lv_tileview_set_tile(ui_main, clock_tile, LV_ANIM_OFF);

    indicator_battery = std::make_unique<indicators::battery>(prefs, lv_screen_active());
    lv_obj_align(indicator_battery->obj(), LV_ALIGN_TOP_RIGHT, -4, 4);
    lv_obj_add_flag(indicator_battery->obj(), LV_OBJ_FLAG_FLOATING);
    lv_obj_move_background(indicator_battery->obj());

    lv_timer_create([](lv_timer_t* timer){
        app_clock->update();
        app_info_battery->update();
        indicator_battery->update();
    }, 1000, nullptr);

    lv_timer_create([](lv_timer_t* timer){
        uint32_t inactive_time = lv_display_get_inactive_time(nullptr);
        if(inactive_time > 30000 && prefs.getBool(preferences::system::AUTO_SLEEP, true)) {
            enter_sleep_mode();
        } else if(inactive_time > 10000 && prefs.getBool(preferences::system::AUTO_DIM, true) && !dim_blocked) {
            instance.setBrightness(1);
            is_dimmed = true;
        } else if(is_dimmed) {
            instance.setBrightness(DEVICE_MAX_BRIGHTNESS_LEVEL);
            is_dimmed = false;
        }
    }, 100, nullptr);

    instance.setBrightness(DEVICE_MAX_BRIGHTNESS_LEVEL);
    instance.enableCharge(1000);

    instance.sensor.configAccelerometer();
    instance.sensor.enableAccelerometer();
    instance.sensor.enablePedometer();
    // IRQs
    instance.sensor.disableActivityIRQ();
    instance.sensor.disableAnyNoMotionIRQ();
    instance.sensor.disablePedometerIRQ();

    bool wakeup_double_tap = prefs.getBool(preferences::system::WAKEUP_DOUBLE_TAP, false);
    bool wakeup_tilt = prefs.getBool(preferences::system::WAKEUP_TILT, false);
    instance.sensor.enableFeature(SensorBMA423::FEATURE_WAKEUP, wakeup_double_tap);
    instance.sensor.configFeatureInterrupt(SensorBMA423::FEATURE_WAKEUP, wakeup_double_tap);
    instance.sensor.enableFeature(SensorBMA423::FEATURE_TILT, wakeup_tilt);
    instance.sensor.configFeatureInterrupt(SensorBMA423::FEATURE_TILT, wakeup_tilt);
    if(wakeup_double_tap || wakeup_tilt) {
        instance.sensor.configInterrupt(0, 0); // needs to be logic level low, so it works with light sleep
        gpio_set_intr_type(static_cast<gpio_num_t>(digitalPinToGPIONumber(SENSOR_INT)), static_cast<gpio_int_type_t>(FALLING & 0x7));
    }

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

    if(prefs.getBool(preferences::system::BOOT_ANIMATION, true)) {
        static const lv_image_dsc_t* startup_images[4] = {
            &lv_img_startup_0,
            &lv_img_startup_1,
            &lv_img_startup_2,
            &lv_img_startup_3
        };
        lv_obj_t* startup_animation = lv_animimg_create(lv_screen_active());
        lv_animimg_set_src(startup_animation, reinterpret_cast<const void**>(startup_images), 4);
        lv_animimg_set_duration(startup_animation, 2500);
        lv_animimg_set_repeat_count(startup_animation, LV_ANIM_REPEAT_INFINITE);
        lv_obj_center(startup_animation);
        lv_obj_move_foreground(startup_animation);
        lv_obj_add_flag(startup_animation, LV_OBJ_FLAG_FLOATING);
        lv_animimg_start(startup_animation);
        lv_obj_delete_delayed(startup_animation, 2500);
    }
}

void loop()
{
    instance.loop();
    lv_timer_handler();
    delay(2);
}
