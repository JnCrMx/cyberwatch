#include "info_sensors.hpp"

#include <LilyGoLib.h>
#include "cyberpunk_ui.hpp"

LV_FONT_DECLARE(lv_font_play_25);

namespace apps {

info_sensors::info_sensors(Preferences& prefs, lv_obj_t* parent) : prefs(prefs) {
    title = std::unique_ptr<lv_obj_t, lv_obj_deleter>(lv_label_create(parent));
    lv_obj_set_style_text_font(title.get(), &lv_font_play_25, 0);
    lv_label_set_text(title.get(), "Battery Info");
    lv_obj_set_width(title.get(), LV_PCT(90));
    lv_obj_align(title.get(), LV_ALIGN_TOP_MID, 0, 35);

    table = std::unique_ptr<lv_obj_t, lv_obj_deleter>(lv_table_create(parent));
    lv_table_set_cell_value(table.get(), 0, 0, "Steps");
    lv_table_set_cell_value(table.get(), 0, 1, "Temperature");
    update();

    lv_obj_set_size(table.get(), LV_PCT(90), lv_obj_get_height(parent) - 35 - lv_obj_get_height(title.get()) - 10 - 35);
    lv_obj_align_to(table.get(), title.get(), LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
    lv_obj_set_style_bg_opa(table.get(), LV_OPA_TRANSP, LV_PART_ITEMS);
    lv_obj_set_style_text_color(table.get(), colors::PRIMARY, LV_PART_ITEMS);
    lv_obj_set_style_border_opa(table.get(), LV_OPA_TRANSP, LV_PART_ITEMS);
    lv_obj_set_style_border_color(table.get(), colors::PRIMARY, LV_PART_MAIN);
    cyberpunk_decoration_apply(table.get());
}

void info_sensors::update() {
    lv_table_set_cell_value(table.get(), 0, 1, std::to_string(instance.sensor.getPedometerCounter()).c_str());
    lv_table_set_cell_value(table.get(), 1, 1, (std::to_string(instance.sensor.getTemperature(SensorBMA423::TemperatureUnit::TEMP_DEG)) + " °C").c_str());
}

}
