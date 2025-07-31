#include "info_battery.hpp"

#include <LilyGoLib.h>
#include "cyberpunk_ui.hpp"

LV_FONT_DECLARE(lv_font_play_25);

namespace apps {

info_battery::info_battery(Preferences& prefs, lv_obj_t* parent) : prefs(prefs) {
    title = std::unique_ptr<lv_obj_t, lv_obj_deleter>(lv_label_create(parent));
    lv_obj_set_style_text_font(title.get(), &lv_font_play_25, 0);
    lv_label_set_text(title.get(), "Battery Info");
    lv_obj_set_width(title.get(), LV_PCT(90));
    lv_obj_align(title.get(), LV_ALIGN_TOP_MID, 0, 35);

    table = std::unique_ptr<lv_obj_t, lv_obj_deleter>(lv_table_create(parent));
    lv_table_set_cell_value(table.get(), 0, 0, "Battery Level");
    lv_table_set_cell_value(table.get(), 1, 0, "Battery Voltage");
    lv_table_set_cell_value(table.get(), 2, 0, "Temperature");
    lv_table_set_cell_value(table.get(), 3, 0, "System Voltage");
    lv_table_set_cell_value(table.get(), 4, 0, "Charging?");
    lv_table_set_cell_value(table.get(), 5, 0, "Charger Voltages");
    lv_table_set_cell_value(table.get(), 6, 0, "Charger Current");
    update();

    lv_obj_set_size(table.get(), LV_PCT(90), lv_obj_get_height(parent) - 35 - lv_obj_get_height(title.get()) - 10 - 35);
    lv_obj_align_to(table.get(), title.get(), LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
    lv_obj_set_style_bg_opa(table.get(), LV_OPA_TRANSP, LV_PART_ITEMS);
    lv_obj_set_style_text_color(table.get(), colors::PRIMARY, LV_PART_ITEMS);
    lv_obj_set_style_border_opa(table.get(), LV_OPA_TRANSP, LV_PART_ITEMS);
    lv_obj_set_style_border_color(table.get(), colors::PRIMARY, LV_PART_MAIN);
    cyberpunk_decoration_apply(table.get());
}

void info_battery::update() {
    lv_table_set_cell_value(table.get(), 0, 1, (std::to_string(instance.pmu.getBatteryPercent()) + "%").c_str());
    lv_table_set_cell_value(table.get(), 1, 1, (std::to_string(instance.pmu.getBattVoltage()) + " mV").c_str());
    lv_table_set_cell_value(table.get(), 2, 1, (std::to_string(instance.pmu.getTemperature()) + " °C").c_str());
    lv_table_set_cell_value(table.get(), 3, 1, (std::to_string(instance.pmu.getSystemVoltage()) + " mV").c_str());
    lv_table_set_cell_value(table.get(), 4, 1, instance.pmu.isCharging() ? "yes" : "no");
    lv_table_set_cell_value(table.get(), 5, 1, (std::to_string(instance.pmu.getVbusVoltage()) + " mV").c_str());
    lv_table_set_cell_value(table.get(), 6, 1, (std::to_string(instance.getChargeCurrent()) + " mA").c_str());
}

}
