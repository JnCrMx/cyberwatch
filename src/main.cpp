#include <LilyGoLib.h>
#include <LV_Helper.h>

lv_obj_t* ui_time_label;
lv_obj_t* ui_battery_label;
lv_obj_t* ui_battery_bar;

void setup()
{
    Serial.begin(115200);
    instance.begin();
    beginLvglHelper(instance);

    ui_time_label = lv_label_create(lv_screen_active());
    lv_label_set_text(ui_time_label, "Time: 00:00:00");
    lv_obj_center(ui_time_label);

    ui_battery_label = lv_label_create(lv_screen_active());
    lv_label_set_text(ui_battery_label, "Battery: ??%");
    lv_obj_align_to(ui_battery_label, ui_time_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    ui_battery_bar = lv_bar_create(lv_screen_active());
    lv_obj_set_size(ui_battery_bar, 100, 20);
    lv_obj_align_to(ui_battery_bar, ui_battery_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
    lv_obj_set_style_bg_opa(ui_battery_bar, LV_OPA_COVER, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(ui_battery_bar, lv_color_hex(0xFF0000), LV_PART_INDICATOR);
    lv_obj_set_style_bg_grad_color(ui_battery_bar, lv_color_hex(0x00FF00), LV_PART_INDICATOR);
    lv_obj_set_style_bg_grad_dir(ui_battery_bar, LV_GRAD_DIR_HOR, LV_PART_INDICATOR);
    lv_bar_set_range(ui_battery_bar, 0, 100);
    lv_bar_set_value(ui_battery_bar, 100, LV_ANIM_OFF);

    lv_timer_create([](lv_timer_t* timer){
        auto datetime = instance.rtc.getDateTime();
        lv_label_set_text_fmt(ui_time_label, "Time: %02d:%02d:%02d", datetime.getHour(), datetime.getMinute(), datetime.getSecond());

        int battery_percent = instance.pmu.getBatteryPercent();
        lv_label_set_text_fmt(ui_battery_label, "Battery: %d%%", battery_percent);
        lv_bar_set_value(ui_battery_bar, battery_percent, LV_ANIM_OFF);
    }, 1000, NULL);

    instance.setBrightness(DEVICE_MAX_BRIGHTNESS_LEVEL);
}

void loop()
{
    lv_timer_handler();
    delay(2);
}
